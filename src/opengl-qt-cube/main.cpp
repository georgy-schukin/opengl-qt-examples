#include "main_window.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("OpenGL Cube Example");
    w.show();

    return a.exec();
}
