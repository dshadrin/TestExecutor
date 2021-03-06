#pragma once

//////////////////////////////////////////////////////////////////////////

#include "ui_VarEdit.h"
#include <QDialog>


//////////////////////////////////////////////////////////////////////////
class CVarEditor : public QDialog
{
    Q_OBJECT

public:
    CVarEditor( const QString& name, QWidget* parent = Q_NULLPTR );

private Q_SLOTS:
    void findFile();
    void findFolder();

public:
    Ui::VarEditDialog uiVarEdit;
};
