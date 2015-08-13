#include <QtCore/QTextStream>

#include "ErrorDialog.h"
#include "ui_ErrorDialog.h"

namespace Gui
{
    ErrorDialog::ErrorDialog(const ASSS::Error& error,
            QWidget* parent) :
            QDialog(parent),
            m_ui(new Ui::ErrorDialog)
    {
        m_ui->setupUi(this);
        showError(error);
    }

    void ErrorDialog::showError(const ASSS::Error& error)
    {
        setWindowTitle(tr("Error") +": "+error.code());
        m_ui->message->setText(error.message());

        QString desctiption;
        QTextStream stream(&desctiption,QIODevice::WriteOnly);

        for(auto key: error.otherFields())
        {
            stream<<"--- "<<key<<endl;
            stream<<error.field(key)<<endl;
        }

        m_ui->description->setText(desctiption);
    }

}
