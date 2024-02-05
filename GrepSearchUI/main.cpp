#include "grepui.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GrepUI w;
    w.show();
    return a.exec();
}
