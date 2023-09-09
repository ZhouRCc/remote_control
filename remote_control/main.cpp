#include "remote.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Remote w;
    w.show();

    return a.exec();
}
