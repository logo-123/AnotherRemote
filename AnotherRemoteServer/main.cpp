#include "mainwindow.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QString>
#include <QtGlobal>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString UniqueID("AnotherRemoteServer_SharedMemory_ID");
    
    QSharedMemory sharedMemory;
    sharedMemory.setKey(UniqueID);
    if (true == sharedMemory.attach())
    {
        QMessageBox::critical(NULL, "AnotherRemoteServer", "Application allready running");
        return 1;
    }

    if (false == sharedMemory.create(1))
    {
        QMessageBox::critical(NULL, "AnotherRemoteServer", "Application allready running");
        return 1;
    }

    QWidget widget;
    MainWindow w(&widget);
    
    return a.exec();
}
