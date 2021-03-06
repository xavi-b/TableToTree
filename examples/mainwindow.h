#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMath>
#include <QTableView>
#include <QTreeView>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QDateTime>
#include "tabletotreemodel.h"
#include "tabletotreewidget.h"

class DemoTableModel : public QAbstractTableModel
{
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

class DemoSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    DemoSortProxyModel(QObject* parent = nullptr) : QSortFilterProxyModel(parent) { }
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
