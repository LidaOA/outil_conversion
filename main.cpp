#include "conversion.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WindowConversion w;
    w.show();
    return a.exec();
}
