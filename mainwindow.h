#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
#include "ModelPart.h"
#include "ModelPartList.h"

#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>

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

/**
 * @file MainWindow.h
 * @brief Defines the MainWindow class for the 3D/VR application
 * @note This file contains the main window class with UI setup, event handlers and rendering/VR management for the application
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief Constructs the MainWindow
     * @param parent Optional parent widget
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroys the MainWindow and frees resources
     */
    ~MainWindow();

    

public slots:

    /**
     * @brief Handles a click event on the tree view
     */
    void handleTreeClicked();

    // -------------------------------- Actions ----------------------------------

    /**
     * @brief Opens a file dialog to load an STL file
     */
    void on_actionOpen_Dir_triggered();

    /**
     * @brief Opens ItemOptions dialog to edit model part display properties
     */
    void on_actionItemOptions_triggered();

    /**
     * @brief Starts the VR rendering thread
     */
    void on_actionStart_VR_triggered();

    /**
     * @brief Stops the VR rendering thread
     */
    void on_actionStop_VR_triggered();

    /**
     * @brief Opens the filter options dialog.
     * @note this is for filter options NOT item options
     */
    void on_actionFilterOptions_triggered(); //filter OPtions NOT itemOptions


    void on_actionReplace_Part_triggered();

    void on_actionRemove_Part_triggered();
    
    // // Generic open file dialog for loading STL file
    // /**
    //  * @brief Opens a generic file dialog for loading STL files
    //  * @note this is for item options not filter options
    //  */
    // void openFileDialog();      //for item options

    /**
     * @brief Opens the filter dialog for adjusting filters
     * @note this is for filter options not item options
     */
    void openFilterDialog();    //filter OPtions not itemOptions


signals:
    /**
     * @brief Emits a status update message to be displayed in the status bar
     * @param message The message string
     * @param timeout The display duration in milliseconds
     */
    void statusUpdateMessage(const QString &message, int timeout);



private:
    Ui::MainWindow *ui;                              /**< Pointer to the user interface elements */
    ModelPartList* partList;                        /**< List of model parts */

    VRRenderThread* vrThread = nullptr;            /**< Pointer to the VR rendering thread */

    // Renderer and window
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow; /**< OpenGL render window */
    vtkSmartPointer<vtkRenderer> renderer;                     /**< Main renderer */
    vtkSmartPointer<vtkLight> mainLight;                      /**< Main scene light */


    /**
     * @brief Loads all STL files from a selected directory into the tree view and both renderers
     *
     * Opens a directory selection dialog and recursively searches for STL files.
     * Creates a new ModelPartList and replaces existing ones and populates it with found models.
     * Updates the tree view with the new model hierarchy.
     */
    void loadFolderAsTree();

    /**
     * @brief Opens the item options dialog window
     */
    void openItemOptionsDialog();


    void replaceSelectedPart();

    void removeSelectedPart();

    
    void addNewPart();

    /**
     * @brief Updates the 3D rendering window
     */
    void updateRender();

    /**
     * @brief Updates the rendering based on the selected tree item
     * @param index The model index of the selected tree item
     */
    void updateRenderFromTree(const QModelIndex& index);

    /**
     * @brief Updates all VR thread actors to reflect current state
     */
    void updateAllThreadActors();

    /**
     * @brief Checks if SteamVR is available on the system
     * @return True if SteamVR is available; false otherwise
     */
    bool steamVRAvailable();
};
#endif // MAINWINDOW_H
