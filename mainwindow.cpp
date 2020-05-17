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
        return QString('a'+section);

    if(role == Qt::DisplayRole && orientation == Qt::Vertical)
        return QString::number(section+1);

    return QVariant();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    DemoTableModel* tableModel = new DemoTableModel;
    QTableView* tableView = new QTableView;
    tableView->setModel(tableModel);
    setCentralWidget(tableView);
}

MainWindow::~MainWindow()
{

}
