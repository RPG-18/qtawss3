#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QBuffer>
#include <QtCore/QLocale>
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QUrlQuery>
#include <QtCore/QEventLoop>

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

#include "Bucket.h"
#include "Signature.h"
#include "ServiceResponse.h"

namespace ASSS
{

    class BucketPrivate: public QObject
    {
    Q_OBJECT

    public:

        BucketPrivate(const QString& bucketName) :
                m_bucketName(bucketName),
                m_networkManager(new QNetworkAccessManager(this)),
                m_loop(new QEventLoop(this))
        {
        }

        void setAuth(const Auth& auth)
        {
            m_auth = auth;
        }


        QList<QString> buckets() const
        {
            QList<QString> bucketLit;
            QUrl url("https://" + m_endpoint.host());

            QNetworkRequest request(url);
            prepareGet(request);

            auto replay = m_networkManager->get(request);

            connect(replay, &QNetworkReply::finished, [&]()
            {
                if (replay->error() == QNetworkReply::NoError)
                {
                    ServiceResponse response;
                    response.parse(replay);
                    bucketLit = response.buckets();
                }
                else
                {
                    m_lastError.fromIoDevice(replay);
                }
                m_loop->quit();
                replay->deleteLater();
            });

            m_loop->exec();
            return bucketLit;
        }

        const BucketResponse& objects(const QString& nextMarker) const
        {

            QUrl url("https://" + m_endpoint.host());
            QUrlQuery query;
            if (!nextMarker.isNull())
            {
                query.addQueryItem("marker", nextMarker);
            }

            url.setQuery(query);

            QNetworkRequest request(url);

            prepareGet(request);

            auto replay = m_networkManager->get(request);

            connect(replay, &QNetworkReply::finished, this,
                    &BucketPrivate::onGetObjects);

            m_loop->exec();
            return m_bucketResponse;
        }

        void download(const ObjectInfo& obj, const QString& fileName, Bucket::ProgressCallback progress) const
        {
            const QUrl url("https://" + m_endpoint.host() + "/" + obj.key());

            QNetworkRequest request(url);
            prepareGet(request);

            auto replay = m_networkManager->get(request);

            QFile outFile(fileName);
            outFile.open(QIODevice::ReadWrite);
            connect(replay, &QNetworkReply::readyRead, [&]()
            {
                outFile.write(replay->readAll());
            });
            connect(replay, &QNetworkReply::finished, m_loop,
                    &QEventLoop::quit);
            connect(replay, &QNetworkReply::downloadProgress,
                    progress);

            m_loop->exec();
            if (replay->error())
            {
                outFile.flush();
                outFile.seek(0);
                m_lastError.fromIoDevice(&outFile);
                outFile.close();
                QFile::remove(fileName);
            }
            else
            {
                outFile.close();
            }
            replay->deleteLater();
        }

        const Error& lastError() const
        {
            return m_lastError;

        }

        void setEndpoint(const Endpoint& endpoint)
        {
            m_endpoint = endpoint;
        }

        QByteArray upload(const QString& key, QFile& file, Bucket::ProgressCallback progress)
        {
            const QUrl url("http://" + m_endpoint.host() + "/" + key);

            QNetworkRequest request(url);
            preparePut(request, file);

            auto replay = m_networkManager->put(request, &file);

            connect(replay, &QNetworkReply::uploadProgress,
                    progress);
            connect(replay, &QNetworkReply::finished, m_loop,
                    &QEventLoop::quit);

            m_loop->exec();
            if (replay->error())
            {
                m_lastError.fromIoDevice(replay);
            }
            replay->deleteLater();
            return replay->rawHeader("ETag");
        }

