#pragma once

#include <memory>

#include <QtWidgets/QMainWindow>

#include "Auth.h"
#include "Error.h"
#include "Endpoint.h"
#include "BucketTreeModel.h"

namespace Ui
{
    class MainWindow;
}

namespace Gui
{

    class MainWindow: public QMainWindow
    {
        Q_OBJECT

    public:

        MainWindow();

    private:

#ifdef TEST_DATASET
        void initTestModel();
#endif
        void loadSettings();
        void createToolBar();
        void getAllObject();
        void showError(const ASSS::Error& error);
        void showObjectInfo(const QModelIndex& index);

    private slots:

        void onItemClick(const QModelIndex& index);
        void showBuckets();

        void onDownload();
        void onSettings();
        void onOpenBucket();

        void onCreateFolder();
        void onUploadFile();

    private:

        std::shared_ptr<Ui::MainWindow> m_ui;
        BucketTreePtr m_tree;
        BucketTreeModel* m_treeModel;
        ASSS::Auth m_auth;
        ASSS::Endpoint m_endpoint;
        QAction* m_actionOpen;
        QString m_bucketName;
    };
}
