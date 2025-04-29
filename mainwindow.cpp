#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include "optiondialog.h"

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkProperty.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeView->addAction(ui->actionItemOptions);

    // Connect signals and slots
    connect(ui->pushButton, &QPushButton::released, this, &MainWindow::handleButton_1);
    connect(ui->pushButton_2, &QPushButton::released, this, &MainWindow::handleButton_2);

    //Ex5 ClickedSignalOfTree Constructor
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);

    // Connect a custom signal to the status bar
    connect(this, &MainWindow::statusUpdateMessage, ui->statusBar, &QStatusBar::showMessage);

    // Connect For exercise 8
    connect(ui->actionOpen_File, &QAction::triggered, this, &MainWindow::openFileDialog);

    // Connect for exercise 9
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::openDialog);



    // Exercise 3, Worksheet 7 Task
    // Link a render window with the Qt widget
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
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
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->ResetCameraClippingRange();

    //-----------------------------------------------

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

void MainWindow::handleButton_1(){
    emit statusUpdateMessage(QString("Add button was clicked"), 0);
}

void MainWindow::handleButton_2(){

    //-----------QMsg Box-------------------
    //QMessageBox msgBox;
    //msgBox.setText("Button 2 was clicked!");
    //msgBox.exec();

    //------------Status Bar----------------
    emit statusUpdateMessage(QString("Add button 2 was clicked"), 0);
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


void MainWindow::on_actionOpen_File_triggered()
{
    emit statusUpdateMessage(QString("Open File action triggered"), 0);
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
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;

    ModelPart *part = static_cast<ModelPart*>(index.internalPointer());

    OptionDialog dialog(this);
    dialog.loadFromModelPart(part->data(0).toString(), part->getColourR(), part->getColourG(), part->getColourB(), part->visible());

    if (dialog.exec() == QDialog::Accepted) {
        part->set(0, dialog.getPartName());
        part->setColour(dialog.getRed(), dialog.getGreen(), dialog.getBlue());
        part->setVisible(dialog.getVisibility());
        emit statusUpdateMessage(QString("ModelPart updated"), 0);
    }
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

        // 🌟 Update actor color immediately
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
    renderer->Render();

    renderer->ResetCamera();    // <<< THIS IS MISSING
    renderer->Render();
    renderer->SetBackground(0.2, 0.2, 0.2); //Background Grey
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
            qDebug() << "Added actor to renderer!"; //IS STL file being rendered
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
