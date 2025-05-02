/**     @file ModelPart.h
  *
  *     EEEE2076 - Software Engineering & VR Project
  *
  *     Template for model parts that will be added as treeview items
  *
  *     P Evans 2022
  */

#ifndef VIEWER_MODELPART_H
#define VIEWER_MODELPART_H

#include <QString>
#include <QList>
#include <QVariant>

/* VTK headers - will be needed when VTK used in next worksheet,
 * commented out for now
 *
 * Note that there are a few function definitions and variables
 * commented out below - this is because you haven't yet installed
 * the VTK library which is needed.
 */
#include <vtkSmartPointer.h>
#include <vtkMapper.h>
#include <vtkActor.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkShrinkFilter.h>


class ModelPart {
public:

    unsigned char modelColourR = 255;
    unsigned char modelColourG = 1;
    unsigned char modelColourB = 1;
    bool partIsVisible = true;

    /** Constructor
     * @param data is a List (array) of strings for each property of this item (part name and visiblity in our case
     * @param parent is the parent of this item (one level up in tree)
     */
    ModelPart(const QList<QVariant>& data, ModelPart* parent = nullptr);

    /** Destructor
      * Needs to free array of child items
      */
    ~ModelPart();

    /** Add a child to this item.
      * @param item Pointer to child object (must already be allocated using new)
      */
    void appendChild(ModelPart* item);

    /** Return child at position 'row' below this item
      * @param row is the row number (below this item)
      * @return pointer to the item requested.
      */
    ModelPart* child(int row);

    /** Return number of children to this item
      * @return number of children
      */
    int childCount() const;         /* Note on the 'const' keyword - it means that this function is valid for
                                     * constant instances of this class. If a class is declared 'const' then it
                                     * cannot be modifed, this means that 'set' type functions are usually not
                                     * valid, but 'get' type functions are.
                                     */

    /** Get number of data items (2 - part name and visibility string) in this case.
      * @return number of visible data columns
      */
    int columnCount() const;

    /** Return the data item at a particular column for this item.
      * i.e. either part name of visibility
      * used by Qt when displaying tree
      * @param column is column index
      * @return the QVariant (represents string)
      */
    QVariant data(int column) const;


    /** Default function required by Qt to allow setting of part
      * properties within treeview.
      * @param column is the index of the property to set
      * @param value is the value to apply
      */
    void set( int column, const QVariant& value );

    /** Get pointer to parent item
      * @return pointer to parent item
      */
    ModelPart* parentItem();

    /** Get row index of item, relative to parent item
      * @return row index
      */
    int row() const;


    /** Set colour
      * (0-255 RGB values as ints)
      */
    void setColour(int R, const unsigned char G, const unsigned char B);

    unsigned char getColourR();
    unsigned char getColourG();
    unsigned char getColourB();


    /** Set visible flag
      * @param isVisible sets visible/non-visible
      */
    void setVisible(bool isVisible);

    /** Get visible flag
      * @return visible flag as boolean
      */
    bool visible();

    /** Load STL file
      * @param fileName
      */
    void loadSTL(QString fileName);

    /** Return actor
      * @return pointer to default actor for GUI rendering
      */
    vtkSmartPointer<vtkActor> getActor();

        /** Return actor
      * @return pointer to vrthread actor use in VR
      */
     vtkSmartPointer<vtkActor> getVrActor();

    //------------------------------Filters---------------------------------------------
    /// Added by Ben :)

    void setActor();

    //void buildActor();


    //for filers
    void setClipFilterStatus(bool inputClipFilterEnabled);

    void setShrinkFilterStatus(bool inputShrinkFilterEnabled);

    void setShrinkFactor(int inputShrinkFactor);

    void setClipOrigin(int inputClipOrigin);

    bool getClipFilterStatus();

    bool getShrinkFilterStatus();

    int getShrinkFactor();

    int getClipOrigin();



    vtkSmartPointer<vtkSTLReader> getFile() const;

    // Getter for mapper
    vtkSmartPointer<vtkPolyDataMapper> getMapper() const;

    // Setter for mapper
    void setMapper(vtkSmartPointer<vtkPolyDataMapper> mapper);

    // Getter for actor
    vtkSmartPointer<vtkActor> getActor() const;

    // Setter for actor
    void setActor(vtkSmartPointer<vtkActor> actor);

    // Getter for clipFiltedActor
    vtkSmartPointer<vtkActor> getClipFiltedActor() const;

    // Setter for clipFiltedActor
    void setClipFiltedActor(vtkSmartPointer<vtkActor> clipFiltedActor);



private:
    QList<ModelPart*>                           m_childItems;       /**< List (array) of child items */
    QList<QVariant>                             m_itemData;         /**< List (array of column data for item */
    ModelPart*                                  m_parentItem;       /**< Pointer to parent */

    /* These are some typical properties that I think the part will need, you might
     * want to add you own.
     */
    bool                                        isVisible;          /**< True/false to indicate if should be visible in model rendering */

    /* These are vtk properties that will be used to load/render a model of this part,
     * commented out for now but will be used later
     */
    vtkSmartPointer<vtkSTLReader>               file;               /**< Datafile from which part loaded */
    vtkSmartPointer<vtkPolyDataMapper>          mapper;             /**< Mapper for rendering */
    vtkSmartPointer<vtkActor>                   actor;              /**< Actor for rendering */
    vtkSmartPointer<vtkMapper>                  vrMapper;             /**< Mapper for rendering in vr*/
    vtkSmartPointer<vtkActor>                   vrActor;              /**< Actor for rendering in vr*/
    vtkSmartPointer<vtkActor>                   clipFiltedActor;
    //vtkColor3<unsigned char>                    colour;             /**< User defineable colour */

    vtkSmartPointer<vtkPolyData> polydata;


    ////////////////////////////////////////////////////////////
    /// Added by Ben :)
    //filters
    bool clipFilterEnabled = false;         //filter is disabeld by default
    bool shrinkFilterEnabled = false;
    int clipOrigin = 0;
    int shrinkFactor = 80;                  //in range 0-100 but actual range used by the filer is 0-1




    ////////////////////////////////////////////////////////////


};


#endif

