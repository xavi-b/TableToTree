#include "mainwindow.h"

int DemoTableModel::rowCount(const QModelIndex& parent) const
{
    return 10;
}

int DemoTableModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

QVariant DemoTableModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
        return QString('a'+index.column()) + QString::number(qCeil(static_cast<qreal>(index.row()+1)/(index.column()+1)));

    return QVariant();
}

QVariant DemoTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section < 0)
        return QVariant();

    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return QString("column_%1").arg(QChar('a'+section));

    if(role == Qt::DisplayRole && orientation == Qt::Vertical)
        return QString::number(section+1);

    return QVariant();
}

QVariant DemoSortProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return this->sourceModel()->headerData(section, orientation, role);
}

bool DemoSortProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    if (leftData.type() == QVariant::DateTime)
    {
        return leftData.toDateTime() < rightData.toDateTime();
    }
    else
    {
        return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QVBoxLayout* layout = new QVBoxLayout;
    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
    QHBoxLayout* hlayout = new QHBoxLayout;
    layout->addLayout(hlayout);

    DemoTableModel* tableModel = new DemoTableModel;
    QTableView* tableView = new QTableView;
    tableView->setModel(tableModel);
    hlayout->addWidget(tableView);

    TableToTreeModel* treeModel = new TableToTreeModel;
    treeModel->setSourceModel(tableModel);
    TableToTreeWidget* treeWidget = new TableToTreeWidget;
    DemoSortProxyModel* proxyModel = new DemoSortProxyModel;
    proxyModel->setSourceModel(treeModel);
    treeWidget->getTreeView()->setSortingEnabled(true);
    treeWidget->getTreeView()->setModel(proxyModel);
    treeWidget->getTreeView()->expandAll();
    treeWidget->aggregationChangedSlot(treeModel);
    hlayout->addWidget(treeWidget);

    connect(treeWidget, &TableToTreeWidget::aggregationChanged, treeModel, &TableToTreeModel::setAggregatedColumns);
    connect(treeModel, &TableToTreeModel::aggregationChanged, treeWidget, &TableToTreeWidget::aggregationChangedSlot);
    treeModel->addAggregatedColumns(2);
    treeModel->addAggregatedColumns(4);

    QPushButton* btn = new QPushButton;
    layout->addWidget(btn);
    connect(btn, &QPushButton::clicked, this, [=]()
    {

    });
}

MainWindow::~MainWindow()
{

}
