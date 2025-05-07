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
#include <QLabel>
#include <QWidgetAction>

#include <vtkGenericOpenGLRenderWindow.h>

#include "VRRenderThread.h"

#include <vtkShrinkFilter.h>
#include <vtkType.h>  // Include for vtkIdType - for filters debug
#include <vtkPolyData.h>    // for filters debug
#include <vtkShrinkPolyData.h>

#include <vtkLight.h> //Lighting

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // -------------------------------- SETUP UI ----------------------------------

    ui->setupUi(this);

    // RIGHT CLICK OPTIONS
    ui->treeView->addAction(ui->actionItemOptions);
    ui->treeView->addAction(ui->actionFilterOptions);   
    ui->treeView->addAction(ui->actionReplace_Part);
    
    ui->actionStart_VR->setEnabled(true); // Enable the Start VR action button
    ui->actionStop_VR->setEnabled(false); // Disable the Stop VR action button


    bool checkConnect;
    // Connect signals and slots


    //Ex5 ClickedSignalOfTree Constructor
    checkConnect = connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    Q_ASSERT(checkConnect);

    // Connect a custom signal to the status bar
    checkConnect = connect(this, &MainWindow::statusUpdateMessage, ui->statusbar1, &QStatusBar::showMessage);
    Q_ASSERT(checkConnect);

    checkConnect = connect(ui->addPushButton, &QPushButton::released, this, &MainWindow::addNewPart);
    Q_ASSERT(checkConnect);

    // -------------------------------- UI PANELS ----------------------------------

    mainLight = vtkSmartPointer<vtkLight>::New();

    //Creates blank panel for lighting sliders
    QWidget *lightingPanel = new QWidget(this);

    //Vertical Layout with 8 pixel spacing on each side
    QVBoxLayout *layout = new QVBoxLayout(lightingPanel);
    layout->setContentsMargins(8, 8, 8, 8);

    // Intensity Slider
    QSlider *intensitySlider = new QSlider(Qt::Horizontal);
    intensitySlider->setRange(0, 100);
    intensitySlider->setValue(50);
    layout->addWidget(new QLabel("Intensity"));
    layout->addWidget(intensitySlider);

    // Azimuth Slider X - Plane
    QSlider *azimuthSlider = new QSlider(Qt::Horizontal);
    azimuthSlider->setRange(0, 360);
    azimuthSlider->setValue(30);
    layout->addWidget(new QLabel("Azimuth"));
    layout->addWidget(azimuthSlider);

    // Pitch Slider Y - Plane
    QSlider *pitchSlider = new QSlider(Qt::Horizontal);
    pitchSlider->setRange(-90, 90);
    pitchSlider->setValue(30);
    layout->addWidget(new QLabel("Pitch"));
    layout->addWidget(pitchSlider);

    QWidgetAction *lightingAction = new QWidgetAction(this);
    lightingAction->setDefaultWidget(lightingPanel);

    QMenu *lightingMenu = new QMenu(this);
    lightingMenu->addAction(lightingAction);

    connect(ui->actionLighting, &QAction::triggered, this, [=]() {
        QPoint pos = mapToGlobal(ui->toolBar->geometry().bottomLeft());
        lightingMenu->exec(QCursor::pos());
    });

    connect(intensitySlider, &QSlider::valueChanged, this, [=](int val) {
        mainLight->SetIntensity(val / 100.0 * 5.0);
        renderWindow->Render();
    });

    connect(azimuthSlider, &QSlider::valueChanged, this, [=](int val) {
        double rad = qDegreesToRadians(static_cast<double>(val));
        mainLight->SetPosition(50 * cos(rad), 50 * sin(rad), 0);
        mainLight->SetFocalPoint(0, 0, 0);
        renderWindow->Render();
    });

    connect(pitchSlider, &QSlider::valueChanged, this, [=](int val) {
        double azimuth = azimuthSlider->value();
        double rAz = qDegreesToRadians(static_cast<double>(azimuth));
        double rEl = qDegreesToRadians(static_cast<double>(val));
        mainLight->SetPosition(50 * cos(rEl) * cos(rAz), 50 * cos(rEl) * sin(rAz), 50 * sin(rEl));
        mainLight->SetFocalPoint(0, 0, 0);
        renderWindow->Render();
    });
    //END SLIDERS FOR LIGHTING



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


    // -------------------------------- Basic cylinder ----------------------------------
    // this is for rendering basic cylinder
    // The mapper is responsible for pushing geometry into the graphics library
    vtkNew<vtkPolyDataMapper> cylinderMapper;
    cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

    // ---------------------------------------------------------------------------------------

    // The actor groups the geometry (mapper) and also has properties like color and transformation
    vtkNew<vtkActor> cylinderActor;
    cylinderActor->SetMapper(cylinderMapper);

    // cylinder colour and rotation
    cylinderActor->GetProperty()->SetColor(1.0, 0.0, 0.35); // Red color
    cylinderActor->RotateX(30.0);
    cylinderActor->RotateY(45.0);

    // ---------------------------------------------------------------------------------------


    // === Section 2.2 Lighting ===
    //light->SetLightTypeToSceneLight();
    //light->SetPosition(50, 0, 0);      // light far to the right TESTING
    //light->SetFocalPoint(0, 0, 0);   // Auto follows camera – safe & easy
    //light->SetIntensity(5.0);           // Full brightness
    renderer->AddLight(mainLight);
    // =============================

    // Add actor to the renderer
    renderer->AddActor(cylinderActor);
    renderer->RemoveActor(cylinderActor);

    // Reset camera
    renderer->SetBackground(0.15, 0.15, 0.15); //Background Grey
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Roll(30);
    renderer->GetActiveCamera()->Elevation(-70);
    renderer->ResetCameraClippingRange();
    

    // create base instance for actor loading but no rendering yet
    vrThread = new VRRenderThread(this); // Create a new VR thread // Store the thread pointer for later use

    // -------------------------------- SETUP MODEL PART LIST ----------------------------------

    /* Create/allocate the ModelList */
    this->partList = new ModelPartList("Parts List");

    /* Link it to the tree view in the GUI */
    ui->treeView->setModel(this->partList);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// -------------------------------- SIGNALS AND SLOTS ----------------------------------

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
    ui->actionOpen_Dir->setEnabled(false); // Disable the Open File action so loading can't happen when in VR
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
    ui->actionOpen_Dir->setEnabled(true); // Enable the Open File action so loading can happen when not in VR

    emit statusUpdateMessage(QString("Stopped VR Renderer"), 0);
}

