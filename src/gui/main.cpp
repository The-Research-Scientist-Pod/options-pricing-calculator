//
// Created by Yusufu Shehu on 18/01/2025.
//
#include "main_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application info
    QApplication::setApplicationName("Options Pricer");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("YourCompany");
    QApplication::setOrganizationDomain("yourcompany.com");

    MainWindow window;
    window.resize(1024, 768);
    window.show();

    return QApplication::exec();
}