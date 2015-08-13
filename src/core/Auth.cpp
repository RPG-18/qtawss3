#include "Auth.h"

namespace ASSS
{
    Auth::Auth()
    {
    }

    Auth::Auth(const QString& authKey, const QString& secretKey):
        m_accessKey(authKey),
        m_secretKey(secretKey)
    {
    }

    const QString& Auth::accessKey() const
    {
        return m_accessKey;
    }

    void Auth::setAccessKey(const QString& key)
    {
        m_accessKey = key;
    }

    const QString& Auth::secretKey() const
    {
        return m_secretKey;
    }

    void Auth::setSecretKey(const QString& key)
    {
        m_secretKey = key;
    }
}
