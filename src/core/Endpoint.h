#pragma once

#include <QtCore/QString>

namespace ASSS
{

    class Endpoint
    {
    public:
        Endpoint(){};

        Endpoint(const QString& region,
                 const QString& host);

        const QString& region() const;
        const QString& host() const;

    private:
        QString m_region;
        QString m_host;
    };
}
