#include "adiantum.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    try {
        Adiantum adiantum;
        adiantum.show();
        app.setQuitOnLastWindowClosed(false);
        return app.exec();
    } catch(...) {
        return 0;
    }
}
