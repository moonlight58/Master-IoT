#include "widget.h"
#include <QApplication>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::srand(static_cast<unsigned>(std::time(nullptr))); // WITHOUT this, "random" is identical every run
    Widget w;
    w.show();
    return QApplication::exec();
}