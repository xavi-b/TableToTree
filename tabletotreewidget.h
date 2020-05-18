#ifndef TABLETOTREEWIDGET_H
#define TABLETOTREEWIDGET_H

#include <QTreeView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QListWidget>
#include <QEvent>
#include <QDropEvent>
#include <QApplication>
#include <QMimeData>
#include <QDrag>
#include "tabletotreemodel.h"

QString const mimeDataType = "application/x-item";

class AggregationListWidget : public QListWidget
{
    Q_OBJECT
protected:
    void dragMoveEvent(QDragMoveEvent* e);
    void dropEvent(QDropEvent* event);
    void startDrag(Qt::DropActions supportedActions);
    void dragEnterEvent(QDragEnterEvent* event);
    QFlags<Qt::DropAction> supportedDropActions() const;
    void keyPressEvent(QKeyEvent* event);

public:
    AggregationListWidget(QWidget* parent = nullptr);
    void addItem(int column, QString name);
    std::vector<int> getAggregatedColumns() const;

signals:
    void aggregationChanged();
};

class TableToTreeHeaderView : public QHeaderView
{
    Q_OBJECT
private:
    int mappedSectionRole = Qt::UserRole;

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void dragMoveEvent(QDragMoveEvent* e);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

public:
    TableToTreeHeaderView(QWidget* parent = nullptr);
    void setMappedSectionRole(int mappedSectionRole);

signals:
    void aggregationChanged();
};

class TableToTreeView : public QTreeView
{
    Q_OBJECT
public:
    TableToTreeView(QWidget* parent = nullptr);
};

class TableToTreeWidget : public QWidget
{
    Q_OBJECT
private:
    AggregationListWidget* aggregationListWidget;
    TableToTreeHeaderView* headerView;
    TableToTreeView* treeView;

public:
    TableToTreeWidget(QWidget *parent = nullptr);
    TableToTreeView* getTreeView();
    void aggregationChangedSlot(TableToTreeModel* treeModel);
    void mappedSectionRoleChangedSlot(TableToTreeModel* treeModel);

signals:
    void aggregationChanged(std::vector<int> sections);
};

#endif // TABLETOTREEWIDGET_H
