#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "optiondialog.h"
#include "filterdialog.h"


#include <QMessageBox>
#include <QFileDialog>
#include <QDirIterator>

#include <openvr.h>

#include <vtkRenderer.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkShrinkFilter.h>

#include <vtkGenericOpenGLRenderWindow.h>

#include "VRRenderThread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // -------------------------------- SETUP UI ----------------------------------

    ui->setupUi(this);

    //tree right click options
    ui->treeView->addAction(ui->actionItemOptions);
    ui->treeView->addAction(ui->actionFilterOptions);   //for filter options
    //ui->treeView->addAction(ui->actionOpen_File);
    
    ui->actionStart_VR->setEnabled(true); // Enable the Start VR action button
    ui->actionStop_VR->setEnabled(false); // Disable the Stop VR action button

    bool checkConnect; 
    // Connect signals and slots
    checkConnect = connect(ui->pushButton, &QPushButton::released, this, &MainWindow::openFileDialog); //just emits push button 1 was pressed
    Q_ASSERT(checkConnect);

    checkConnect = connect(ui->pushButton_2, &QPushButton::released, this, &MainWindow::on_actionItemOptions_triggered);
    Q_ASSERT(checkConnect);

    //Ex5 ClickedSignalOfTree Constructor
    checkConnect = connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    Q_ASSERT(checkConnect);

    // Connect a custom signal to the status bar
    checkConnect = connect(this, &MainWindow::statusUpdateMessage, ui->statusbar1, &QStatusBar::showMessage);
    Q_ASSERT(checkConnect);

    // // Connect For openning file dialog in menubar
    // checkConnect =  connect(ui->actionOpen_File, &QAction::triggered, this, &MainWindow::on_actionOpen_File_triggered);
    // Q_ASSERT(checkConnect);  

    //autoconnect to the actionOpen_File in the menu bar

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
    //vtkSmartPointer<vtkCylinderSource> cylinder = vtkSmartPointer<vtkCylinderSource>::New();
    cylinder->SetResolution(30);

    // The mapper is responsible for pushing geometry into the graphics library
    //vtkNew<vtkPolyDataMapper> cylinderMapper;
    //cylinderMapper->SetInputConnection(cylinder->GetOutputPort());



    // testing using clipping filter - this works on the cylinder
    // making the clipping plane
    vtkSmartPointer<vtkPlane> planeLeft = vtkSmartPointer<vtkPlane>::New();
    planeLeft->SetOrigin(0,0,0);
    planeLeft->SetNormal(-1,0,0);

    //applying the clipping filter to the cylinder
    vtkSmartPointer<vtkClipPolyData> clipFilter = vtkSmartPointer<vtkClipPolyData>::New();
    clipFilter->SetInputConnection(cylinder->GetOutputPort());
    clipFilter->SetClipFunction(planeLeft);

    // Update the mapper to use the clip filter output
    vtkNew<vtkPolyDataMapper> cylinderMapper;
    cylinderMapper->SetInputConnection(clipFilter->GetOutputPort());






    // The actor groups the geometry (mapper) and also has properties like color and transformation
    vtkNew<vtkActor> cylinderActor;
    cylinderActor->SetMapper(cylinderMapper);
    cylinderActor->GetProperty()->SetColor(1.0, 0.0, 0.35); // Red color
    cylinderActor->RotateX(30.0);
    cylinderActor->RotateY(45.0);

    // Add actor to the renderer
    renderer->AddActor(cylinderActor);
    cylinderActor->SetVisibility(1);        //make cylinder be visible
    renderer->RemoveActor(cylinderActor);

    // Reset camera
    renderer->SetBackground(0.15, 0.15, 0.15); //Background Grey
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->ResetCameraClippingRange();

    // create base instance for actor loading but no rendering yet
    vrThread = new VRRenderThread(this); // Create a new VR thread // Store the thread pointer for later use

    // -------------------------------- SETUP MODEL PART LIST ----------------------------------

    /* Create/allocate the ModelList */
    this->partList = new ModelPartList("Parts List");

    /* Link it to the tree view in the GUI */
    ui->treeView->setModel(this->partList);

    // /* Add 3 top-level items */
    // for (int i = 0; i < 3; i++) {
    //     /* Create strings for both data columns */
    //     QString name = QString("TopLevel %1").arg(i);
    //     QString visible("true");

    //     /* Create child item */
    //     ModelPart *childItem = new ModelPart({name, visible});

    //     /* Append to tree top-level */
    //     rootItem->appendChild(childItem);

    //     /* Add 5 sub-items */
    //     for (int j = 0; j < 5; j++) {
    //         QString name = QString("Item %1,%2").arg(i).arg(j);
    //         QString visible("true");

    //         ModelPart *childChildItem = new ModelPart({name, visible});

    //         /* Append to parent */
    //         childItem->appendChild(childChildItem);
    //     }
    // }

}

