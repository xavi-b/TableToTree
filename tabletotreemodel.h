#ifndef TABLETOTREEMODEL_H
#define TABLETOTREEMODEL_H

#include <set>
#include <QAbstractTableModel>

struct TableToTreeNode
{
    ~TableToTreeNode()
    {
        for(auto& l : this->lines)
            delete l;
    }

    int row() const
    {
        if(this->parent != nullptr)
            return this->parent->lines.indexOf(const_cast<TableToTreeNode*>(this));

        return 0;
    }

    bool isParent = false;
    TableToTreeNode* parent = nullptr;
    QVector<TableToTreeNode*> lines;
    QVector<QPersistentModelIndex> proxyIndexes;
};

class TableToTreeModel : public QAbstractItemModel
{
private:
    QAbstractTableModel* sourceModel = nullptr;
    std::vector<int> aggregatedColumns;
    QMap<QPersistentModelIndex, QPersistentModelIndex> mapping;
    TableToTreeNode* rootNode = nullptr;

//    void columnsAboutToBeInsertedSlot(const QModelIndex &parent, int first, int last);
//    void columnsAboutToBeMovedSlot(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn);
//    void columnsAboutToBeRemovedSlot(const QModelIndex &parent, int first, int last);
    void columnsInsertedSlot(const QModelIndex &parent, int first, int last);
    void columnsMovedSlot(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column);
    void columnsRemovedSlot(const QModelIndex &parent, int first, int last);
    void dataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void headerDataChangedSlot(Qt::Orientation orientation, int first, int last);
//    void layoutAboutToBeChangedSlot(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void layoutChangedSlot(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
//    void modelAboutToBeResetSlot();
    void modelResetSlot();
//    void rowsAboutToBeInsertedSlot(const QModelIndex &parent, int start, int end);
//    void rowsAboutToBeMovedSlot(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
//    void rowsAboutToBeRemovedSlot(const QModelIndex &parent, int first, int last);
    void rowsInsertedSlot(const QModelIndex &parent, int first, int last);
    void rowsMovedSlot(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void rowsRemovedSlot(const QModelIndex &parent, int first, int last);

public:
    ~TableToTreeModel();
    void setSourceModel(QAbstractTableModel* sourceModel);
    void addAggregatedColumns(int section);
    void insertAggregatedColumns(int index, int section);
    void removeAggregatedColumns(int section);
    void clearAggregatedColumns();
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const ;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int mapSectionToSource(int proxySection) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
//    QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection) const;
//    QItemSelection mapSelectionToSource(const QItemSelection &proxySelection) const;
};


#endif // TABLETOTREEMODEL_H
