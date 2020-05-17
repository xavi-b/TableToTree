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
    if(orientation == Qt::Vertical)
        return;

    //TODO improve ?
    int columnCount = this->sourceModel->columnCount() - this->aggregatedColumns.size() + 1;
    emit headerDataChanged(orientation, 0, columnCount);
}

void TableToTreeModel::layoutChangedSlot(const QList<QPersistentModelIndex>& parents, QAbstractItemModel::LayoutChangeHint hint)
{
    this->modelResetSlot();
}

void TableToTreeModel::modelResetSlot()
{
    emit layoutAboutToBeChanged();

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
        TableToTreeNode* parentNode = this->rootNode;
        for(size_t k = 0; k < this->aggregatedColumns.size(); ++k)
        {
            QModelIndex sourceIndex = this->sourceModel->index(i, this->aggregatedColumns[k]);
            auto it = std::find_if(parentNode->lines.begin(), parentNode->lines.end(), [&](TableToTreeNode* child)
            {
                return child->proxyIndexes.at(0).data(this->aggregationRole) == sourceIndex.data(this->aggregationRole);
            });

            // create node if it does not exist
            if(it == parentNode->lines.end())
            {
                TableToTreeNode* childNode = new TableToTreeNode;
                QModelIndex childIndex = this->createIndex(parentNode->lines.size(), 0, childNode);
                childNode->parent = parentNode;
                childNode->proxyIndexes.append(childIndex);
                parentNode->isAnAggregate = true;
                parentNode->lines.append(childNode);
                parentNode = childNode;

                this->mapping.insert(sourceIndex, childIndex);
            }
            else
            {
                parentNode = *it;
            }
        }

        TableToTreeNode* childNode = new TableToTreeNode;
        childNode->isAnAggregate = false;
        childNode->parent = parentNode;

        for(int j = 0; j < columnCount; ++j)
        {
            QModelIndex sourceIndex = this->sourceModel->index(i, mappedSections[j]);

            if(sourceIndex.isValid())
            {
                // create index
                QModelIndex childIndex = this->createIndex(parentNode->lines.size(), j+1, childNode);

                // add it to node
                childNode->proxyIndexes.append(childIndex);

                // add it to mapping
                this->mapping.insert(sourceIndex, childIndex);
            }
        }

        parentNode->lines.append(childNode);
    }

    emit layoutChanged();
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

TableToTreeModel::TableToTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    this->rootNode = new TableToTreeNode;
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

void TableToTreeModel::setAggregationRole(int aggregationRole)
{
    this->aggregationRole = aggregationRole;
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

    if(childNode->isAnAggregate)
    {
        if(column > 0)
            return QModelIndex();

        return childNode->proxyIndexes.at(0);
    }
    else
    {
        if(column == 0)
            return this->createIndex(row, 0, childNode);

        if(column-1 >= childNode->proxyIndexes.size())
            return QModelIndex();

        return childNode->proxyIndexes.at(column-1);
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

    if (!parent.isValid())
        parentNode = this->rootNode;
    else
        parentNode = static_cast<TableToTreeNode*>(parent.internalPointer());

    if(!parentNode->isAnAggregate)
        return 0;

    return parentNode->lines.size();
}

int TableToTreeModel::columnCount(const QModelIndex& parent) const
{
//    if(parent.isValid())
//        return static_cast<TableToTreeNode*>(parent.internalPointer())->proxyIndexes.size();
//    return 1;

    return this->sourceModel->columnCount() - this->aggregatedColumns.size() + 1;
}

QVariant TableToTreeModel::data(const QModelIndex& index, int role) const
{
    return this->sourceModel->data(this->mapToSource(index), role);
}

QVariant TableToTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(this->sourceModel == nullptr)
        return QVariant();

    if(orientation == Qt::Vertical)
        return QVariant();

    if(section < 1)
        return QVariant();

    int mappedSection = this->mapSectionToSource(section-1);

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
