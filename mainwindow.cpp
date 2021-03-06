#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>

#include "smartverticalflowlayout.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mp_scrollLayout(NULL)
{
    ui->setupUi(this);

    mp_scrollLayout = new SmartVerticalFlowLayout(ui->scrollableContent);
    ui->scrollableContent->setLayout(mp_scrollLayout);
    ui->scrollArea->setWidgetResizable(true);

    //
    ui->boxAlignment->setItemData(0, Qt::AlignJustify);
    ui->boxAlignment->setItemData(1, Qt::AlignLeft);
    ui->boxAlignment->setItemData(2, Qt::AlignHCenter);
    ui->boxAlignment->setItemData(3, Qt::AlignRight);

    //
    connect(ui->actionQuit, SIGNAL(triggered()),
            qApp, SLOT(quit()));

    connect(ui->slideNumberElements, SIGNAL(valueChanged(int)),
            this, SLOT(updateNumberOfElement(int)));
    connect(ui->spinBoxHorizontalMargins, SIGNAL(valueChanged(int)),
            this, SLOT(updateMargin()));
    connect(ui->spinBoxVerticalMargins, SIGNAL(valueChanged(int)),
            this, SLOT(updateMargin()));
    connect(ui->checkSizePolicy, SIGNAL(toggled(bool)),
            this, SLOT(updateElementsPolicy()));

    //
    updateMargin();
    updateNumberOfElement(ui->slideNumberElements->value());
    on_boxAlignment_currentIndexChanged(ui->boxAlignment->currentIndex());
    on_spinBoxVerticalSpace_valueChanged(ui->spinBoxVerticalSpace->value());
    on_spinBoxHorizontalSpace_valueChanged(ui->spinBoxHorizontalSpace->value());
    on_spinBoxMaxElements_valueChanged(ui->spinBoxMaxElements->value());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateNumberOfElement(int number)
{
    ui->groupNumberElements->setTitle(QStringLiteral("Elements: %1").arg(number));

    while (mp_scrollLayout->count() < number)
        mp_scrollLayout->addWidget(createWidget(mp_scrollLayout, ui->scrollableContent));

    while (mp_scrollLayout->count() > number) {
        QLayoutItem* item = mp_scrollLayout->takeAt(mp_scrollLayout->count()-1);
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }
}

QWidget* MainWindow::createWidget(SmartVerticalFlowLayout *layout, QWidget *master) const
{
    static const QSize unitSize = QSize(60, 35);

    int counter = layout->count();
    int xFact = 1, yFact = 1;

    // small & average
    if (ui->boxExample->currentIndex() == 1) {
        // 0,1,2 -> small
        // 3 -> average (horizontal)
        // 4,5 -> small
        // 6 -> average (vertical)

        int modCounter = counter % 7;
        if (modCounter == 3)
            xFact = 2;
        else if (modCounter == 6)
            yFact = 2;
    }
    // all sizes
    else if (ui->boxExample->currentIndex() == 2) {
        // 20/30 -> small
        // 4,8 -> average
        // 12 -> square*3
        // 15 -> 2*3
        // 18 -> 3*1
        // 24 -> 1*3
        // 29 -> 5*2

        int modCounter = counter % 30;
        if (modCounter == 4)
            xFact = 2;
        else if (modCounter == 8)
            yFact = 2;
        else if (modCounter == 12) {
            xFact = 3;
            yFact = 3;
        }
        else if (modCounter == 15) {
            xFact = 2;
            yFact = 3;
        }
        else if (modCounter == 18)
            xFact = 3;
        else if (modCounter == 24)
            yFact = 3;
        else if (modCounter == 29) {
            xFact = 5;
            yFact = 2;
        }
    }

    QSize size = QSize(unitSize.width()*xFact, unitSize.height()*yFact);

    QPushButton* button = new DemoWidget(master, size);
    button->setText(QString("#%1 (%2x%3)")
                    .arg(counter).arg(xFact).arg(yFact));

    QWidget* widget = button;
    widget->setSizePolicy(elementPolicy());

    widget->setMinimumSize(size);
    layout->addWidget(widget);
    return widget;
}

void MainWindow::updateMargin()
{
    mp_scrollLayout->setContentsMargins(
                ui->spinBoxHorizontalMargins->value(),
                ui->spinBoxVerticalMargins->value(),
                ui->spinBoxHorizontalMargins->value(),
                ui->spinBoxVerticalMargins->value());
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    ui->scrollableContent->resize(mp_scrollLayout->sizeHint());
}

void MainWindow::on_boxExample_currentIndexChanged(int)
{
    updateNumberOfElement(0);
    updateNumberOfElement(ui->slideNumberElements->value());
}

void MainWindow::on_boxAlignment_currentIndexChanged(int index)
{
    Qt::Alignment align = (Qt::Alignment)ui->boxAlignment->itemData(index).toInt();
    mp_scrollLayout->setAlignment(align);
}

void MainWindow::on_spinBoxVerticalSpace_valueChanged(int space)
{
    mp_scrollLayout->setVerticalSpacing(space);
}

void MainWindow::on_spinBoxHorizontalSpace_valueChanged(int space)
{
    mp_scrollLayout->setHorizontalSpacing(space);
}

void MainWindow::updateElementsPolicy()
{
    updateNumberOfElement(0);
    updateNumberOfElement(ui->slideNumberElements->value());
}

QSizePolicy MainWindow::elementPolicy() const
{
    if (ui->checkSizePolicy->isChecked())
        return QSizePolicy(QSizePolicy::Preferred,
                           QSizePolicy::Preferred);
    // or else, return default policy
    return QSizePolicy(QSizePolicy::Minimum,
                       QSizePolicy::Fixed);
}

void MainWindow::on_spinBoxMaxElements_valueChanged(int count)
{
    mp_scrollLayout->setMaxRowCount(count);
}
