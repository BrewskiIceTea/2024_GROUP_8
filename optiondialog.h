#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include "ModelPart.h"

namespace Ui {
class OptionDialog;
}

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(QWidget *parent = nullptr);
    ~OptionDialog();

    // Function to load ModelPart data into the dialog
    void loadFromModelPart(const QString &name, int r, int g, int b, bool visible);

    QString getPartName() const;
    int getRed() const;
    int getGreen() const;
    int getBlue() const;
    bool getVisibility() const;

private:
    Ui::OptionDialog *ui;
};

#endif // OPTIONDIALOG_H


