#include <QtCore/QDebug>
#include <QtCore/QRegularExpression>

#include "ObjectInfo.h"

namespace ASSS
{
    ObjectInfo::ObjectInfo() :
            m_size(0),
            m_owner(new Owner)
    {
    }

    ObjectInfo::ObjectInfo(const QString& key):
            m_key(key),
            m_size(0)
    {
    }

    ObjectInfo::ObjectInfo(const QString& key, const QDateTime& lastModified,
            const QByteArray& etag, size_t size, OwnerPtr owner,
            QString storageClass):
                    m_key(key),
                    m_lastModified(lastModified),
                    m_etag(etag),
                    m_size(size),
                    m_owner(owner),
                    m_storageClass(storageClass)
    {
    }

    const QString& ObjectInfo::key() const
    {
        return m_key;
    }

    const QDateTime& ObjectInfo::lastModified() const
    {
        return m_lastModified;
    }

    const QByteArray& ObjectInfo::etag() const
    {
        return m_etag;
    }

    size_t ObjectInfo::size() const
    {
        return m_size;
    }

    const ObjectInfo::Owner& ObjectInfo::owner() const
    {
        return *m_owner.get();
    }


    QString ObjectInfo::extractFileName(const QString& key)
    {
        QRegularExpression re("^.*[:|/](.+)$");
        QRegularExpressionMatch match = re.match(key);
        if (match.hasMatch())
        {
            return match.captured(1);
        }

        return key;
    }

    const QString& ObjectInfo::storageClass() const
    {
        return m_storageClass;
    }

    ObjectInfo::Owner::Owner()
    {
    }

    ObjectInfo::Owner::Owner(const QByteArray& id, const QString& displayName) :
            m_id(id),
            m_displayName(displayName)
    {
    }

    const QByteArray& ObjectInfo::Owner::id() const
    {
        return m_id;
    }

    const QString& ObjectInfo::Owner::displayName() const
    {
        return m_displayName;
    }
}
