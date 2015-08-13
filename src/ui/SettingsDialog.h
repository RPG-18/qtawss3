#pragma once

#include <memory>

#include <QtWidgets/QDialog>

namespace Ui
{
    class SettingsDialog;
}

namespace Gui
{

    class SettingsDialog: public QDialog
    {
        Q_OBJECT

    public:

        SettingsDialog(QWidget* parent = nullptr);

        QString accessKey() const;
        QString secretKey() const;
        QString host() const;
        QString region() const;

    public slots:

        virtual void accept();

    private:

        void saveState();
        void restoreState();

    private:

        std::shared_ptr<Ui::SettingsDialog> m_ui;
    };

}