// -------------------------------- FILE MODEL LOADING ----------------------------------


void MainWindow::on_actionItemOptions_triggered() {
    return;
}

void MainWindow::on_actionReplace_Part_triggered() {
    replaceSelectedPart();
}


// -------------------------------- DIALOGS ----------------------------------
void MainWindow::replaceSelectedPart() {
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(
            this,
            tr("No item selected"),
            tr("Please select an item in the tree view to replace.")
        );
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        "C:\\",
        tr("STL Files (*.stl);;Text Files (*.txt)")
    );

    //  Give a warning if no file was selected
    if (filePath.isEmpty()) { 
        QMessageBox::warning(this, 
            tr("File Selection"), 
            tr("No file was selected."), 
            QMessageBox::Ok);
        emit statusUpdateMessage(QString("No file selected"), 0);
        return;
    }   

    ModelPart *part = partList->getPart(index); // Get the part from the model

    QString name = QFileInfo(filePath).completeBaseName();
    part->set(0, name); 

    qDebug() << "About to load STL for" << filePath;

    part->loadSTL(filePath);
    ui->treeView->model()->dataChanged(index, index); // Tell the view that the model has changed
    updateRender();
}

void MainWindow::addNewPart() {
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        "C:\\",
        tr("STL Files (*.stl);;Text  (*.txt)")
    );

    //  Give a warning if no file was selected
    if (filePath.isEmpty()) { 
        QMessageBox::warning(this, 
            tr("File Selection"), 
            tr("No file was selected."), 
            QMessageBox::Ok);
        emit statusUpdateMessage(QString("No file selected"), 0);
        return;
    }

    QString name = QFileInfo(filePath).completeBaseName();
    ModelPart *newPart = new ModelPart({name, "true"});

    qDebug() << "About to load STL for" << filePath;

    QModelIndex index = ui->treeView->currentIndex();
    if (index.isValid()) {
       partList->getPart(index)->appendChild(newPart); // Append the new part to the selected part
       ui->treeView->model()->dataChanged(index, index); // Tell the view that the model has changed
    } else {
        ModelPart* rootItem = partList->getRootItem();
        rootItem->appendChild(newPart);
        
        // Get index of the new item
        QModelIndex newPartIndex = partList->index(rootItem->childCount() - 1, 0, QModelIndex());
        emit partList->dataChanged(QModelIndex(), newPartIndex);
    }
    newPart->loadSTL(filePath);
    updateRender();
}