        QByteArray upload(const QString& key, const QByteArray& data)
        {
            QBuffer buffer(const_cast<QByteArray*>(&data));
            buffer.open(QIODevice::ReadOnly);

            const QUrl url("http://" + m_endpoint.host() + "/" + key);

            QNetworkRequest request(url);
            preparePut(request, buffer);

            auto replay = m_networkManager->put(request, data);

            connect(replay, &QNetworkReply::finished, m_loop,
                    &QEventLoop::quit);

            m_loop->exec();
            if (replay->error())
            {
                m_lastError.fromIoDevice(replay);
            }
            replay->deleteLater();
            return replay->rawHeader("ETag");
        }

    private slots:

        void onGetObjects()
        {
            QNetworkReply* replay = qobject_cast<QNetworkReply*>(sender());
            m_loop->quit();

            if (replay->error() == QNetworkReply::NoError)
            {
                m_bucketResponse = BucketResponse();
                m_bucketResponse.parse(replay);
            }
            else
            {
                m_lastError.fromIoDevice(replay);
            }
            replay->deleteLater();
        }

    private:


        void prepareGet(QNetworkRequest& request) const
        {
            const auto dateTime = QDateTime::currentDateTime().toUTC();
            request.setRawHeader("Date", httpDate(dateTime));
            request.setRawHeader("Host",
                    QString("%1.s3.amazonaws.com").arg(m_bucketName).toUtf8());

            Signature signature(m_auth, m_endpoint.region());
            const auto dataSign = signature.sign(QByteArray());
            request.setRawHeader("x-amz-content-sha256", dataSign);

            signature.sign(request, QNetworkAccessManager::GetOperation,
                    dateTime, dataSign);
            m_lastError = Error();
        }

        void preparePut(QNetworkRequest& request, QIODevice& device)const
        {
            request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");

            const auto dateTime = QDateTime::currentDateTime().toUTC();
            request.setRawHeader("Date", httpDate(dateTime));
            request.setRawHeader("Host",
                    QString("%1.s3.amazonaws.com").arg(m_bucketName).toUtf8());

            Signature signature(m_auth, m_endpoint.region());
            const auto dataSign = signature.sign(device);
            request.setRawHeader("x-amz-content-sha256", dataSign);
            request.setRawHeader("x-amz-acl", "bucket-owner-full-control");

            signature.sign(request, QNetworkAccessManager::PutOperation,
                    dateTime, dataSign);
            m_lastError = Error();

        }

        QByteArray httpDate(const QDateTime& dt) const
        {
            const QLocale locale(QLocale::English, QLocale::UnitedStates);
            const QString httpDate = locale.toString(dt,
                    "ddd, dd MMM yyyy hh:mm:ss 'GMT+04:00'");
            return httpDate.toUtf8();
        }

    private:

        QString m_bucketName;
        Endpoint m_endpoint;
        QNetworkAccessManager* m_networkManager;
        QEventLoop* m_loop;
        Auth m_auth;
        BucketResponse m_bucketResponse;
        mutable Error m_lastError;
    };

    Bucket::Bucket(const QString& bucketName) :
            m_impl(new BucketPrivate(bucketName))
    {
    }

    const BucketResponse& Bucket::objects(const QString& nextMarker) const
    {
        return m_impl->objects(nextMarker);
    }

    void Bucket::setAuth(const Auth& auth)
    {
        m_impl->setAuth(auth);
    }

    void Bucket::download(const ObjectInfo& obj, const QString& fileName, ProgressCallback progress) const
    {
        m_impl->download(obj, fileName, progress);
    }

    void Bucket::setEndpoint(const Endpoint& endpoint)
    {
        m_impl->setEndpoint(endpoint);
    }

    const Error& Bucket::lastError() const
    {
        return m_impl->lastError();
    }

    QByteArray Bucket::upload(const QString& key, QFile& file, ProgressCallback progress)
    {
        return m_impl->upload(key, file, progress);
    }

    QByteArray Bucket::upload(const QString& key, const QByteArray& data)
    {
        return m_impl->upload(key, data);
    }

    QList<QString> Bucket::buckets() const
    {
        return m_impl->buckets();
    }
}

#include "Bucket.moc"
