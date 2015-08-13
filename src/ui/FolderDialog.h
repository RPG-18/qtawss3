#pragma once

#include <memory>

#include <QtWidgets/QDialog>

namespace Ui
{
    class FolderDialog;
}

namespace Gui
{

    class FolderDialog: public QDialog
    {
    Q_OBJECT
    public:

        FolderDialog(QWidget* parent = nullptr);

        QString folderName() const;

    public slots:

        virtual void accept();

    private:

        std::shared_ptr<Ui::FolderDialog> m_ui;
    };
}
