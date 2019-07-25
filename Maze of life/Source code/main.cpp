#include "Lifesmaze.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Lifesmaze w;
    w.show();
    return a.exec();
}
