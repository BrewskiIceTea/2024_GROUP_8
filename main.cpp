#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // adding a stylesheet to the application
    QFile file("style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QString style = stream.readAll();
        a.setStyleSheet(style); // or QApplication::setStyleSheet(style);
        file.close();
        qDebug() << "opened sheet.";
    } else {
        qDebug() << "Could not open stylesheet file.";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
