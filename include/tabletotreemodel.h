#ifndef TABLETOTREEMODEL_H
#define TABLETOTREEMODEL_H

#include <set>
#include <QAbstractTableModel>

namespace XB
{

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

    bool isAnAggregate = true;
    TableToTreeNode* parent = nullptr;
    QVector<TableToTreeNode*> lines;
    QVector<QPersistentModelIndex> proxyIndexes;
};

class TableToTreeModel : public QAbstractItemModel
{
    Q_OBJECT
private:
    QAbstractTableModel* sourceModel = nullptr;
    std::vector<int> aggregatedColumns;
    int aggregationRole = Qt::DisplayRole;
    int mappedSectionRole = Qt::UserRole;
    QMap<QPersistentModelIndex, QPersistentModelIndex> mapping;
    TableToTreeNode* rootNode = nullptr;

    void columnsInsertedSlot(const QModelIndex &parent, int first, int last);
    void columnsMovedSlot(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column);
    void columnsRemovedSlot(const QModelIndex &parent, int first, int last);
    void dataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void headerDataChangedSlot(Qt::Orientation orientation, int first, int last);
    void layoutChangedSlot(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void resetModel();
    void rowsInsertedSlot(const QModelIndex &parent, int first, int last);
    void rowsMovedSlot(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void rowsRemovedSlot(const QModelIndex &parent, int first, int last);

public:
    TableToTreeModel(QObject* parent = nullptr);
    ~TableToTreeModel();
    void setSourceModel(QAbstractTableModel* sourceModel);
    QAbstractTableModel* getSourceModel();
    void setAggregationRole(int aggregationRole);
    int getAggregationRole() const;
    void setMappedSectionRole(int mappedSectionRole);
    int getMappedSectionRole() const;
    void setAggregatedColumns(std::vector<int> const& sections);
    void addAggregatedColumns(int section);
    void insertAggregatedColumns(int index, int section);
    void removeAggregatedColumns(int section);
    void clearAggregatedColumns();
    std::vector<int> const& getAggregatedColumns() const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int mapSectionToSource(int proxySection) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

signals:
    void mappedSectionRoleChanged(TableToTreeModel* treeModel);
    void aggregationChanged(TableToTreeModel* treeModel);
};

}

#endif // TABLETOTREEMODEL_H
