#pragma once

#include <memory>

#include <QtCore/QString>

namespace Gui
{
    class SettingsPrivate;
    class Settings
    {
    public:

        Settings();

        const QString& accessKey() const;
        void setAccessKey(const QString& key);

        const QString& secretKey() const;
        void setSecretKey(const QString& key);

        const QString& host() const;
        void setHost(const QString& host);

        const QString& region() const;
        void setRegion(const QString& region);

    private:
        std::shared_ptr<SettingsPrivate> m_impl;
    };

}
