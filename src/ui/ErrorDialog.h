#pragma once

#include <memory>

#include <QtWidgets/QDialog>

#include "Error.h"

namespace Ui
{
    class ErrorDialog;
}
namespace Gui
{

    class ErrorDialog:public QDialog
    {
        Q_OBJECT
    public:

        ErrorDialog(const ASSS::Error& error,
                QWidget * parent = nullptr);
    private:

        void showError(const ASSS::Error& error);

    private:

        std::shared_ptr<Ui::ErrorDialog> m_ui;

    };

}
