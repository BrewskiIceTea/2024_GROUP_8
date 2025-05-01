#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "optiondialog.h"
#include "filterdialog.h"


#include <QMessageBox>
#include <QFileDialog>


#include <vtkRenderer.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkProperty.h>

#include <vtkGenericOpenGLRenderWindow.h>

#include "VRRenderThread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // -------------------------------- SETUP UI ----------------------------------

    ui->setupUi(this);
    ui->treeView->addAction(ui->actionItemOptions);
    ui->treeView->addAction(ui->actionFilterOptions);   //for filter options
    
    ui->actionStart_VR->setEnabled(true); // Enable the Start VR action button
    ui->actionStop_VR->setEnabled(false); // Disable the Stop VR action button

    bool checkConnect; 
    // Connect signals and slots
    checkConnect = connect(ui->pushButton, &QPushButton::released, this, &MainWindow::handleAddButton);
    Q_ASSERT(checkConnect);

    checkConnect = connect(ui->pushButton_2, &QPushButton::released, this, &MainWindow::on_actionItemOptions_triggered);
    Q_ASSERT(checkConnect);

    //Ex5 ClickedSignalOfTree Constructor
    checkConnect = connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    Q_ASSERT(checkConnect);

    // Connect a custom signal to the status bar
    checkConnect = connect(this, &MainWindow::statusUpdateMessage, ui->statusbar1, &QStatusBar::showMessage);
    Q_ASSERT(checkConnect);

    // Connect For openning file dialog in menubar
    checkConnect =  connect(ui->actionOpen_File, &QAction::triggered, this, &MainWindow::on_actionOpenFile_triggered);
    Q_ASSERT(checkConnect);  

    // Start VR Action
    checkConnect =  connect(ui->actionStart_VR, &QAction::triggered, this, &MainWindow::on_actionStart_VR_triggered);
    Q_ASSERT(checkConnect);  

    // Stop VR Action
    checkConnect =  connect(ui->actionStop_VR, &QAction::triggered, this, &MainWindow::on_actionStop_VR_triggered);
    Q_ASSERT(checkConnect);

    // -------------------------------- VTK RENDERING ----------------------------------

    // Exercise 3, Worksheet 7 Task
    // Link a render window with the Qt widget
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

    // Assigning the Desktop application widget to the render window
    ui->vtkWidget->setRenderWindow(renderWindow);

    
    // Add a renderer
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    // Create an object and add to renderer (temporary: displaying a cylinder)
    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(8);

    // The mapper is responsible for pushing geometry into the graphics library
    vtkNew<vtkPolyDataMapper> cylinderMapper;
    cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

    // The actor groups the geometry (mapper) and also has properties like color and transformation
    vtkNew<vtkActor> cylinderActor;
    cylinderActor->SetMapper(cylinderMapper);
    cylinderActor->GetProperty()->SetColor(1.0, 0.0, 0.35); // Red color
    cylinderActor->RotateX(30.0);
    cylinderActor->RotateY(45.0);

    // Add actor to the renderer
    renderer->AddActor(cylinderActor);

    // Reset camera
    renderer->SetBackground(0.15, 0.15, 0.15); //Background Grey
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->ResetCameraClippingRange();

    // -------------------------------- SETUP MODEL PART LIST ----------------------------------

    /* Create/allocate the ModelList */
    this->partList = new ModelPartList("Parts List");

    /* Link it to the tree view in the GUI */
    ui->treeView->setModel(this->partList);

    ModelPart *rootItem = this->partList->getRootItem();

    /* Add 3 top-level items */
    for (int i = 0; i < 3; i++) {
        /* Create strings for both data columns */
        QString name = QString("TopLevel %1").arg(i);
        QString visible("true");

        /* Create child item */
        ModelPart *childItem = new ModelPart({name, visible});

        /* Append to tree top-level */
        rootItem->appendChild(childItem);

        /* Add 5 sub-items */
        for (int j = 0; j < 5; j++) {
            QString name = QString("Item %1,%2").arg(i).arg(j);
            QString visible("true");

            ModelPart *childChildItem = new ModelPart({name, visible});

            /* Append to parent */
            childItem->appendChild(childChildItem);
        }
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

// Update your button handling slot so it activates the status bar
void MainWindow::handleAddButton() {
    // This causes MainWindow to emit the signal that will then be
    // received by the status bar’s slot
    openDialog();
    emit statusUpdateMessage(QString("Add button was clicked"), 0);
}



void MainWindow::handleTreeClicked(){
    //------Exercise-5------

    /* Get the index of the selected item */
    QModelIndex index = ui->treeView->currentIndex();

    /* Get a pointer to the item from the index */
    ModelPart *selectedPart = static_cast<ModelPart*>(index.internalPointer());

    /* In this case, we will retrieve the name string from the internal QVariant data array */
    QString text = selectedPart->data(0).toString();

    emit statusUpdateMessage(QString("The selected item is: ") + text, 0);

}


void MainWindow::on_actionOpenFile_triggered() {

    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        "C:\\",
        tr("STL Files (*.stl);;Text Files (*.txt)")
    );

    //  Give a warning if no file was selected
    if (fileName.isEmpty()) { 
        QMessageBox::warning(this, 
            tr("File Selection"), 
            tr("No file was selected."), 
            QMessageBox::Ok);
        emit statusUpdateMessage(QString("No file selected"), 0);
        return;
    }   

    //  Load the selected file into the current item
    QModelIndex index = ui->treeView->currentIndex();
    if (index.isValid()) {
        ModelPart *part = static_cast<ModelPart*>(index.internalPointer());
        part->set(0, fileName); 

        qDebug() << "About to load STL for" << fileName;

        part->loadSTL(fileName);    // <<< This MUST happen!

        if (vrThread) {
            vrThread->addActorOffline(part->getActor().GetPointer());
        }
        // Adding the actor to the VR thread
        updateRender();             // <<< Then refresh
    }
    

    QString name = QFileInfo(fileName).completeBaseName();
     // Get the index of the selected item
     ModelPart *selectedPart = static_cast<ModelPart *>(index.internalPointer());

     selectedPart->set(0, QVariant(name));

    // Add this line of code so you can see if the action is working
    emit statusUpdateMessage(QString("The selected item is: ") + fileName, 0);
}

