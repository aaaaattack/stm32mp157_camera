#include "widget.h"
#include "hotplug.h"
#include "button.h"
#include <QApplication>
#include "v4l2.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    AppDemo w;
    w.show();


    return a.exec();
}
