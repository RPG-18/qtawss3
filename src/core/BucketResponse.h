#pragma once

#include <memory>

#include "ObjectInfo.h"

class QIODevice;

namespace ASSS
{
    class BucketResponsePrivate;
    class BucketResponse
    {
    public:

        BucketResponse();

        void parse(QIODevice* device);

        const ObjectInfoList& objects() const;

        const QString& bucketName() const;
        const QString& prefix() const;
        const QString& makrer() const;
        size_t maxKeys() const;
        bool isTruncated() const;

    private:

        std::shared_ptr<BucketResponsePrivate> m_impl;
    };
}