void MainWindow::on_actionStart_VR_triggered() {
    VRRenderThread* thread = new VRRenderThread(this); // Create a new VR thread

    thread->start(); // Start the VR thread
    
    this->vrThread = thread; // Store the thread pointer for later use

    ui->actionStart_VR->setEnabled(false); // Disable the Start VR action once started
    ui->actionStop_VR->setEnabled(true); // Enable the Stop VR action
    emit statusUpdateMessage(QString("Started VR Renderer"), 0);
}

void MainWindow::on_actionStop_VR_triggered() {

    if (vrThread) {
        // Tell the thread to end its loop:
        vrThread->issueCommand(VRRenderThread::END_RENDER, 0);

        // Wait for run() to finish and do its own TerminateApp/Finalize:
        vrThread->wait();

        delete vrThread;
        vrThread = nullptr;
    }
    
    ui->actionStart_VR->setEnabled(true); // Enable the Start VR action once started
    ui->actionStop_VR->setEnabled(false); // Disable the Stop VR action
    emit statusUpdateMessage(QString("Stopped VR Renderer"), 0);
}

void MainWindow::openFileDialog() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        "C:\\",
        tr("STL Files (*.stl);;Text Files (*.txt)")
        );

    if (!fileName.isEmpty()) {
        QModelIndex index = ui->treeView->currentIndex();
        if (index.isValid()) {
            ModelPart *part = static_cast<ModelPart*>(index.internalPointer());
            part->set(0, fileName);

            qDebug() << "About to load STL for" << fileName;

            part->loadSTL(fileName);    // <<< This MUST happen!
            updateRender();             // <<< Then refresh
        }
    }
}


void MainWindow::on_actionItemOptions_triggered() {
    openFileDialog();
}

