#include "tabletotreemodel.h"

void TableToTreeModel::columnsInsertedSlot(const QModelIndex& parent, int first, int last)
{

}

void TableToTreeModel::columnsMovedSlot(const QModelIndex& parent, int start, int end, const QModelIndex& destination, int column)
{

}

void TableToTreeModel::columnsRemovedSlot(const QModelIndex& parent, int first, int last)
{

}

void TableToTreeModel::dataChangedSlot(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    for(int i = topLeft.row(); i <= bottomRight.row(); ++i)
    {
        for(int j = topLeft.column(); j <= bottomRight.column(); ++j)
        {
            QModelIndex sourceIndex = this->sourceModel->index(i, j);

            if(sourceIndex.isValid())
            {
                QModelIndex proxyIndex = this->mapFromSource(sourceIndex);

                if(proxyIndex.isValid())
                {
                    emit dataChanged(proxyIndex, proxyIndex, roles);
                }
            }
        }
    }
}

void TableToTreeModel::headerDataChangedSlot(Qt::Orientation orientation, int first, int last)
{
    //TODO vertical ?
    if(orientation == Qt::Vertical)
        return;

    //TODO improve ?
    int columnCount = this->sourceModel->columnCount() - this->aggregatedColumns.size();
    emit headerDataChanged(orientation, 0, columnCount);
}

void TableToTreeModel::layoutChangedSlot(const QList<QPersistentModelIndex>& parents, QAbstractItemModel::LayoutChangeHint hint)
{
    this->modelResetSlot();
}

void TableToTreeModel::modelResetSlot()
{
    this->beginResetModel();

    this->mapping.clear();

    if(this->rootNode != nullptr)
        delete this->rootNode;
    this->rootNode = new TableToTreeNode;

    if(this->sourceModel == nullptr)
        return;

    int columnCount = this->sourceModel->columnCount() - this->aggregatedColumns.size();

    std::vector<int> mappedSections;
    mappedSections.reserve(columnCount);

    for(int j = 0; j < columnCount; ++j)
        mappedSections[j] = this->mapSectionToSource(j);

    for(int i = 0; i < this->sourceModel->rowCount(); ++i)
    {
        for(size_t k = 0; k < this->aggregatedColumns.size(); ++k)
        {
            //proxyindex data contains aggregatedcolumn value
            //find parent tabletotreenode
            //create nodes if does not exist

            //create indexes and add them to node and mapping
            for(int j = 0; j < columnCount; ++j)
            {
                QModelIndex sourceIndex = this->sourceModel->index(i, mappedSections[j]);

                if(sourceIndex.isValid())
                {
                    //TODO add to mapping
                    //TODO add to node
                }
            }
        }
    }

    this->endResetModel();
}

void TableToTreeModel::rowsInsertedSlot(const QModelIndex& parent, int first, int last)
{

}

void TableToTreeModel::rowsMovedSlot(const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row)
{

}

void TableToTreeModel::rowsRemovedSlot(const QModelIndex& parent, int first, int last)
{

}

TableToTreeModel::~TableToTreeModel()
{
    if(this->rootNode != nullptr)
        delete rootNode;
}

void TableToTreeModel::setSourceModel(QAbstractTableModel* sourceModel)
{
    if(this->sourceModel == sourceModel)
        return;

    this->sourceModel = sourceModel;
    this->modelResetSlot();
}

void TableToTreeModel::addAggregatedColumns(int section)
{
    this->aggregatedColumns.push_back(section);
    this->modelResetSlot();
}

void TableToTreeModel::insertAggregatedColumns(int index, int section)
{
    this->aggregatedColumns.insert(this->aggregatedColumns.begin() + index, section);
    this->modelResetSlot();
}

void TableToTreeModel::removeAggregatedColumns(int section)
{
    this->aggregatedColumns.erase(std::find(this->aggregatedColumns.begin(), this->aggregatedColumns.end(), section));
    this->modelResetSlot();
}

void TableToTreeModel::clearAggregatedColumns()
{
    this->aggregatedColumns.clear();
    this->modelResetSlot();
}

QModelIndex TableToTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    TableToTreeNode* parentNode;

    if(!parent.isValid())
        parentNode = this->rootNode;
    else
        parentNode = static_cast<TableToTreeNode*>(parent.internalPointer());

    TableToTreeNode* childNode = parentNode->lines.at(row);

    if(childNode->isParent)
    {
        return childNode->proxyIndexes.at(0);
    }
    else
    {
        if(column >= childNode->proxyIndexes.size())
            return QModelIndex();

        return childNode->proxyIndexes.at(column);
    }
}

QModelIndex TableToTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    TableToTreeNode* childNode = static_cast<TableToTreeNode*>(index.internalPointer());
    TableToTreeNode* parentNode = childNode->parent;

    if (parentNode == rootNode)
        return QModelIndex();

    return parentNode->proxyIndexes.at(0);
}

int TableToTreeModel::rowCount(const QModelIndex& parent) const
{
    TableToTreeNode* parentNode;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentNode = this->rootNode;
    else
        parentNode = static_cast<TableToTreeNode*>(parent.internalPointer());

    return parentNode->lines.count();
}

int TableToTreeModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<TableToTreeNode*>(parent.internalPointer())->proxyIndexes.size();
    return this->rootNode->proxyIndexes.size();
}

QVariant TableToTreeModel::data(const QModelIndex& index, int role) const
{
    return this->sourceModel->data(this->mapToSource(index), role);
}

QVariant TableToTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //TODO vertical ?
    if(orientation == Qt::Vertical)
        return QVariant();

    if(section < 0)
        return QVariant();

    int mappedSection = this->mapSectionToSource(section);

    if(mappedSection < 0 || mappedSection >= this->sourceModel->columnCount())
        return QVariant();

    return this->sourceModel->headerData(mappedSection, orientation, role);
}

int TableToTreeModel::mapSectionToSource(int proxySection) const
{
    std::vector<int> aggregatedColumnsCopy = this->aggregatedColumns;
    std::sort(aggregatedColumnsCopy.begin(), aggregatedColumnsCopy.end());

    int nbColumns = 0;
    int count = std::count_if(aggregatedColumnsCopy.begin(), aggregatedColumnsCopy.end(),
                              [&](int value)
    {
        if(value <= (proxySection+nbColumns))
        {
            nbColumns++;
            return true;
        }

        return false;
    });

    return proxySection + count;
}

QModelIndex TableToTreeModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();
    return this->mapping.value(sourceIndex);
}

QModelIndex TableToTreeModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();
    return this->mapping.key(proxyIndex);
}
