
/**     @file ModelPart.cpp
  *
  *     EEEE2076 - Software Engineering & VR Project
  *
  *     Template for model parts that will be added as treeview items
  *
  *     P Evans 2022
  */

#include "ModelPart.h"
#include <QDebug>



/* Commented out for now, will be uncommented later when you have
 * installed the VTK library
 */
#include <vtkSmartPointer.h>
#include <vtkDataSetMapper.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

#include <vtkPlane.h>
#include <vtkClipDataSet.h>
#include <vtkDataSetAlgorithm.h> // For GetOutputPort()
#include <vtkXMLUnstructuredGridReader.h> // Example reader


ModelPart::ModelPart(const QList<QVariant>& data, ModelPart* parent )
    : m_itemData(data), m_parentItem(parent) {

    /* You probably want to give the item a default colour */
}


ModelPart::~ModelPart() {
    qDeleteAll(m_childItems);
}


void ModelPart::appendChild( ModelPart* item ) {
    /* Add another model part as a child of this part
     * (it will appear as a sub-branch in the treeview)
     */
    item->m_parentItem = this;
    m_childItems.append(item);
}


ModelPart* ModelPart::child( int row ) {
    /* Return pointer to child item in row below this item.
     */
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int ModelPart::childCount() const {
    /* Count number of child items
     */
    return m_childItems.count();
}


int ModelPart::columnCount() const {
    /* Count number of columns (properties) that this item has.
     */
    return m_itemData.count();
}

QVariant ModelPart::data(int column) const {
    /* Return the data associated with a column of this item
     *  Note on the QVariant type - it is a generic placeholder type
     *  that can take on the type of most Qt classes. It allows each
     *  column or property to store data of an arbitrary type.
     */
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}


void ModelPart::set(int column, const QVariant &value) {
    /* Set the data associated with a column of this item
     */
    if (column < 0 || column >= m_itemData.size())
        return;

    m_itemData.replace(column, value);
}


ModelPart* ModelPart::parentItem() {
    return m_parentItem;
}


int ModelPart::row() const {
    /* Return the row index of this item, relative to it's parent.
     */
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModelPart*>(this));
    return 0;
}

void ModelPart::setColour(int R, const unsigned char  G, const unsigned char  B) {
    /* This is a placeholder function */

    modelColourR = static_cast<unsigned char>(R);
    modelColourG = G;
    modelColourB = B;
    qDebug() << "R:" << R << "G:" << G << "B:" << B;
    qDebug() << "modelColourR:" << modelColourR << "modelColourG:" << modelColourG << "modelColourB:" << modelColourB;

}

unsigned char ModelPart::getColourR() {
    /* This is a placeholder function that you will need to modify if you want to use it */
    qDebug() << "getmodelColourR:" << modelColourR;
    /* As the name suggests ... */
    return modelColourR;   // needs updating
}

unsigned char ModelPart::getColourG() {
    /* This is a placeholder function that you will need to modify if you want to use it */
    qDebug() << "getmodelColourG:" << modelColourG;
    /* As the name suggests ... */
    return modelColourG;   // needs updating
}


unsigned char ModelPart::getColourB() {
    /* This is a placeholder function that you will need to modify if you want to use it */
    qDebug() << "getmodelColourB:" << modelColourB;
    /* As the name suggests ... */
    return modelColourB;   // needs updating
}


void ModelPart::setVisible(bool isVisible) {
    /* This is a placeholder function that you will need to modify if you want to use it */
    partIsVisible = isVisible;
    /* As the name suggests ... */
}

bool ModelPart::visible() {
    /* This is a placeholder function that you will need to modify if you want to use it */
    qDebug() << "partIsVisible:" << partIsVisible;
    /* As the name suggests ... */
    return partIsVisible;
}

void ModelPart::loadSTL(QString fileName) {
    qDebug() << "Loading STL file:" << fileName;

    file = vtkSmartPointer<vtkSTLReader>::New();
    file->SetFileName(fileName.toStdString().c_str());
    file->Update();
    double bounds[6];
    file->GetOutput()->GetBounds(bounds);
    qDebug() << "Model bounds:"
             << bounds[0] << bounds[1]
             << bounds[2] << bounds[3]
             << bounds[4] << bounds[5];

    if (file->GetOutput()->GetNumberOfPoints() == 0) {
        qDebug() << "STL file contains 0 points! Failed to load model.";
        return;
    } else {
        qDebug() << "STL file loaded successfully with" << file->GetOutput()->GetNumberOfPoints() << "points.";
    }

    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(file->GetOutputPort());

    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // CAD Colour
    //actor->GetProperty()->SetColor(
    //    modelColourR / 255.0,
    //    modelColourG / 255.0,
    //    modelColourB / 255.0
    //    );
    //actor->SetVisibility(partIsVisible ? 1 : 0); //set to visible

    actor->GetProperty()->SetColor(modelColourR, modelColourG, modelColourB);  // White model for testing

    actor->SetVisibility(1); // Make sure it's visible


}

