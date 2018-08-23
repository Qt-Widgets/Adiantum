#include "adiantum.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    try {
        Adiantum adiantum;
        adiantum.show();
        return app.exec();
    } catch(...) {
        return 0;
    }
}
