#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class SmartVerticalFlowLayout;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event);

private slots:
    void on_boxExample_currentIndexChanged(int index);
    void on_boxAlignment_currentIndexChanged(int index);

    void updateNumberOfElement(int number);

    void updateMargin();

    void on_spinBoxVerticalSpace_valueChanged(int space);
    void on_spinBoxHorizontalSpace_valueChanged(int space);

    void on_actionShow_not_scrollable_panel_toggled(bool enable);

    void updateElementsPolicy();

private:
    QWidget* createWidget(SmartVerticalFlowLayout* layout, QWidget* master) const;

    QSizePolicy elementPolicy() const;

private:
    Ui::MainWindow *ui;

    SmartVerticalFlowLayout* mp_scrollLayout;
    SmartVerticalFlowLayout* mp_fixedLayout;
};



#include <QPushButton>

class DemoWidget : public QPushButton
{
    Q_OBJECT
public:
    explicit DemoWidget(QWidget* parent, const QSize& hint) : QPushButton(parent) { trueHintSize = hint; }
    virtual ~DemoWidget() {}

    QSize sizeHint() const { return trueHintSize; }

private:
    QSize trueHintSize;
};

#endif // MAINWINDOW_H
