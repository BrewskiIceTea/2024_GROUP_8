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

// vtk headers
#include <vtkSmartPointer.h>
#include <vtkMapper.h>
#include <vtkActor.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkShrinkFilter.h>

/**
 * @brief Contains all model part related information
 * @note Contains RGB colour component values, visibility status, filter status and filter values
 */
class ModelPart {
public:

    // model defualt colour values
    unsigned char modelColourR = 255;        /**< Red (R) value component of the model */
    unsigned char modelColourG = 1;          /**< Green (G) value component of the model */
    unsigned char modelColourB = 1;          /**< Blue (B) value component of the model */
    bool partIsVisible = true;               /**< Visibility of the model */

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


    /**
     * @brief Sets the RGB color components of the model part
     * @param Red (R) value (0–255)
     * @param Green (G) value (0–255)
     * @param Blue (B) value (0–255)
     */
    void setColour(int R, const unsigned char G, const unsigned char B);

    /**
     * @brief get the int Red (R) colour component of the model
     * @return The red colour component value (0-255)
     */
    unsigned char getColourR();

    /**
     * @brief get the int Green (G) colour component of the model
     * @return The green colour component value (0-255)
     */
    unsigned char getColourG();

    /**
     * @brief get the int Blue (B) colour component of the model
     * @return The blue colour component value (0-255)
     */
    unsigned char getColourB();


    /** Set visible flag
     *  @brief controls if the model part is visible or not in the render
      * @param isVisible sets visible/non-visible
      */
    void setVisible(bool isVisible);

    /** Get visible flag
     *  @brief Gets the model visibility value (getVisible)
      * @return visible flag as boolean
      */
    bool visible();

    /** Load STL file
     *  @brief loads the part and prepares it to be rendered
      * @param fileName
      * @note this is used in file Open to load parts into the program and sets them up so they can be used later
      * @return part name, mapper, actor, vrMapper and vrActor
      */
    void loadSTL(QString fileName);

    /** Return actor
     *  @brief gets the actor from the model to be used in rendering
      * @return pointer to default actor for GUI rendering
      */
    vtkSmartPointer<vtkActor> getActor();

    /** Return actor
      * @brief gets the VR actor from the model
      * @return pointer to vrthread actor use in VR
      */
     vtkSmartPointer<vtkActor> getVrActor();



    //------------------------------Filters---------------------------------------------
    /// Added by Ben :)

    //------------------------------Setters---------------------------------------------
     /**
     * @brief this is used to update the models actor with releveant colour and visibility changes
     * @note used in item options to update the RGB values and model visibility status
     */
    void setActorValues();  //think this is duplicated and not needed

     /**
     * @brief updates the moodel if it's being clip filtered
     * @param inputClipFilterEnabled
     */
    void setClipFilterStatus(bool inputClipFilterEnabled);

    /**
     * @brief updates the model if it's being shrink filtered
     * @param inputShrinkFilterEnabled
     */
    void setShrinkFilterStatus(bool inputShrinkFilterEnabled);

    /**
     * @brief Sets the shrink factor for the shrink filter and determines how much the model planes are scaled by
     * @note the input is an int but SetShrinkFactor() uses float inputs
     * @param inputShrinkFactor
     */
    void setShrinkFactor(int inputShrinkFactor);

    /**
     * @brief Sets the position of the clip filter which determines how much of the model is clipped
     * @note this changes the x position of the filter so the fitler can only move along the x-axis
     * @param inputClipOrigin
     */
    void setClipOrigin(int inputClipOrigin);

    /**
     * @brief this is used to change the model's actor
     * @param new actor value to replace the original
     */
    void setActor(vtkSmartPointer<vtkActor> actor);

    /**
     * @brief updates the mapper of the model to a new value
     * @param new mapper value
     */
    void setMapper(vtkSmartPointer<vtkPolyDataMapper> mapper);


    //------------------------------Getters---------------------------------------------
    /**
     * @brief gets the clip filter status of the model
     * @return returns the boolean value of the clip filter status (0/false = fitlered disabled 1/true = filter enabled)
     */
    bool getClipFilterStatus();

    /**
     * @brief gets the shrink filter status of the model
     * @return returns the boolean value of the shrink filter status (0/false = fitlered disabled 1/true = filter enabled)
     */
    bool getShrinkFilterStatus();

    /**
     * @brief gets the INT shrink factor of the model, to be used in filter dialog
     * @note to use the shrink facotr it needs to be a float but to update the dialog it needs to be an int so two functions are used
     * @return returns the shrink factor as a float in the range (0->100)
     */
    int getShrinkFactor();

    /**
     * @brief gets the FLOAT shrink factor to be used in SetShrinkFactor()
     * @note conversion to float is needed as shrinkFactor is stored as an int, its also divided by 100 to fit in the range (0->1)
     * @return returns the shrink factor as a float in the range (0->1)
     */
    float getShrinkFactorAsFloat();

    /**
     * @brief Sets the position of the clip filter which determines how much of the model is clipped
     * @note this changes the x position of the filter so the fitler can only move along the x-axis
     * @return the int clip origin of the filter
     */
    int getClipOrigin();

    /**
     * @brief Gets the STL file reader used to load the model part
     * @return Smart pointer to the vtkSTLReader
     */
    vtkSmartPointer<vtkSTLReader> getFile() const;

    /**
     * @brief Gets the data mapper for rendering the model part
     * @return Smart pointer to the vtkPolyDataMapper
     */
    vtkSmartPointer<vtkPolyDataMapper> getMapper() const;

    /**
     * @brief Gets the base unfiltered VTK actor used for GUI rendering
     * @return Smart pointer to the vtkActor
     */
    vtkSmartPointer<vtkActor> getActor() const;


    //------------------------------ Filters 2 --------------------------------------

    /**
     * @brief Used to change the VTK filtered actor of the model
     * @note this is used to change the filtered actor used by the model
     * @param filtedActor
     */
    void setFiltedActor(vtkSmartPointer<vtkActor> filtedActor);

    /**
     * @brief Gets the filtered VTK actor after applying the clipping filter
     * @note this is different to getActor as this actor has had 1 or 2 filters act upon it
     * @return Smart pointer to the clip-filtered vtkActor
     */
    vtkSmartPointer<vtkActor> getFiltedActor() const;

    //---------------------------------------------------------------------------------



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
    vtkSmartPointer<vtkMapper>                  vrMapper;             /**< Mapper for rendering in vr*/

    vtkSmartPointer<vtkActor>                   actor;              /**< Actor for rendering */
    vtkSmartPointer<vtkActor>                   vrActor;              /**< Actor for rendering in vr*/
    vtkSmartPointer<vtkActor>                   filtedActor;            /**< Filtered Actor for rendering*/

    //vtkColor3<unsigned char>                    colour;             /**< User defineable colour */


    //------------------------------Filters defaults---------------------------------------------
    // Added by Ben :)
    bool clipFilterEnabled = false;         /**< Status of the clip flter */
    bool shrinkFilterEnabled = false;       /**< Status of the shrink flter */
    int clipOrigin = 0;                     /**< x position of the clip flter */
    int shrinkFactor = 80;                  /**< Decides how much the planes of the model are */


    //-------------------------------------------------------------------------------------------




};


#endif

