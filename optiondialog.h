#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include "ModelPart.h"

/**
 * @file optionDialog.h
 * @brief Defines the OptionDialog class for configuring model visual components such as colour and visibility
 */



/**
 * @brief The Ui namespace contains the user interface elements for the FilterDialog
 */
namespace Ui {
class OptionDialog;
}

/**
 * @brief Dialog window for configuring colour and visibility of a model part
 * @note this dialog window allows the user to edit the RGB colour components and toggle visibility of the model
 */
class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the ItemOptions dialog
     * @param parent Optional parent widget
     */
    explicit OptionDialog(QWidget *parent = nullptr);

    /**
     * @brief Destroys the ItemOptions Dialog and frees resources (does denconstructor things)
     */
    ~OptionDialog();

    /**
     * @brief Loads data from a ModelPart into the dialog
     * @param name The name of the model part
     * @param r The red colour component value (0–255)
     * @param g The green colour component value (0–255)
     * @param b The blue colour component value (0–255)
     * @param visible The visibility state of the model part
     */
    void loadFromModelPart(const QString &name, int r, int g, int b, bool visible);

    /**
     * @brief Gets the name of the model part
     * @return The part name as a QString
     */
    QString getPartName() const;

    /**
     * @brief get the int Red (R) colour component of the model
     * @return The red colour component value (0-255)
     */
    int getRed() const;

    /**
     * @brief get the int Green (G) colour component of the model
     * @return The green colour component value (0-255)
     */
    int getGreen() const;

    /**
     * @brief get the int Blue (B) colour component of the model
     * @return The blue colour component value (0-255)
     */
    int getBlue() const;

    /** Get visible flag
     *  @brief Gets the bool model visibility value
      * @return visible flag as boolean
      */
    bool getVisibility() const;

private:
    Ui::OptionDialog *ui;   /**< Pointer to the user interface elements. */
};

#endif // OPTIONDIALOG_H


