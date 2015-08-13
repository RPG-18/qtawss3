#include <array>

#include "Settings.h"
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

namespace Gui
{

    SettingsDialog::SettingsDialog(QWidget* parent) :
            QDialog(parent),
            m_ui(new Ui::SettingsDialog)
    {
        m_ui->setupUi(this);
        restoreState();
    }

    QString SettingsDialog::accessKey() const
    {
        return m_ui->accesKey->text().trimmed();
    }

    QString SettingsDialog::secretKey() const
    {
        return m_ui->secretKey->text().trimmed();
    }

    QString SettingsDialog::host() const
    {
        return m_ui->host->text().trimmed();
    }

    QString SettingsDialog::region() const
    {
        return m_ui->region->text().trimmed();
    }

    void SettingsDialog::accept()
    {
        const std::array<QLineEdit*, 4> widgets =
        {
                m_ui->accesKey,
                m_ui->secretKey,
                m_ui->host,
                m_ui->region
        };
        bool notEmptys = true;
        for (auto widget : widgets)
        {
            notEmptys = notEmptys && !widget->text().isEmpty();
        }
        if (notEmptys)
        {
            saveState();
            done(QDialog::Accepted);
        }
    }

    void SettingsDialog::saveState()
    {
        Settings state;
        state.setAccessKey(accessKey());
        state.setSecretKey(secretKey());
        state.setHost(host());
        state.setRegion(region());
    }

    void SettingsDialog::restoreState()
    {
        Settings state;
        m_ui->accesKey->setText(state.accessKey());
        m_ui->secretKey->setText(state.secretKey());
        m_ui->host->setText(state.host());
        m_ui->region->setText(state.region());
    }
}
