#include <chrono>
#include <thread>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QEventLoop>
#include <QtCore/QModelIndex>
#include <QtCore/QStandardPaths>

#include <QtCore/QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include <QtGui/QDesktopServices>

#include <QtWidgets/QAction>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QApplication>
#include <QtWidgets/QProgressDialog>

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Bucket.h"
#include "Settings.h"
#include "ErrorDialog.h"
#include "KeyDelegate.h"
#include "FolderDialog.h"
#include "SettingsDialog.h"
#include "BucketResponse.h"
#include "ObjectTree.h"

using namespace ASSS;
namespace Gui
{
    MainWindow::MainWindow() :
            m_ui(new Ui::MainWindow),
            m_treeModel(new ObjectTreeModel(this)),
            m_actionOpen(new QAction(this))
    {
        m_ui->setupUi(this);
        m_ui->treeView->setItemDelegate(new KeyDelegate(this));
        m_ui->treeView->setModel(m_treeModel);

        loadSettings();
        createToolBar();

        m_actionOpen->setIcon(
        qApp->style()->standardIcon(QStyle::SP_DriveNetIcon));
        m_actionOpen->setText(tr("&Show buckets"));
        connect(m_actionOpen, SIGNAL(triggered()), this, SLOT(showBuckets()));
#ifdef TEST_DATASET
        initTestModel();
#endif
    }

#ifdef TEST_DATASET
    void MainWindow::initTestModel()
    {
        QString dataDir = QLatin1String(DATA_DIR);

        QFile ans(dataDir + "/ans.xml");
        qDebug() << "Test data open: "
        << ans.open(QIODevice::ReadOnly | QIODevice::Text);

        BucketResponse response;

        response.parse(&ans);

        m_tree = std::make_shared<ObjectTree>();
        m_tree->build(response.objects());

        m_treeModel->setTree(m_tree);
    }
#endif

    void MainWindow::onItemClick(const QModelIndex & index)
    {
        if (m_treeModel->nodeType(index) == ObjectTree::TreeNode::Node)
        {
            m_ui->download->setDisabled(true);
            m_ui->uploadFile->setEnabled(true);
            m_ui->createFolder->setEnabled(true);
            return;
        }
        else if (m_treeModel->nodeType(index) == ObjectTree::TreeNode::Leaf)
        {
            m_ui->download->setEnabled(true);
            m_ui->uploadFile->setDisabled(true);
            m_ui->createFolder->setDisabled(true);
            showObjectInfo(index);
        }
    }

    void MainWindow::showObjectInfo(const QModelIndex& index)
    {
        const int objPos = m_treeModel->objPosition(index);
        const auto& obj = m_tree->object(objPos);

        m_ui->key->setPlainText(obj.key());
        m_ui->size->setText(QString::number(obj.size()));
        m_ui->etag->setPlainText(obj.etag().toHex());
        m_ui->storageClass->setText(obj.storageClass());
        m_ui->lastModified->setText(obj.lastModified().toString());

        m_ui->ownerName->setText(obj.owner().displayName());
        m_ui->ownerId->setPlainText(obj.owner().id().toHex());
    }

