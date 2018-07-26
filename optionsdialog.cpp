#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}
void OptionsDialog::setWindow(Window *win)
{
    window = win;
}
void OptionsDialog::on_pushButton_clicked()
{
    ui->horizontalSlider->setValue(10  );
}

void OptionsDialog::on_spinBoxRoi_valueChanged(double arg1)
{
    window->updateShaderUniform(Window::SSAO, "ROI", (float)arg1);
}


void OptionsDialog::on_spinBoxAoc_valueChanged(double arg1)
{
     window->updateShaderUniform(Window::SSAO, "AOC", (float)arg1);
     window->updateShaderUniform(Window::SSAO, "AOCSQ", (float)(arg1*arg1));
}

void OptionsDialog::on_spinBoxDelta_valueChanged(double arg1)
{
     window->updateShaderUniform(Window::SSAO, "DELTA", (float)arg1);
}

void OptionsDialog::on_SpinBoxSamples_valueChanged(double arg1)
{
     window->updateShaderUniform(Window::SSAO, "numSamples", (int)arg1);
}

void OptionsDialog::on_doubleSpinBox_valueChanged(double arg1)
{
    window->updateShaderUniform(Window::OTHER, "near", (float)arg1);
}

void OptionsDialog::on_spinFarPlane_valueChanged(double arg1)
{
     window->updateShaderUniform(Window::OTHER, "far", (float)arg1);
}

void OptionsDialog::on_doubleSpinBox_2_valueChanged(double arg1)
{
     window->updateShaderUniform(Window::SSAO, "contrast", (float)arg1);
}


void OptionsDialog::on_scaleSpinBox_3_valueChanged(double arg1)
{
    window->updateShaderUniform(Window::SSAO, "scale", (float)arg1);
}
