#ifndef SMARTVERTICALFLOWLAYOUT_H
#define SMARTVERTICALFLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>


class SmartVerticalFlowLayout : public QLayout
{
    Q_OBJECT

    Q_PROPERTY(int horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing)
    Q_PROPERTY(int verticalSpacing READ verticalSpacing WRITE setVerticalSpacing)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)

public:
    explicit SmartVerticalFlowLayout(QWidget *parent = 0);
    ~SmartVerticalFlowLayout();

    int count() const;
    void addItem(QLayoutItem *item);
    QLayoutItem *itemAt(int index) const;
    QLayoutItem *takeAt(int index);

    int horizontalSpacing() const;
    int verticalSpacing() const;

    Qt::Orientations expandingDirections() const;

    QSize sizeHint() const;
    QSize minimumSize() const;
    void setGeometry(const QRect &rect);

public slots:
    void setHorizontalSpacing(int space);
    void setVerticalSpacing(int space);

    void setAlignment(Qt::Alignment align);

    void updateLayout();

private:
    void doLayout(const QRect &rect);
    int smartSpacing(QStyle::PixelMetric pm) const;
    QSize sizeOfItem(const QLayoutItem* item) const;

    QList<QLayoutItem *> m_items;
    int m_hSpace;
    int m_vSpace;

    QList<QList<QLayoutItem*> > m_structure;
    QSize m_structureSize;
    bool m_isLayoutModified;

private:
    Q_DISABLE_COPY(SmartVerticalFlowLayout)
};


#endif // SMARTVERTICALFLOWLAYOUT_H
