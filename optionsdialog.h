#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include "window.h"

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();
    void setWindow(Window *win);

private slots:
    void on_pushButton_clicked();

    void on_spinBoxRoi_valueChanged(double arg1);

    void on_spinBoxAoc_valueChanged(double arg1);

    void on_spinBoxDelta_valueChanged(double arg1);

    void on_SpinBoxSamples_valueChanged(double arg1);

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_spinFarPlane_valueChanged(double arg1);

private:
    Ui::OptionsDialog *ui;
    Window *window;
};

#endif // OPTIONSDIALOG_H