void MainWindow::loadFolderAsTree() {
    emit statusUpdateMessage(QString("Loading models, this may take a while for large groups."), 0);

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
        renderer->RemoveAllViewProps();
        qDebug() << "Deleted old part list";
    }
    // Create a new part list and set it to the tree view
    this->partList = new ModelPartList("Parts List");
    ui->treeView->setModel(this->partList);  
    ModelPart *rootItem = this->partList->getRootItem();

    // Create iterator to recursively search through directories
    QDirIterator item(dirPath, QStringList() << "*.stl", QDir::Files, QDirIterator::Subdirectories);

    int count = 0; // Counter for the number of STL files found
    while (item.hasNext()) {
        QString filePath = item.next();

        // Check if it's actually an STL file (case insensitive)
        QFileInfo fileInfo(filePath);
        if (fileInfo.suffix().toLower() != "stl") continue; // Skip if not an STL file
        
        qDebug() << "Found STL file:" << filePath;
        QString name = QFileInfo(filePath).completeBaseName();

        // Create a new part for each STL file found
        ModelPart *newPart = new ModelPart({name, "true"});
        newPart->loadSTL(filePath); 
        rootItem->appendChild(newPart); // append to tree

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

void MainWindow::on_actionOpen_Dir_triggered(){
    loadFolderAsTree(); // Load the folder as a tree structure
}

// -------------------------------- UPDATE RENDERING ----------------------------------

void MainWindow::updateRender() {
    updateRenderFromTree(QModelIndex());
    renderer->ResetCamera();  
    // renderer->RotateCamera(90, 0, 0); // Rotate the camera instead of the model to get accurate lighting

    renderer->SetBackground(0.15, 0.15, 0.15); //Background Grey

    // Forcing render update
    ui->vtkWidget->update();
    renderer->Render();

    renderWindow->Render(); // forces the render window to update when any 
    // change is made rather than having to interact to render
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

        // -------------- update values from filters dialog -------------------------
        part->setClipFilterStatus(dialog.getClipFilterEnabled());
        part->setShrinkFilterStatus(dialog.getShrinkFilterEnabled());
        part->setClipOrigin(dialog.getClipOrigin());
        part->setShrinkFactor(dialog.getShrinkFactor());

        vtkAlgorithmOutput* currentOutput = part->getFile()->GetOutputPort();
        vtkSmartPointer<vtkAlgorithm> lastFilter;

        // -------------------------- render setup ----------------------------------
        // Remove old filtered actors
        if (part->getFiltedActor()) {
            renderer->RemoveActor(part->getFiltedActor());
        }

        renderer->RemoveActor(part->getActor());    //remove original part
        part->getActor()->SetVisibility(0);         //temp method should use remove actor

        bool clipEnabled = dialog.getClipFilterEnabled();   //transfereed values jsut to make easier to read
        bool shrinkEnabled = dialog.getShrinkFilterEnabled();

        // -------------------------- clip filter ----------------------------------
        if (clipEnabled) {
            // creating the clipping plane
            auto planeLeft = vtkSmartPointer<vtkPlane>::New();
            planeLeft->SetOrigin(part->getClipOrigin(), 0, 0);
            planeLeft->SetNormal(-1, 0, 0);

            //applying the clipping filter to the part
            auto clipFilter = vtkSmartPointer<vtkClipPolyData>::New();
            clipFilter->SetInputConnection(currentOutput);
            clipFilter->SetClipFunction(planeLeft);
            currentOutput = clipFilter->GetOutputPort();
            lastFilter = clipFilter;
        }

        // -------------------------- shrink filter ----------------------------------
        if (shrinkEnabled) {
            // setup the shrink filter
            auto shrinkFilter = vtkSmartPointer<vtkShrinkPolyData>::New();
            shrinkFilter->SetInputConnection(currentOutput);
            shrinkFilter->SetShrinkFactor(part->getShrinkFactorAsFloat());
            currentOutput = shrinkFilter->GetOutputPort();
            lastFilter = shrinkFilter;
        }

        // -------------------------- making actor ----------------------------------
        if (clipEnabled || shrinkEnabled) {
            auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(currentOutput);
            auto actor = vtkSmartPointer<vtkActor>::New();

            actor->SetMapper(mapper);

            //copy position data  to new actor
            actor->SetPosition(part->getActor()->GetPosition());
            actor->SetOrientation(part->getActor()->GetOrientation());
            actor->SetScale(part->getActor()->GetScale());
            actor->SetUserTransform(part->getActor()->GetUserTransform());

            if (clipEnabled && shrinkEnabled) { //both filters
                part->setFiltedActor(actor);
                renderer->AddActor(part->getFiltedActor());
                //emit statusUpdateMessage(QString("Both filtering"), 0);

            } else if (clipEnabled) {           //just clip filter
                part->setFiltedActor(actor);
                renderer->AddActor(part->getFiltedActor());
                //emit statusUpdateMessage(QString("Clip filtering"), 0);

            } else if (shrinkEnabled) {         // just shrink filter
                part->setFiltedActor(actor);
                renderer->AddActor(part->getFiltedActor());
                //emit statusUpdateMessage(QString("Shrink filtering"), 0);
            }


        } else {
            // No filter: show original actor
            renderer->AddActor(part->getActor());
            part->getActor()->SetVisibility(1);     //temp method should use add actor

            //emit statusUpdateMessage(QString("No filtering"), 0);
        }

        renderer->Render();
        updateRender();

        emit statusUpdateMessage(
            QString("FilterDialog: clipFilterEnabled = %1, shrinkFilterEnabled = %2")
                .arg(dialog.getClipFilterEnabled())
                .arg(dialog.getShrinkFilterEnabled()),
            0
            );
    }

}