vtkSmartPointer<vtkActor> ModelPart::getActor() {
/* This is a placeholder function that you will need to modify if you want to use it */
    return actor;
/* Needs to return a smart pointer to the vtkActor to allow
     * part to be rendered.
     */
}

// ------------------------------------------------------------------
//added by Ben :)
void ModelPart::setActor(){

    actor->GetProperty()->SetColor(modelColourR,modelColourG,modelColourB);
    actor->SetVisibility(partIsVisible);

}

// -------------------------------- Filters ----------------------------------
//for filters UI setters/getters
void ModelPart::setClipFilterStatus(bool inputClipFilterEnabled){
    clipFilterEnabled = inputClipFilterEnabled;
}

void ModelPart::setShrinkFilterStatus(bool inputShrinkFilterEnabled){
    shrinkFilterEnabled = inputShrinkFilterEnabled;
}

void ModelPart::setClipOrigin(int inputClipOrigin){
    clipOrigin = inputClipOrigin;
}

void ModelPart::setShrinkFactor(int inputShrinkFactor){
    shrinkFactor = inputShrinkFactor;
}

bool ModelPart::getShrinkFilterStatus(){
    return shrinkFilterEnabled;
}

bool ModelPart::getClipFilterStatus(){
    return clipFilterEnabled;
}

int ModelPart::getShrinkFactor(){
    return shrinkFactor;
}

int ModelPart::getClipOrigin(){
    return clipOrigin;
}


// --------------------------------- WIP ----------------------------------


void ModelPart::getClipActor(){

    if (clipFilterEnabled && !shrinkFilterEnabled){
        /*
        //vtk filter
        //this will apply a clipping plane whose normal is the x= axis that crosses the x=axis
        vtkSmartPointer<vtkPlane> clipPlane = vtkSmartPointer<vtkPlane>::New();
        clipPlane->SetOrigin(getClipOrigin(), 0.0, 0.0); //adjust x to change how much of the model is clipped
        clipPlane->SetNormal(1, 0.0, 0.0);

        vtkSmartPointer<vtkClipDataSet> clipFilter = vtkSmartPointer<vtkClipDataSet >::New();
        clipFilter ->SetInputConnection(file->GetOutputPort() );
        clipFilter ->SetClipFunction(clipPlane);


        //mapper -> filter
        vtkSmartPointer<vtkPolyDataMapper> clipMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        clipMapper->SetInputConnection(clipFilter->GetOutputPort());

        //filter -> actor
        //actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(clipMapper);*/
    }



}

void ModelPart::getShrinkActor(){
/*
    vtkSmartPointer<vtkShrinkFilter> shrinkFilter = vtkSmartPointer<vtkShrinkFilter >::New();

    shrinkFilter->SetInputConnection(file->GetOutputPort());
    shrinkFilter->SetShrinkFactor(shrinkFactor);
    shrinkFilter->Update();

    vtkSmartPointer<vtkPolyDataMapper> shrinkMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    shrinkMapper->SetInputConnection( shrinkFilter->GetOutputPort());

    //filter -> actor
    //shrinkActor = vtkSmartPointer<vtkActor>::New();
    // Set the new mapper to the existing actor

    actor->SetMapper(shrinkMapper);*/

}

void ModelPart::getBaseModel(){

    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(file->GetOutputPort());

    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    actor->GetProperty()->SetColor(modelColourR,modelColourG,modelColourB);
    actor->SetVisibility(partIsVisible);


}


// ---------------------------------------------------------------------


//vtkActor* ModelPart::getNewActor() {
/* This is a placeholder function that you will need to modify if you want to use it
     *
     * The default mapper/actor combination can only be used to render the part in
     * the GUI, it CANNOT also be used to render the part in VR. This means you need
     * to create a second mapper/actor combination for use in VR - that is the role
     * of this function. */


/* 1. Create new mapper */

/* 2. Create new actor and link to mapper */

/* 3. Link the vtkProperties of the original actor to the new actor. This means
      *    if you change properties of the original part (colour, position, etc), the
      *    changes will be reflected in the GUI AND VR rendering.
      *
      *    See the vtkActor documentation, particularly the GetProperty() and SetProperty()
      *    functions.
      */


/* The new vtkActor pointer must be returned here */
//    return nullptr;

//}


