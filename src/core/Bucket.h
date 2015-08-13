#pragma once

#include <memory>
#include <functional>

#include "Auth.h"
#include "Error.h"
#include "Endpoint.h"
#include "BucketResponse.h"

class QFile;
namespace ASSS
{
    class BucketPrivate;
    class Bucket
    {
    public:

        typedef std::function<void(qint64, qint64)> ProgressCallback;

        Bucket(const QString& bucketName = QString());

        void setAuth(const Auth& auth);
        void setEndpoint(const Endpoint& endpoint);

        QList<QString> buckets() const;
        const BucketResponse& objects(const QString& nextMarker = QString()) const;

        void download(const ObjectInfo& obj, const QString& fileName, ProgressCallback progress = [](qint64, qint64){}) const;
        QByteArray upload(const QString& key, QFile& file, ProgressCallback progress = [](qint64, qint64){});
        QByteArray upload(const QString& key, const QByteArray& data = QByteArray());

        const Error& lastError() const;

    private:
        std::shared_ptr<BucketPrivate> m_impl;
    };
}
