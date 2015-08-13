#pragma once

#include <QtCore/QMap>
#include <QtCore/QString>

class QIODevice;
namespace ASSS
{
    class Error
    {
    public:

        bool isEmpty()const;
        void fromIoDevice(QIODevice* device);

        const QString code() const;
        const QString message() const;

        QList<QString> otherFields() const;
        QString field(const QString& key) const;

    private:

        QMap<QString, QString> m_fields;
    };
}