MainWindow::~MainWindow()
{
    delete ui;
}

// -------------------------------- SIGNALS AND SLOTS ----------------------------------

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

void MainWindow::on_actionStart_VR_triggered(){

    if (!steamVRAvailable())
    {
        QMessageBox::warning(
            this,
            tr("Cannot start VR"),
            tr("SteamVR is not running or no HMD detected.\n"
               "Please launch SteamVR and try again.")
        );

        return;
    }
    vrThread->start(); // Start the VR thread

    ui->actionStart_VR->setEnabled(false); // Disable the Start VR action once started
    ui->actionStop_VR->setEnabled(true); // Enable the Stop VR action
    // ui->actionOpenFile->setEnabled(false); // Disable the Open File action so loading can't happen when in VR
    emit statusUpdateMessage(QString("Started VR Renderer"), 0);
}

void MainWindow::on_actionStop_VR_triggered() {

    if (vrThread) {
        // Tell the thread to end its loop:
        vrThread->issueCommand(VRRenderThread::END_RENDER, 0);

        // Wait for run() to finish and do its own TerminateApp/Finalize:
        vrThread->wait();

        delete vrThread;
        vrThread = new VRRenderThread(this); // Create a new VR thread without rendering for future use
    }

    ui->actionStart_VR->setEnabled(true); // Enable the Start VR action once started
    ui->actionStop_VR->setEnabled(false); // Disable the Stop VR action
    // ui->actionOpenFile->setEnabled(true); // Enable the Open File action so loading can happen when not in VR

    emit statusUpdateMessage(QString("Stopped VR Renderer"), 0);
}

// -------------------------------- FILE MODEL LOADING ----------------------------------

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
    openDialog();
}

//item options
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
        part->setActor();

        /*//  Update actor color immediately
        if (part->getActor()) {
            part->getActor()->GetProperty()->SetColor(
                part->getColourR() / 255.0,
                part->getColourG() / 255.0,
                part->getColourB() / 255.0
                );
        }*/

        renderer->Render(); // Refresh the window
        updateRender();

        ui->treeView->model()->dataChanged(index, index);

        emit statusUpdateMessage(
            QString("ModelPart updated, dialog visible: %1").arg(part->visible()),
            0
            );

    } else {
        emit statusUpdateMessage(QString("Dialog rejected"), 0);
    }
}

