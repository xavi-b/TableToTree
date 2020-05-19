#include "tabletotreewidget.h"

void AggregationListWidget::dragMoveEvent(QDragMoveEvent* e)
{
    if (e->mimeData()->hasFormat(mimeDataType))
    {
        e->setDropAction(Qt::MoveAction);
        e->accept();
    }
    else
    {
        e->ignore();
    }
}

void AggregationListWidget::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat(mimeDataType))
    {
        event->accept();
        event->setDropAction(Qt::MoveAction);
        QByteArray ba = event->mimeData()->data(mimeDataType);
        QDataStream stream(&ba, QIODevice::ReadOnly);
        int column;
        QString name;
        stream >> column;
        stream >> name;

        auto item = itemAt(event->pos());
        if(item == nullptr)
            addItem(column, name);
        else
        {
            int r = row(item);
            QListWidgetItem* item = new QListWidgetItem;
            item->setText(name);
            item->setData(Qt::UserRole, column);
            insertItem(r, item);
        }

        // drop will either add or move aggregation column
        emit aggregationChanged();
    }
    else
    {
        event->ignore();
    }
}

void AggregationListWidget::startDrag(Qt::DropActions supportedActions)
{
    QListWidgetItem* item = currentItem();
    QMimeData* mimeData = new QMimeData;
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    int column = item->data(Qt::UserRole).toInt();
    QString name = item->text();
    stream << column;
    stream << name;
    mimeData->setData(mimeDataType, ba);
    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    int r = row(item);
    delete takeItem(r);
    if(drag->exec(Qt::MoveAction | Qt::IgnoreAction, Qt::IgnoreAction) == Qt::IgnoreAction)
    {
        QListWidgetItem* item = new QListWidgetItem;
        item->setText(name);
        item->setData(Qt::UserRole, column);
        insertItem(r, item);
    }
}

void AggregationListWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(mimeDataType))
        event->accept();
    else
        event->ignore();
}

QFlags<Qt::DropAction> AggregationListWidget::supportedDropActions() const
{
    return Qt::MoveAction | Qt::IgnoreAction;
}

void AggregationListWidget::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Delete)
    {
        event->accept();
        QListWidgetItem* item = currentItem();
        delete takeItem(row(item));

        // remove aggregation column which will reset treeview
        emit aggregationChanged();
        return;
    }

    QListWidget::keyPressEvent(event);
}

AggregationListWidget::AggregationListWidget(QWidget* parent)
    : QListWidget(parent)
{
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setViewMode(QListWidget::IconMode);
    this->setDefaultDropAction(Qt::MoveAction);
    this->setAcceptDrops(true);
    this->setDropIndicatorShown(true);
    this->setDragEnabled(true);
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
}

void AggregationListWidget::addItem(int column, QString name)
{
    QListWidgetItem* item = new QListWidgetItem;
    item->setText(name);
    item->setData(Qt::UserRole, column);
    QListWidget::addItem(item);
}

std::vector<int> AggregationListWidget::getAggregatedColumns() const
{
    std::vector<int> sections;

    for(int i = 0; i < this->count(); ++i)
        sections.push_back(this->item(i)->data(Qt::UserRole).toInt());

    return sections;
}

void TableToTreeHeaderView::dragMoveEvent(QDragMoveEvent* e)
{
    if (e->mimeData()->hasFormat(mimeDataType))
    {
        e->setDropAction(Qt::MoveAction);
        e->accept();
    }
    else
    {
        e->ignore();
    }
}

void TableToTreeHeaderView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(mimeDataType))
        event->accept();
    else
        event->ignore();
}

void TableToTreeHeaderView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(mimeDataType) && event->source() != this)
    {
        event->accept();
        event->setDropAction(Qt::MoveAction);

//        QByteArray ba = event->mimeData()->data(mimeDataType);
//        QDataStream stream(&ba, QIODevice::ReadOnly);
//        int column;
//        QString name;
//        stream >> column;
//        stream >> name;

        // drop will remove aggregation column
        emit aggregationChanged();
    }
    else
    {
        event->ignore();
    }
}

void TableToTreeHeaderView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if(!this->rect().contains(event->pos()))
        {
            int index = logicalIndexAt(event->pos());
            QMimeData* mimeData = new QMimeData;
            QByteArray ba;
            QDataStream stream(&ba, QIODevice::WriteOnly);
            stream << this->model()->headerData(index, Qt::Horizontal, this->mappedSectionRole).toInt();
            stream << this->model()->headerData(index, Qt::Horizontal, Qt::DisplayRole).toString();
            mimeData->setData(mimeDataType, ba);
            QDrag* drag = new QDrag(this);
            drag->setMimeData(mimeData);
            if(drag->exec(Qt::MoveAction) == Qt::MoveAction)
            {
                //TRICK to "cancel" moving section
                QMouseEvent mouseEvent(QEvent::MouseMove, event->localPos(), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
                QApplication::sendEvent(this, &mouseEvent);
                return;
            }
        }
    }

    QHeaderView::mouseMoveEvent(event);
}

TableToTreeHeaderView::TableToTreeHeaderView(QWidget* parent)
    : QHeaderView(Qt::Horizontal, parent)
{
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setStretchLastSection(false);
    this->setSectionsMovable(true);
}

void TableToTreeHeaderView::setMappedSectionRole(int mappedSectionRole)
{
    this->mappedSectionRole = mappedSectionRole;
}

TableToTreeView::TableToTreeView(QWidget* parent)
    : QTreeView(parent)
{
    this->setAlternatingRowColors(true);
}

TableToTreeWidget::TableToTreeWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout;

    this->aggregationListWidget = new AggregationListWidget;

    this->headerView = new TableToTreeHeaderView;
    this->treeView = new TableToTreeView;
    this->treeView->setHeader(this->headerView);

    layout->addWidget(this->aggregationListWidget);
    layout->addWidget(this->treeView, 1);
    this->setLayout(layout);

    connect(this->aggregationListWidget, &AggregationListWidget::aggregationChanged, this, [=]()
    {
        emit aggregationChanged(this->aggregationListWidget->getAggregatedColumns());
    });

    connect(this->headerView, &TableToTreeHeaderView::aggregationChanged, this, [=]()
    {
        emit aggregationChanged(this->aggregationListWidget->getAggregatedColumns());
    });
}

TableToTreeView* TableToTreeWidget::getTreeView()
{
    return this->treeView;
}

void TableToTreeWidget::changeAggregation(TableToTreeModel* treeModel)
{
    this->aggregationListWidget->clear();

    for(auto const& c : treeModel->getAggregatedColumns())
        this->aggregationListWidget->addItem(c, treeModel->getSourceModel()->headerData(c, Qt::Horizontal).toString());

    this->headerView->setMappedSectionRole(treeModel->getMappedSectionRole());
}

void TableToTreeWidget::changeMappedSectionRole(TableToTreeModel* treeModel)
{
    this->headerView->setMappedSectionRole(treeModel->getMappedSectionRole());
}
