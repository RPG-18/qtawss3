#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamReader>

#include "ServiceResponse.h"

namespace ASSS
{
    class ServiceResponsePrivate
    {
    public:
        void parse(QIODevice* device)
        {
            QXmlStreamReader stream(device);

            while (!stream.atEnd())
            {
                stream.readNextStartElement();
                if (stream.name() == "Name")
                {
                    m_buckets.push_back(stream.readElementText());
                }
            }

        }

        const QList<QString>& buckets() const
        {
            return m_buckets;
        }

    private:

        QList<QString> m_buckets;
    };

    ServiceResponse::ServiceResponse() :
            m_impl(new ServiceResponsePrivate)
    {

    }

    void ServiceResponse::parse(QIODevice* device)
    {
        m_impl->parse(device);
    }

    const QList<QString>& ServiceResponse::buckets() const
    {
        return m_impl->buckets();
    }
}
