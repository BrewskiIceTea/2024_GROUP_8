#include "filterdialog.h"
#include "ui_filterdialog.h"

FilterDialog::FilterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FilterDialog)
{
    ui->setupUi(this);
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

void FilterDialog::loadValuesFromPart(bool clipFilterEnabled, bool shrinkFilterEnabled, int clipOrigin, int shrinkFactor) {

    ui->clipFilter_checkBox->setChecked(clipFilterEnabled);
    ui->shrinkFilter_tickBox->setChecked(shrinkFilterEnabled);

    ui->shrinkFilter_horizontalSlider->setMinimum(0);
    ui->shrinkFilter_horizontalSlider->setMaximum(100);
    ui->shrinkFilter_horizontalSlider->setValue(shrinkFactor);

    ui->clipFilter_horizontalSlider->setValue(clipOrigin);

}


bool FilterDialog::getClipFilterEnabled() const{
    return ui->clipFilter_checkBox->isChecked();
}

bool FilterDialog::getShrinkFilterEnabled() const{
    return ui->shrinkFilter_tickBox->isChecked();
}

int FilterDialog::getClipOrigin() const{
    return ui->clipFilter_horizontalSlider->value();
}
///returns Int but c;o[[omg fo;ter uses decimales :O

int FilterDialog::getShrinkFactor() const{
    return ui->shrinkFilter_horizontalSlider->value();
}



