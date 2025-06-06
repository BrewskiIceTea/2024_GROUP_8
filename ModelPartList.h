/**     @file ModelPartList.h
  *
  *     EEEE2076 - Software Engineering & VR Project
  *
  *     Template for model part list that will be used to create the trewview.
  *
  *     P Evans 2022
  */

#ifndef VIEWER_MODELPARTLIST_H
#define VIEWER_MODELPARTLIST_H


#include "ModelPart.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QString>
#include <QList>



class ModelPart;

class ModelPartList : public QAbstractItemModel {
    Q_OBJECT        /**< A special Qt tag used to indicate that this is a special Qt class that might require preprocessing before compiling. */
public:
    /** Constructor
      *  Arguments are standard arguments for this type of class but are not used in this example.
      * @param data is not used
      * @param parent is used by the parent class constructor
      */
    ModelPartList( const QString& data, QObject* parent = NULL );

    /** Destructor
      *  Frees root item allocated in constructor
      */
    ~ModelPartList();

    /** Return column count
      * @param parent is not used
      * @return number of columns in the tree view - "Part" and "Visible", i.e. 2 in this case
      */
    int columnCount( const QModelIndex& parent ) const;

    /** This returns the value of a particular row (i.e. the item index) and
      *  columns (i.e. either the "Part" or "Visible" property).
      *  It is used by QT internally - this is how Qt retrieves the text to display in the TreeView
      * @param index in a stucture Qt uses to specify the row and column it wants data for
      * @param role is how Qt specifies what it wants to do with the data
      * @return a QVariant which is a generic variable used to represent any Qt class type, in this case the QVariant will be a string
      */
    QVariant data( const QModelIndex& index, int role ) const;

    /** Standard function used by Qt internally.
      * @param index in a stucture Qt uses to specify the row and column it wants data for
      * @return a Qt item flags
      */
    Qt::ItemFlags flags( const QModelIndex& index ) const;


    /** Standard function used by Qt internally.
      */
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;


    /** Get a valid QModelIndex for a location in the tree (row is the row in the tree under "parent"
      * or under the root of the tree if parent isnt specified. Column is either 0 = "Part" or 1 = "Visible"
      * in this example
      * @param row is the item index
      * @param column is 0 or 1 - part name or visible stringstream
      * @param parent where the row is referenced from, usually the tree root
      * @return the QModelIndex structure
     */
    QModelIndex index( int row, int column, const QModelIndex& parent ) const;


    /** Take a QModelIndex for an item, get a QModel Index for its parent
      * @param index of item
      * @return index of parent
      */
    QModelIndex parent( const QModelIndex& index ) const;

    /** Get number of rows (items) under an item in tree
      *  @param is the parent, all items under this will be counted
      *  @return number of children
      */
    int rowCount( const QModelIndex& parent ) const;

    /** Get a pointer to the root item of the tree
      * @return the root item pointer
      */
    ModelPart* getRootItem();

    /**
     * @brief Gets a ModelPart pointer from a QModelIndex
     * @param index The model index to get the part from
     * @return ModelPart* Pointer to the part, or nullptr if invalid index
     */
    ModelPart* getPart(const QModelIndex& index);
    
    /**
     * @brief Removes a part from the model at the specified index
     * @param index The QModelIndex of the part to remove
     * @return true if the part was successfully removed, false if failed
     */
    bool removePart(const QModelIndex &index);

    /**
     * @brief Appends a child item to the parent index
     *
     * @note Creates a new child item with the provided data and attaches it under the parent in the model
     *
     * @param parent The QModelIndex of the parent item
     * @param data A list of QVariant values representing the new child’s data
     * @return The QModelIndex of the newly added child item
     */
    QModelIndex appendChild( QModelIndex& parent, const QList<QVariant>& data );

    /**
     * @brief Inserts a new part as a child of the root item in the model hierarchy
     * @note This function appends the given MOdelPart to the root of the part tree and notifys the view/model system of the change if necessary
     * @param newPart Pointer to the ModelPart to be inserted at the root level
     */
    void insertPartAtRoot(ModelPart* newPart);

    /**
     * @brief Retrieves the root item of the part hierarchy
     * Returns a pointer to the root ModelPart, which serves as the top-level node in the model's tree structure
     * @return Pointer to the root ModelPart
     */
    ModelPart* getRootItem() const;


private:
    ModelPart *rootItem;    /**< This is a pointer to the item at the base of the tree */
};
#endif

