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

#include "VRRenderThread.h"


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
    void handleTreeClicked();

    // Actions
    void on_actionOpenFile_triggered();
    void on_actionItemOptions_triggered();
    void on_actionStart_VR_triggered();
    void on_actionStop_VR_triggered();
    
    // Generic open file dialog for loading STL files
    void openFileDialog(); 

signals:
    void statusUpdateMessage(const QString &message, int timeout);



private:
    Ui::MainWindow *ui;
    ModelPartList* partList;

    VRRenderThread* vrThread = nullptr; // Pointer to the VR thread

    // Renderer and window
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> renderer;

    void openDialog();
    void updateRender();
    void updateRenderFromTree(const QModelIndex& index);
    void updateAllThreadActors();

    bool steamVRAvailable();
};
#endif // MAINWINDOW_H
