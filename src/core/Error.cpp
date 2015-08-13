#include <QtCore/QDebug>
#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamReader>

#include "Error.h"

namespace ASSS
{

    bool Error::isEmpty() const
    {
        return m_fields.isEmpty();
    }

    void Error::fromIoDevice(QIODevice* device)
    {
        QXmlStreamReader stream(device);
        stream.readNextStartElement();
        if (stream.name() != "Error")
        {
            return;
        }

        while (!stream.atEnd())
        {
            stream.readNextStartElement();
            m_fields[stream.name().toString()] = stream.readElementText();
        }
    }

    const QString Error::code() const
    {
        return m_fields["Code"];
    }

    const QString Error::message() const
    {
        return m_fields["Message"];
    }

    QList<QString> Error::otherFields() const
    {
        QList<QString> fields;
        for (auto key : m_fields.keys())
        {
            if (key == "Code" || key == "Message")
            {
                continue;
            }
            fields.push_back(key);
        }

        return fields;
    }

    QString Error::field(const QString& key) const
    {
        return m_fields[key];
    }
}
