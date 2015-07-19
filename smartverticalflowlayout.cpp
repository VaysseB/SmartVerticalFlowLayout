#include "smartverticalflowlayout.h"

#include <QtWidgets>



SmartVerticalFlowLayout::SmartVerticalFlowLayout(QWidget *parent) :
    QLayout(parent),
    m_hSpace(-1),
    m_vSpace(-1),
    m_isLayoutModified(false)
{
    setContentsMargins(-1, -1, -1, -1);
}

SmartVerticalFlowLayout::~SmartVerticalFlowLayout()
{
    qDeleteAll(m_items);
}

//------------------------------------------------------------------------------

int SmartVerticalFlowLayout::horizontalSpacing() const
{
    if (m_hSpace >= 0)
        return m_hSpace;
    return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int SmartVerticalFlowLayout::verticalSpacing() const
{
    if (m_vSpace >= 0)
        return m_vSpace;
    return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

void SmartVerticalFlowLayout::setHorizontalSpacing(int space)
{
    m_isLayoutModified |= (m_hSpace != space);
    m_hSpace = space;
    doLayout(geometry());
}

void SmartVerticalFlowLayout::setVerticalSpacing(int space)
{
    m_isLayoutModified |= (m_vSpace != space);
    m_vSpace = space;
    doLayout(geometry());
}

//------------------------------------------------------------------------------

int SmartVerticalFlowLayout::count() const
{
    return m_items.size();
}

void SmartVerticalFlowLayout::addItem(QLayoutItem *item)
{
    //m_isLayoutModified = true; is useless here as long as QLayout keeps calling setGeometry
    m_items.append(item);
}

QLayoutItem *SmartVerticalFlowLayout::itemAt(int index) const
{
    return m_items.value(index);
}

QLayoutItem *SmartVerticalFlowLayout::takeAt(int index)
{
    m_isLayoutModified = true;
    QLayoutItem* item = m_items.takeAt(index);
    doLayout(geometry());
    return item;
}

//------------------------------------------------------------------------------

void SmartVerticalFlowLayout::setAlignment(Qt::Alignment align)
{
    m_isLayoutModified = (alignment() != align);
    QLayout::setAlignment(align);
    doLayout(geometry());
}

Qt::Orientations SmartVerticalFlowLayout::expandingDirections() const
{
    if (alignment() == 0 ||
            (alignment() == Qt::AlignJustify))
        return Qt::Horizontal;
    return 0;
}

//------------------------------------------------------------------------------

void SmartVerticalFlowLayout::setGeometry(const QRect &rect)
{
    m_isLayoutModified |= (rect != geometry());
    QLayout::setGeometry(rect);
    doLayout(rect);
}

QSize SmartVerticalFlowLayout::sizeHint() const
{
    return m_structureSize;
}

QSize SmartVerticalFlowLayout::minimumSize() const
{
    return sizeHint();
}

//------------------------------------------------------------------------------

void SmartVerticalFlowLayout::updateLayout()
{
    m_isLayoutModified = true;
    doLayout(geometry());
}

void SmartVerticalFlowLayout::doLayout(const QRect &rect)
{
    if (!m_isLayoutModified)
        return;

    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();

    // update structure (ordering line by lines)
    m_structure.clear();
    QLayoutItem* item = 0;
    QList<QLayoutItem*> rowItems;
    Q_FOREACH(item, m_items) {
        QSize itemSize = sizeOfItem(item);

        // if the item is over the line limit
        if (x + itemSize.width() > effectiveRect.right()+1) {
            // if the line isn't empty => we add to the structure
            // and reset the row
            if (!rowItems.isEmpty()) {
                m_structure.append(rowItems);
                x = effectiveRect.x();
            }
            rowItems.clear();
        }

        rowItems.append(item);
        x += itemSize.width() + horizontalSpacing();
    }

    if (!rowItems.isEmpty())
        m_structure.append(rowItems);


    int y = effectiveRect.y();
    // update items position
    QRect layoutGeometry = QRect(0, 0, 0, 0);
    Q_FOREACH(rowItems, m_structure) {

        // reset horizontal position
        x = effectiveRect.x();

        // calculating row height and row width
        int rowHeight = 0;
        int rowWidth = 0;
        Q_FOREACH(item, rowItems) {
            QSize itemSize = sizeOfItem(item);
            rowWidth += itemSize.width();
            rowHeight = qMax(rowHeight, itemSize.height());
        }

        // placing X coordinates
        int hSpaceTot = horizontalSpacing() * (rowItems.count() - 1);
        int freeSpace = effectiveRect.width() - rowWidth - hSpaceTot;
        // if stretch content => nothing to do (free space is ignored)
        // if no strect and center
        if ((alignment() & Qt::AlignHCenter)) {
            // shift to align center
            x += freeSpace / 2;
        }
        // if no strect and left
        else if ((alignment() & Qt::AlignLeft)) {
            // no shift X (already align to the left)
        }
        // if no strect and right
        else if ((alignment() & Qt::AlignRight)) {
            // shift to align the right
            x += freeSpace;
        }

        qreal cumulStrecthWidth = x;


        // calculating space between elements


        // setting position of row elements
        Q_FOREACH(item, rowItems) {

            QSize itemSize = sizeOfItem(item);

            QRect itemGeometry;

            // if no strect and align
            if ((alignment() & Qt::AlignHCenter)
                     || (alignment() & Qt::AlignLeft)
                     || (alignment() & Qt::AlignRight)) {
                itemGeometry = QRect(x, y, itemSize.width(), rowHeight);

                // moving to next element position
                x += itemSize.width() + horizontalSpacing();
            }
            // if stretch content
            else {
                // getting tru width depending on the pourcentage of width taken by the item
                qreal strecthOptimalWidth = (effectiveRect.width() - hSpaceTot) *
                         ((qreal)itemSize.width()) / ((qreal)rowWidth);

                itemGeometry = QRect(x, y, qFloor(strecthOptimalWidth), rowHeight);

                qreal xIncr = strecthOptimalWidth + horizontalSpacing();
                cumulStrecthWidth += xIncr;
                x = qFloor(cumulStrecthWidth);
            }

            // update geometry of item
            item->setGeometry(itemGeometry);

            // update internal geometry
            layoutGeometry = layoutGeometry.united(itemGeometry);
        }

        y += rowHeight + verticalSpacing();
    }

    layoutGeometry.setHeight(y + bottom);
    layoutGeometry.setWidth(layoutGeometry.width() + right);
    m_structureSize = layoutGeometry.size();

    m_isLayoutModified = false;
}


int SmartVerticalFlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = this->parent();
    QLayout *parentLayout = 0;

    int space = 0;
    if (parent && parent->isWidgetType()) {
        QWidget *pw = static_cast<QWidget *>(parent);
        space = pw->style()->pixelMetric(pm, 0, pw);
    }
    else if ((parentLayout = qobject_cast<QLayout*>(parent)))
        space = parentLayout->spacing();
    return qMax(space, 0);
}


QSize SmartVerticalFlowLayout::sizeOfItem(const QLayoutItem* item) const
{
    QSize itemSizeHint = item->sizeHint();
    QSize itemMinSize = item->minimumSize();
    if (itemSizeHint.width() >= itemMinSize.width() &&
            itemSizeHint.height() >= itemMinSize.height())
        return itemSizeHint;
    return itemMinSize;
}
