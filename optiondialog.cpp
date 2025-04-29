#include "optiondialog.h"
#include "ui_optiondialog.h"

    OptionDialog::OptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionDialog)
{
    ui->setupUi(this);
}

OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::loadFromModelPart(const QString &name, int r, int g, int b, bool visible) {
    ui->lineEdit->setText(name);
    ui->spinBox_R->setValue(r);
    ui->spinBox_G->setValue(g);
    ui->spinBox_B->setValue(b);
    ui->checkBox->setChecked(visible);
}

QString OptionDialog::getPartName() const {
    return ui->lineEdit->text();
}

int OptionDialog::getRed() const {
    return ui->spinBox_R->value();
}

int OptionDialog::getGreen() const {
    return ui->spinBox_G->value();
}

int OptionDialog::getBlue() const {
    return ui->spinBox_B->value();
}

bool OptionDialog::getVisibility() const {
    return ui->checkBox->isChecked();
}
