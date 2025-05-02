#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>

namespace Ui {
class FilterDialog;
}

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(QWidget *parent = nullptr);
    ~FilterDialog();

    void loadValuesFromPart(bool clipFilterEnabled, bool shrinkFilterEnabled, int clipOrigin, int shrinkFactor);

    bool getClipFilterEnabled() const;
    bool getShrinkFilterEnabled() const;
    int getShrinkFactor() const;
    int getClipOrigin() const;


private:
    Ui::FilterDialog *ui;
};

#endif // FILTERDIALOG_H



