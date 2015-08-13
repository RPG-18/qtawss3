#include "FolderDialog.h"
#include "ui_FolderDialog.h"

namespace Gui
{
    FolderDialog::FolderDialog(QWidget* parent) :
            QDialog(parent),
            m_ui(new Ui::FolderDialog)
    {
        m_ui->setupUi(this);
        m_ui->iconLabel->setPixmap(
                qApp->style()->standardPixmap(QStyle::SP_DirIcon).scaled(QSize(64,64)));
    }

    QString FolderDialog::folderName() const
    {
        return m_ui->name->text().trimmed();
    }

    void FolderDialog::accept()
    {
        if(!m_ui->name->text().isEmpty())
        {
            done(Accepted);
        }
    }
}
