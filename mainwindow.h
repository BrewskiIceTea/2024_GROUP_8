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

#include <vtkCylinderSource.h>
#include <vtkPlane.h>
#include <vtkClipPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>


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
    void on_actionOpen_Dir_triggered();     //for item options
    void on_actionItemOptions_triggered();
    void on_actionStart_VR_triggered();
    void on_actionStop_VR_triggered();
    void on_actionFilterOptions_triggered(); //filter OPtions not itemOptions
    void on_actionReplace_Part_triggered();  
    
    // Generic open file dialog for loading STL files
    void openFilterDialog();    //filter OPtions not itemOptions


signals:
    void statusUpdateMessage(const QString &message, int timeout);



private:
    Ui::MainWindow *ui;
    ModelPartList* partList;

    VRRenderThread* vrThread = nullptr; // Pointer to the VR thread

    // Renderer and window
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkLight> mainLight;

/**
 * @brief Loads all STL files from a selected directory into the tree view and both renderers
 * 
 * Opens a directory selection dialog and recursively searches for STL files.
 * Creates a new ModelPartList and replaces existing ones and populates it with found models.
 * Updates the tree view with the new model hierarchy.
 */
    void loadFolderAsTree();


    void replaceSelectedPart();
    void openDialog();
    void addNewPart();
    void updateRender();
    void updateRenderFromTree(const QModelIndex& index);
    void updateAllThreadActors();

    bool steamVRAvailable();
};
#endif // MAINWINDOW_H
