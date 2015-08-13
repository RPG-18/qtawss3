#include <QtCore/QSettings>

#include "Settings.h"

namespace Gui
{
    class SettingsPrivate
    {
    public:

        SettingsPrivate()
        {
            restore();
        }

        ~SettingsPrivate()
        {
            save();
        }

        const QString& accessKey() const
        {
            return m_accessKey;
        }

        void setAccessKey(const QString& key)
        {
            m_accessKey = key;
        }

        const QString& secretKey() const
        {
            return m_secretKey;
        }

        void setSecretKey(const QString& key)
        {
            m_secretKey = key;
        }

        const QString& host() const
        {
            return m_host;
        }

        void setHost(const QString& host)
        {
            m_host = host;

        }

        const QString& region() const
        {
            return m_region;
        }

        void setRegion(const QString& region)
        {
            m_region = region;

        }

    private:

        void save()
        {
            QSettings state("qtawss3", "qtawss3");
            state.setValue("Settings/AccessKey", m_accessKey);
            state.setValue("Settings/SecretKey", m_secretKey);
            state.setValue("Settings/Host", m_host);
            state.setValue("Settings/Region", m_region);
        }

        void restore()
        {
            QSettings state("qtawss3", "qtawss3");
            m_accessKey = state.value("Settings/AccessKey").toString();
            m_secretKey = state.value("Settings/SecretKey").toString();
            m_host = state.value("Settings/Host").toString();
            m_region = state.value("Settings/Region").toString();
        }

    private:

        QString m_accessKey;
        QString m_secretKey;
        QString m_host;
        QString m_region;
    };

    Settings::Settings():
            m_impl(new SettingsPrivate)
    {
    }

    const QString& Settings::accessKey() const
    {
        return m_impl->accessKey();
    }

    void Settings::setAccessKey(const QString& key)
    {
        return m_impl->setAccessKey(key);
    }

    const QString& Settings::secretKey() const
    {
        return m_impl->secretKey();
    }

    void Settings::setSecretKey(const QString& key)
    {
        m_impl->setSecretKey(key);
    }

    const QString& Settings::host() const
    {
        return m_impl->host();
    }

    void Settings::setHost(const QString& host)
    {
        m_impl->setHost(host);
    }

    const QString& Settings::region() const
    {
        return m_impl->region();
    }

    void Settings::setRegion(const QString& region)
    {
        m_impl->setRegion(region);

    }
}
