#include "LC3.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    lc3 w;
    w.show();
    return a.exec();
}
