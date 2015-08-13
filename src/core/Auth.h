#pragma once

#include <QtCore/QString>

namespace ASSS
{

    class Auth
    {
    public:

        Auth();

        Auth(const QString& accessKey, const QString& secretKey);

        const QString& accessKey() const;
        void setAccessKey(const QString& key);

        const QString& secretKey() const;
        void setSecretKey(const QString& key);

    private:

        QString m_accessKey;
        QString m_secretKey;
    };

}
