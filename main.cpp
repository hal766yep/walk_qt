#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
#if defined(__clang__)
        qDebug() << "Compiler: Clang\n";
#elif defined(__GNUC__)
    qDebug() << "Compiler: GCC\n";
#elif defined(_MSC_VER)
    qDebug() << "Compiler: MSVC\n";
#else
    qDebug() << "Unknown Compiler\n";
#endif
    w.setWindowTitle("VN 70x56阵列信号采集系统");
    w.show();
    return a.exec();
}
