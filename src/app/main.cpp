#include "MainWindow.h"
#include <QApplication>
#include <Kpf/Kpf.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!kpfCore.init(argc, argv)) {
        return -1;
    }

    return app.exec();
}
