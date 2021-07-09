#pragma once

//////////////////////////////////////////////////////////////////////////

#include "ui_VarEdit.h"
#include "Common.h"
#include <QDialog>


//////////////////////////////////////////////////////////////////////////
class CVarEditor : public QDialog
{
    Q_OBJECT

public:
    CVarEditor( const QString& name, QWidget* parent = Q_NULLPTR );

    void SetCurrentType( const QString& str );

private Q_SLOTS:
    void findFile();
    void findFolder();
    void changedType(int index);
    void checkValue( const QString& text );

public:
    Ui::VarEditDialog uiVarEdit;
    ETypeValue m_currentStrType;
};
