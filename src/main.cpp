#include "TestExecutor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TestExecutor w;
    w.show();
    return a.exec();
}
