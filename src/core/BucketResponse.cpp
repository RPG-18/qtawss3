#include <QtCore/QMap>
#include <QtCore/QDebug>
#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamReader>

#include "BucketResponse.h"

namespace ASSS
{
    class BucketResponsePrivate
    {
    public:

        BucketResponsePrivate():
            m_isTruncated(false),
            m_maxKeys(0)
        {
        }

        void parse(QIODevice* device)
        {
            QXmlStreamReader stream(device);

            parseHead(stream);

            while (!stream.atEnd())
            {
                stream.readNextStartElement();
                if (stream.name() == "Contents")
                {
                    parseContents(stream);
                }
            }
        }

        void parseHead(QXmlStreamReader& stream)
        {

            stream.readNextStartElement();
            if(stream.name() != "ListBucketResult")
            {
                //TODO throw exception
            }

            stream.readNextStartElement();
            m_bucketName = stream.readElementText();

            stream.readNextStartElement();
            m_prefix = stream.readElementText();

            stream.readNextStartElement();
            m_marker = stream.readElementText();

            stream.readNextStartElement();
            m_maxKeys = stream.readElementText().toUInt();

            stream.readNextStartElement();
            m_isTruncated = stream.readElementText() == "true";
        }

        void parseContents(QXmlStreamReader& stream)
        {
            stream.readNextStartElement();
            const QString key = stream.readElementText();

            stream.readNextStartElement();
            const auto lastModified = QDateTime::fromString(
                    stream.readElementText(), Qt::ISODate);

            stream.readNextStartElement();
            const auto etag = QByteArray::fromHex(
                    stream.readElementText().toLatin1());

            stream.readNextStartElement();
            const size_t size = stream.readElementText().toUInt();

            stream.readNextStartElement(); //Owner
            stream.readNextStartElement();
            auto ownerId = stream.readElementText().toLatin1();
            ownerId = QByteArray::fromHex(ownerId);

            stream.readNextStartElement();
            const QString ownerDisplayName = stream.readElementText();

            ObjectInfo::OwnerPtr owner;
            if (m_owrners.contains(ownerId))
            {
                owner = m_owrners[ownerId];
            }
            else
            {
                owner = std::make_shared<ObjectInfo::Owner>(ownerId,
                        ownerDisplayName);
            }
            stream.skipCurrentElement();

            stream.readNextStartElement();
            const QString storageClass = stream.readElementText();
            m_buckets.push_back(ObjectInfo(
                    key,
                    lastModified,
                    etag,
                    size,
                    owner,
                    storageClass));
            while (stream.readNext() == QXmlStreamReader::StartElement)
            {
            }
        }

        const ObjectInfoList& objects() const
        {
            return m_buckets;
        }

        const QString& bucketName() const
        {
            return m_bucketName;
        }

        const QString& prefix() const
        {
            return m_prefix;
        }

        const QString& makrer() const
        {
            return m_marker;
        }

        size_t maxKeys() const
        {
            return m_maxKeys;
        }

        bool isTruncated() const
        {
            return m_isTruncated;
        }

    private:

        QString m_bucketName;
        QString m_prefix;
        QString m_marker;
        size_t m_maxKeys;
        bool m_isTruncated;
        ObjectInfoList m_buckets;
        QMap<QByteArray, ObjectInfo::OwnerPtr> m_owrners;
    };

    BucketResponse::BucketResponse() :
            m_impl(new BucketResponsePrivate)
    {
    }

    void BucketResponse::parse(QIODevice* device)
    {
        m_impl->parse(device);
    }

    const ObjectInfoList& BucketResponse::objects() const
    {
        return m_impl->objects();
    }

    const QString& BucketResponse::bucketName() const
    {
        return m_impl->bucketName();
    }

    const QString& BucketResponse::prefix() const
    {
        return m_impl->prefix();
    }

    const QString& BucketResponse::makrer() const
    {
        return m_impl->makrer();
    }

    size_t BucketResponse::maxKeys() const
    {
        return m_impl->maxKeys();
    }

    bool BucketResponse::isTruncated() const
    {
        return m_impl->isTruncated();
    }
}
