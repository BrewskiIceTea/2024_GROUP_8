#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
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
    void handleAddButton();
    void on_actionOpenFile_triggered();
    void handleTreeClicked();
    void on_actionItemOptions_triggered();
    void on_actionOpen_File_triggered(); 
    void openFileDialog();  

signals:
    void statusUpdateMessage(const QString &message, int timeout);



private:
    Ui::MainWindow *ui;
    ModelPartList* partList;

    //Worksheet 7 Exercise 3
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> renderer;
    void openDialog();
    void updateRender();
    void updateRenderFromTree(const QModelIndex& index);
};
#endif // MAINWINDOW_H
