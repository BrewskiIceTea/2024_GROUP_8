#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>

/**
 * @file FilterDialog.h
 * @brief Defines the FilterDialog class for configuring model part filter values
 */

/**
 * @brief The Ui namespace contains the user interface elements for the FilterDialog
 */
namespace Ui {
class FilterDialog;
}

/**
 * @brief Dialog window for configuring clip and shrink filters on a model part
 * @note this dialog window allows the user to enable or disable filters and adjust the filter values applied to the model
 */
class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the FilterDialog
     * @param parent Optional parent widget
     */
    explicit FilterDialog(QWidget *parent = nullptr);

    /**
     * @brief Destroys the FilterDialog and frees resources
     */
    ~FilterDialog();

    /**
     * @brief Loads filter values from the model part into the dialog
     * @param clipFilterEnabled True if the clip filter is enabled
     * @param shrinkFilterEnabled True if the shrink filter is enabled
     * @param clipOrigin the x-value origin point for the clip filter
     * @param shrinkFactor the shrink factor (in range 0–100)
     */
    void loadValuesFromPart(bool clipFilterEnabled, bool shrinkFilterEnabled, int clipOrigin, int shrinkFactor);

    /**
     * @brief Gets the clip filter status
     * @return True = clip filter enabled / False = clip filter disabled
     */
    bool getClipFilterEnabled() const;

    /**
     * @brief Gets the shrink filter status
     * @return True = shrink filter enabled / False = shrink filter disabled
     */
    bool getShrinkFilterEnabled() const;

    /**
     * @brief Gets the current shrink factor
     * @return The shrink factor value (in range 0–100)
     */
    int getShrinkFactor() const;

    /**
     * @brief Gets the current clip origin
     * @return The clip x-value origin value
     */
    int getClipOrigin() const;


private:
    Ui::FilterDialog *ui;   /**< Pointer to the user interface elements. */
};

#endif // FILTERDIALOG_H



