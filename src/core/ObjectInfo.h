#pragma once

#include <memory>

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QByteArray>

namespace ASSS
{

    class ObjectInfo
    {
    public:
        class Owner;
        typedef std::shared_ptr<Owner> OwnerPtr;


        ObjectInfo();

        // fot tests
        ObjectInfo(const QString& key);

        ObjectInfo(const QString& key,
                   const QDateTime& lastModified,
                   const QByteArray& etag,
                   size_t size,
                   OwnerPtr owner,
                   QString storageClass);

        const QString& key() const;
        const QDateTime& lastModified() const;
        const QByteArray& etag() const;
        size_t size() const;
        const Owner& owner() const;
        const QString& storageClass() const;

        static QString extractFileName(const QString& key);

    private:

        QString m_key;
        QDateTime m_lastModified;
        QByteArray m_etag;
        size_t m_size;
        OwnerPtr m_owner;
        QString m_storageClass;
    };

    class ObjectInfo::Owner
    {
    public:

        Owner();

        Owner(const QByteArray& id, const QString& displayName);

        const QByteArray& id() const;
        const QString& displayName() const;
    private:

        QByteArray m_id;
        QString m_displayName;
    };

    typedef QList<ObjectInfo> ObjectInfoList;
}
