#include "Endpoint.h"

namespace ASSS
{
    Endpoint::Endpoint(const QString& region, const QString& host) :
            m_region(region),
            m_host(host)
    {

    }

    const QString& Endpoint::region() const
    {
        return m_region;
    }

    const QString& Endpoint::host() const
    {
        return m_host;
    }

}