void MainWindow::loadFolderAsTree() {
    emit statusUpdateMessage(QString("Loading models"), 0);

    QString dirPath = QFileDialog::getExistingDirectory(
    this,
    tr("Open Directory"),
    "C:\\",
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    //  Give a warning if no folder was selected
    if (dirPath.isEmpty()) { 
        QMessageBox::warning(this, 
            tr("File Selection"), 
            tr("No folder was selected."), 
            QMessageBox::Ok);
        emit statusUpdateMessage(QString("No folder selected"), 0);
        return;
    } 

    
    // Loading new STL files over the old ones if exist
    if (this->partList){
        delete this->partList; // Delete the old part list if it exists
        this->partList = nullptr; // Set to null to avoid dangling pointer
    }

    this->partList = new ModelPartList("Parts List");
    ui->treeView->setModel(this->partList);  
    ModelPart *rootItem = this->partList->getRootItem();
    // Create iterator to recursively search through directories
    QDirIterator it(dirPath, QStringList() << "*.stl", QDir::Files, QDirIterator::Subdirectories);

    int count = 0; // Counter for the number of STL files found
    while (it.hasNext()) {
        QString filePath = it.next();

        // Check if it's actually an STL file (case insensitive)
        QFileInfo fileInfo(filePath);
        if (fileInfo.suffix().toLower() != "stl") continue; // Skip if not an STL file
        
        qDebug() << "Found STL file:" << filePath;
        QString name = QFileInfo(filePath).completeBaseName();

        // Create a new part for each STL file found
        ModelPart *newPart = new ModelPart({name, "true"});
        newPart->loadSTL(filePath); 
        rootItem->appendChild(newPart); // append to tree

        // partList->getRootItem()->appendChild(newPart);
        

        // ModelPart *part = static_cast<ModelPart*>(index.internalPointer());
        // part->set(0, filePath);

        
        
        vrThread->addActorOffline(newPart->getVrActor().GetPointer());
        

        // Tell the view that the model has changed
        QModelIndex parentIndex = this->partList->index(0, 0, QModelIndex());
        emit this->partList->dataChanged(parentIndex, parentIndex);
        count++;
    }
    if (count == 0) {
        emit statusUpdateMessage(QString("No STL files found in the selected directory"), 0);
    } else {
        emit statusUpdateMessage(QString("%1 STL files loaded").arg(count), 0);
        updateRender();
    }
}

void MainWindow::on_actionOpen_File_triggered(){
    loadFolderAsTree(); // Load the folder as a tree structure
}

// -------------------------------- UPDATE RENDERING ----------------------------------

void MainWindow::updateRender() {
    renderer->RemoveAllViewProps();
    updateRenderFromTree(QModelIndex());
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

void MainWindow::updateRenderFromTree(const QModelIndex& parentIndex) {
    // Process the parent item first if it's valid
    if (parentIndex.isValid()) {
        ModelPart* part = static_cast<ModelPart*>(parentIndex.internalPointer());
        if (part && part->getActor()) {
            renderer->AddActor(part->getActor());
            qDebug() << "Added actor for part:" << part->data(0).toString();
        }
    }

    // Get the number of children under this parent
    int rowCount = partList->rowCount(parentIndex);
    qDebug() << "Processing" << rowCount << "children at this level";

    // Process all children
    for (int i = 0; i < rowCount; ++i) {
        QModelIndex childIndex = partList->index(i, 0, parentIndex);
        updateRenderFromTree(childIndex);
    }
}

// -------------------------------- VR CHECKERS----------------------------------

// Returns true iff SteamVR is installed, running, and an HMD is connected
bool MainWindow::steamVRAvailable() {
    // Quick check: is the runtime installed on this machine?
    if (!vr::VR_IsRuntimeInstalled())
        return false;

    // Is an HMD actually plugged in?
    if (!vr::VR_IsHmdPresent())
        return false;

    // Try to initialize & immediately shut down the VR system just to test
    vr::EVRInitError initError = vr::VRInitError_None;
    vr::IVRSystem* pVRSys = vr::VR_Init(&initError, vr::VRApplication_Scene);
    if (initError != vr::VRInitError_None)
    {
        // describe the failure in English
        vr::VR_Shutdown();
        return false;
    }
    vr::VR_Shutdown();
    return true;
}


// --------------------------------  Filters ----------------------------------

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

        if((dialog.getClipFilterEnabled()) && !(dialog.getShrinkFilterEnabled())){

            // testing using clipping filter - this works on the cylinder
            // creating the clipping plane
            vtkSmartPointer<vtkPlane> planeLeft = vtkSmartPointer<vtkPlane>::New();
            planeLeft->SetOrigin(part->getClipOrigin(),0,0);
            planeLeft->SetNormal(-1,0,0);

            //applying the clipping filter to the cylinder
            vtkSmartPointer<vtkClipPolyData> clipFilterM = vtkSmartPointer<vtkClipPolyData>::New();
            clipFilterM->SetInputConnection(part->getFile()->GetOutputPort());
            clipFilterM->SetClipFunction(planeLeft);

            // Update the mapper to use the clip filter output
            vtkNew<vtkPolyDataMapper> clipMapper;
            clipMapper->SetInputConnection(clipFilterM->GetOutputPort());


            // The actor groups the geometry (mapper) and also has properties like color and transformation
            vtkNew<vtkActor> clipActor;
            clipActor->SetMapper(clipMapper);
            clipActor->GetProperty()->SetColor(part->getColourR(), part->getColourG(), part->getColourG());

            // Add actor to the renderer
            renderer->AddActor(clipActor);
            clipActor->SetVisibility(part->visible());        //make cylinder be visible


            //updating the render
            renderer->Render(); // Refresh the window
            updateRender();
            renderer->RemoveActor(part->getActor());

            emit statusUpdateMessage(QString("Clip filtering"), 0);

        }

        if(!(dialog.getClipFilterEnabled()) && (dialog.getShrinkFilterEnabled())){

            emit statusUpdateMessage(QString("shrink filtering"), 0);

        }

        //check for no filter needed actor
        if(!(dialog.getClipFilterEnabled()) && !(dialog.getShrinkFilterEnabled())){


            renderer->AddActor(part->getActor());
            //renderer->RemoveActor(clipActor);

            renderer->Render(); // Refresh the window
            updateRender();

            emit statusUpdateMessage(QString("No filtering"), 0);

        }


        /*emit statusUpdateMessage(
            QString("FilterDialog: clipFilterEnabled = %1, shrinkFilterEnabled = %2")
                .arg(dialog.getClipFilterEnabled())
                .arg(dialog.getShrinkFilterEnabled()),
            0
            );*/

    }
}