void MainWindow::openDialog() {
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;

    ModelPart *part = static_cast<ModelPart*>(index.internalPointer());

    OptionDialog dialog(this);
    dialog.loadFromModelPart(part->data(0).toString(),
                             part->getColourR(), part->getColourG(), part->getColourB(), part->visible());

    if (dialog.exec() == QDialog::Accepted) {
        emit statusUpdateMessage(QString("Dialog accepted"), 0);

        part->set(0, dialog.getPartName());
        part->setColour(dialog.getRed(), dialog.getGreen(), dialog.getBlue());
        part->setVisible(dialog.getVisibility());

        //  Update actor color immediately
        if (part->getActor()) {
            part->getActor()->GetProperty()->SetColor(
                part->getColourR() / 255.0,
                part->getColourG() / 255.0,
                part->getColourB() / 255.0
                );
        }

        renderer->Render(); // Refresh the window

        ui->treeView->model()->dataChanged(index, index);
    } else {
        emit statusUpdateMessage(QString("Dialog rejected"), 0);
    }
}

void MainWindow::updateRender() {
    renderer->RemoveAllViewProps();
    updateRenderFromTree(partList->index(0, 0, QModelIndex()));
    renderer->ResetCamera();    // <<< THIS IS MISSING
    renderer->SetBackground(0.15, 0.15, 0.15); //Background Grey

    // Forcing render update 
    ui->vtkWidget->update();
    renderer->Render();

    renderWindow->Render(); // forces the render window to update
}

void MainWindow::updateAllThreadActors() {
    return;
}

void MainWindow::updateRenderFromTree(const QModelIndex& index) {
    if (!index.isValid())
        return;

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());
    if (selectedPart) {
        auto partActor = selectedPart->getActor();
        if (partActor) {
            partActor->SetVisibility(1);
            renderer->AddActor(partActor);
            qDebug() << "Added actor to renderer!"; // IS STL file being rendered
        } else {
            qDebug() << "No actor to add.";
        }
    }
    if (!partList->hasChildren(index))
        return;

    int rows = partList->rowCount(index);
    for (int i = 0; i < rows; ++i) {
        updateRenderFromTree(partList->index(i, 0, index));
    }
}


   // --------------------------------  Filters :( ----------------------------------

void MainWindow::openFilterDialog(){
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;

    ModelPart *part = static_cast<ModelPart*>(index.internalPointer());

    FilterDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {

        emit statusUpdateMessage(QString("Dialog accepted"), 0);


    } else {
        emit statusUpdateMessage(QString("Dialog rejected"), 0);
    }
}

void MainWindow::on_actionFilterOptions_triggered(){    //should only ever be opened through on action and not through normal openFilterDialog
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;

    ModelPart *part = static_cast<ModelPart*>(index.internalPointer());

    FilterDialog dialog(this);
    dialog.loadValuesFromPart(part->getClipFilterStatus(),part->getShrinkFilterStatus(),part->getClipOrigin(),part->getShrinkFactor());

    if (dialog.exec() == QDialog::Accepted) {
        //update part values
        part->setClipFilterStatus(dialog.getClipFilterEnabled());
        part->setShrinkFilterStatus(dialog.getShrinkFilterEnabled());
        part->setClipOrigin(dialog.getClipOrigin());
        part->setShrinkFactor(dialog.getShrinkFactor());

        //Need to update actor
        //part->updateActor();

        //check for no filter needed actor
        if(!(dialog.getClipFilterEnabled()) && !(dialog.getShrinkFilterEnabled())){
            part->getBaseModel();
            emit statusUpdateMessage(QString("No filtering"), 0);

        }

        if((dialog.getClipFilterEnabled()) && !(dialog.getShrinkFilterEnabled())){
            part->getClipActor();
            emit statusUpdateMessage(QString("Clip filtering"), 0);

        }

        if(!(dialog.getClipFilterEnabled()) && (dialog.getShrinkFilterEnabled())){
            part->getShrinkActor();
            emit statusUpdateMessage(QString("shrink filtering"), 0);

        }

        renderer->Render(); // Refresh the window

        /*emit statusUpdateMessage(
            QString("FilterDialog: clipFilterEnabled = %1, shrinkFilterEnabled = %2")
                .arg(dialog.getClipFilterEnabled())
                .arg(dialog.getShrinkFilterEnabled()),
            0
            );*/

    }
}
















