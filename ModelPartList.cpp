/**     @file ModelPartList.h
  *
  *     EEEE2076 - Software Engineering & VR Project
  *
  *     Template for model part list that will be used to create the trewview.
  *
  *     P Evans 2022
  */

#include "ModelPartList.h"
#include "ModelPart.h"

ModelPartList::ModelPartList( const QString& data, QObject* parent ) : QAbstractItemModel(parent) {
    /* Have option to specify number of visible properties for each item in tree - the root item
     * acts as the column headers
     */
    rootItem = new ModelPart( { tr("Part"), tr("Visible") } );
}



ModelPartList::~ModelPartList() {
    delete rootItem;
}


int ModelPartList::columnCount( const QModelIndex& parent ) const {
    Q_UNUSED(parent);

    return rootItem->columnCount();
}


QVariant ModelPartList::data( const QModelIndex& index, int role ) const {
    /* If the item index isnt valid, return a new, empty QVariant (QVariant is generic datatype
     * that could be any valid QT class) */
    if( !index.isValid() )
        return QVariant();

    /* Role represents what this data will be used for, we only need deal with the case
     * when QT is asking for data to create and display the treeview. Return a new,
     * empty QVariant if any other request comes through. */
    if (role != Qt::DisplayRole)
        return QVariant();

    /* Get a a pointer to the item referred to by the QModelIndex */
    ModelPart* item = static_cast<ModelPart*>( index.internalPointer() );

    /* Each item in the tree has a number of columns ("Part" and "Visible" in this 
     * initial example) return the column requested by the QModelIndex */
    return item->data( index.column() );
}


Qt::ItemFlags ModelPartList::flags( const QModelIndex& index ) const {
    if( !index.isValid() )
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags( index );
}


QVariant ModelPartList::headerData( int section, Qt::Orientation orientation, int role ) const {
    if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        return rootItem->data( section );

    return QVariant();
}

ModelPart* ModelPartList::getPart(const QModelIndex& index) {
    if (!index.isValid())
        return rootItem;
    return static_cast<ModelPart*>(index.internalPointer());
}

QModelIndex ModelPartList::index(int row, int column, const QModelIndex& parent) const {
    ModelPart* parentItem;
    
    if( !parent.isValid() || !hasIndex(row, column, parent) )
        parentItem = rootItem;              // default to selecting root 
    else
        parentItem = static_cast<ModelPart*>(parent.internalPointer());

    ModelPart* childItem = parentItem->child(row);
    if( childItem )
        return createIndex(row, column, childItem);
    
    
    return QModelIndex();
}


QModelIndex ModelPartList::parent( const QModelIndex& index ) const {
    if (!index.isValid())
        return QModelIndex();

    ModelPart* childItem = static_cast<ModelPart*>(index.internalPointer());
    ModelPart* parentItem = childItem->parentItem();

    if( parentItem == rootItem )
        return QModelIndex();

    return createIndex( parentItem->row(), 0, parentItem );
}


int ModelPartList::rowCount( const QModelIndex& parent ) const {
    ModelPart* parentItem;
    if( parent.column() > 0 )
        return 0;

    if( !parent.isValid() )
        parentItem = rootItem;
    else
        parentItem = static_cast<ModelPart*>(parent.internalPointer());

    return parentItem->childCount();
}


ModelPart* ModelPartList::getRootItem() {
    return rootItem; 
}



QModelIndex ModelPartList::appendChild(QModelIndex& parent, const QList<QVariant>& data) {      
    ModelPart* parentPart;

    if (parent.isValid())
        parentPart = static_cast<ModelPart*>(parent.internalPointer());
    else {
        parentPart = rootItem;
        parent = createIndex(0, 0, rootItem );
    }

    beginInsertRows( parent, rowCount(parent), rowCount(parent) ); 

    ModelPart* childPart = new ModelPart( data, parentPart );

    parentPart->appendChild(childPart);

    QModelIndex child = createIndex(0, 0, childPart);

    endInsertRows();

    emit layoutChanged();

    return child;
}

bool ModelPartList::removePart(const QModelIndex &index){
    if (!index.isValid())
        return false;

    ModelPart* partToBeRemoved = static_cast<ModelPart*>(index.internalPointer());
    ModelPart* parent = partToBeRemoved->parentItem();

    if (!parent)
        return false;

    int row = partToBeRemoved->row(); // assumes ModelPart::row() gives correct position in parent's children

    beginRemoveRows(index.parent(), row, row);
    parent->removeChild(partToBeRemoved); // make sure this deletes the child from the parent's list
    //delete part; // free memory if you're managing raw pointers
    endRemoveRows();

    emit layoutChanged(); // optional but helpful to refresh the view

    qDebug() << "removePart:";

    return true;

}

void ModelPartList::insertPartAtRoot(ModelPart* newPart) {
    ModelPart* rootItem = getRootItem();
    int row = rootItem->childCount();

    beginInsertRows(QModelIndex(), row, row);
    rootItem->appendChild(newPart);
    endInsertRows();
}

ModelPart* ModelPartList::getRootItem() const {
    return rootItem; // assuming `rootItem` is a private member
}
