#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ModelPart.h"
#include "ModelPartList.h"
#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void handleButton_1(); //Slot for first button
    void handleButton_2(); //Slot for second button
    void handleTreeClicked(); //Slot for Tree Clicks

signals:
    void statusUpdateMessage(const QString &message, int timeout);


private slots:
    void on_actionOpen_File_triggered();
    void openFileDialog(); //Slot for opening a file Ex8

    void openDialog();
    void on_actionItemOptions_triggered();


private:
    Ui::MainWindow *ui;
    ModelPartList* partList;

    //Worksheet 7 Exercise 3
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> renderer;

    void updateRender();
    void updateRenderFromTree(const QModelIndex& index);
};
#endif // MAINWINDOW_H
