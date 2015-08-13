#include <QtWidgets/QApplication>

#include "MainWindow.h"

using namespace Gui;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);


    MainWindow window;

    window.show();
    return app.exec();
}
