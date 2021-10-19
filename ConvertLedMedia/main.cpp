#include "ConvertLedMedia.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConvertLedMedia w;
    w.show();
    return a.exec();
}