    void MainWindow::onDownload()
    {
        int pos = m_treeModel->objPosition(m_ui->treeView->currentIndex());
        const auto& obj = m_tree->object(pos);
        const auto fileName = ObjectInfo::extractFileName(obj.key());
        QDir donwloadPath = QStandardPaths::standardLocations(
                QStandardPaths::DownloadLocation).first();

        auto outFileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                donwloadPath.absoluteFilePath(fileName));
        if (!outFileName.isNull() && !outFileName.isEmpty())
        {
            QProgressDialog dialog(this);
            dialog.setRange(0, 0);
            dialog.setLabelText(tr("Downloading") + " " + fileName);
            dialog.setCancelButton(0);
            dialog.setModal(true);
            dialog.show();

            Bucket bucket(m_bucketName);
            bucket.setEndpoint(m_endpoint);
            bucket.setAuth(m_auth);
            bucket.download(obj, outFileName, [&dialog](qint64 pos, qint64 all){
                dialog.setValue(pos);
                dialog.setMaximum(all);
            });
            if (!bucket.lastError().isEmpty())
            {
                showError(bucket.lastError());
            }
            else
            {
                QDesktopServices::openUrl(
                        QUrl("file://" + outFileName, QUrl::TolerantMode));
            }
        }
    }

    void MainWindow::onOpenBucket()
    {
        const QString bucket = m_ui->buckets->currentText();
        if (bucket.isEmpty())
        {
            return;
        }
        m_bucketName = bucket;
        QProgressDialog dialog(this);
        dialog.setRange(0, 0);
        dialog.setLabelText(tr("Getting a list of objects"));
        dialog.setCancelButton(0);
        dialog.setModal(true);
        dialog.show();

        m_ui->centralwidget->setDisabled(true);
        getAllObject();
        m_ui->centralwidget->setEnabled(true);
    }

    void MainWindow::getAllObject()
    {

        Bucket bucket(m_bucketName);
        bucket.setEndpoint(m_endpoint);
        bucket.setAuth(m_auth);

        QString marker;
        bool next = false;
        ObjectInfoList buckets;
        while (true)
        {
            const auto response = bucket.objects(marker);
            if (!bucket.lastError().isEmpty())
            {
                showError(bucket.lastError());
            }
            if (response.objects().isEmpty())
            {
                break;
            }
            buckets.append(response.objects());

            marker = response.objects().last().key();
        };

        m_tree = std::make_shared<ObjectTree>();

        QFutureWatcher<void> watcher;
        QEventLoop loop;
        connect(&watcher, &QFutureWatcher<void>::finished, &loop,
                &QEventLoop::quit);

        watcher.setFuture(
                QtConcurrent::run(m_tree.get(), &ObjectTree::build, buckets));
        loop.exec();
        m_tree->setBucketName(m_bucketName);
        m_treeModel->setTree(m_tree);
    }

    void MainWindow::onSettings()
    {
        SettingsDialog settings(this);
        if (settings.exec() != QDialog::Accepted)
        {
            return;
        }

        m_auth.setAccessKey(settings.accessKey());
        m_auth.setSecretKey(settings.secretKey());
        m_endpoint = Endpoint(settings.region(), settings.host());
    }

    void MainWindow::createToolBar()
    {
        QToolBar* toolBar = addToolBar(tr("Open"));
        toolBar->addAction(m_actionOpen);
    }

    void MainWindow::showBuckets()
    {
        QProgressDialog dialog(this);
        dialog.setRange(0, 0);
        dialog.setLabelText(tr("Getting a list of baskets"));
        dialog.setCancelButton(0);
        dialog.setModal(true);
        dialog.show();

        Bucket bucket;
        bucket.setEndpoint(m_endpoint);
        bucket.setAuth(m_auth);

        m_ui->buckets->addItems(bucket.buckets());

        if (!bucket.lastError().isEmpty())
        {
            showError(bucket.lastError());
        }
    }

    void MainWindow::loadSettings()
    {
        Settings state;

        m_auth.setAccessKey(state.accessKey());
        m_auth.setSecretKey(state.secretKey());
        m_endpoint = Endpoint(state.region(), state.host());
    }

    void MainWindow::showError(const ASSS::Error& error)
    {
        ErrorDialog dialog(error, this);
        dialog.setModal(true);
        dialog.exec();
    }

    void MainWindow::onCreateFolder()
    {

        FolderDialog dialog(this);
        dialog.setModal(true);
        if (dialog.exec() != QDialog::Accepted)
        {
            return;
        }

        const auto modelIndex = m_ui->treeView->currentIndex();
        const auto path = m_treeModel->path(modelIndex);
        QString key = dialog.folderName();
        if (key[key.size() - 1] != '/')
        {

            key += '/';
        }

        key = path + "/" + key;

        Bucket bucket(m_bucketName);
        bucket.setEndpoint(m_endpoint);
        bucket.setAuth(m_auth);
        const auto etag = bucket.upload(key);

        if (bucket.lastError().isEmpty())
        {
            ObjectInfo obj(key, QDateTime::currentDateTime(), QByteArray(), 0,
                    std::make_shared<ObjectInfo::Owner>(), QString("STANDARD"));
            m_treeModel->insert(modelIndex, obj);
        }
        else
        {
            showError(bucket.lastError());
        }
    }

    void MainWindow::onUploadFile()
    {
        const auto uploadFile = QFileDialog::getOpenFileName(this,
                tr("Upload file"));
        if (uploadFile.isEmpty())
        {
            return;
        }
        const auto modelIndex = m_ui->treeView->currentIndex();
        auto key = m_treeModel->path(modelIndex);
        QFileInfo info(uploadFile);
        key += "/" + info.fileName();

        QProgressDialog dialog(this);
        dialog.setRange(0, info.size());
        dialog.setLabelText(tr("Upload") + ": " + info.fileName());
        dialog.setCancelButton(0);
        dialog.setModal(true);
        dialog.show();

        Bucket bucket(m_bucketName);
        bucket.setEndpoint(m_endpoint);
        bucket.setAuth(m_auth);
        QFile file(uploadFile);
        file.open(QIODevice::ReadOnly);
        const auto etag = bucket.upload(key, file, [&dialog](qint64 pos, qint64 all){
            dialog.setValue(pos);
        });

        if (bucket.lastError().isEmpty())
        {
            ObjectInfo obj(key, info.lastModified(), etag, info.size(),
                    std::make_shared<ObjectInfo::Owner>(), QString("STANDARD"));
            m_treeModel->insert(modelIndex, obj);
        }
        else
        {
            showError(bucket.lastError());
        }
    }
}
