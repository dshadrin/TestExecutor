#include "VarEditDialog.h"
#include "AppConfig.h"
#include <QColorDialog>

//////////////////////////////////////////////////////////////////////////
CVarEditor::CVarEditor( const QString& name, QWidget* parent) :
    m_currentStrType(ETypeValue::StringValue)
{
    uiVarEdit.setupUi( this );
    setWindowTitle( name );
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );
    setAttribute( Qt::WA_CustomWhatsThis );

    int pos = 0;
    //QList<QString> types = CAppConfig::GetValueTypesList();
    //for (auto& type : types)
    //{
    //    uiVarEdit.comboBoxTypeJson->addItem( type );
    //    if (type == "STRING")
    //    {
    //        uiVarEdit.comboBoxTypeJson->setCurrentIndex( pos );
    //    }
    //    ++pos;
    //}

    QObject::connect( uiVarEdit.pushButtonFiles, &QPushButton::clicked, this, &CVarEditor::findFile );
    QObject::connect( uiVarEdit.pushButtonFolders, &QPushButton::clicked, this, &CVarEditor::findFolder );
    QObject::connect( uiVarEdit.comboBoxTypeJson, &QComboBox::currentIndexChanged, this, &CVarEditor::changedType );
    QObject::connect( uiVarEdit.lineEditValue, &QLineEdit::textChanged, this, &CVarEditor::checkValue );
    QObject::connect( uiVarEdit.pushButtonColor, &QPushButton::clicked, this, &CVarEditor::selectColor );
}


void CVarEditor::SetCurrentType( const QString& str )
{
    int count = uiVarEdit.comboBoxTypeJson->count();
    for (int i = 0; i < count; ++i)
    {
        if (str == uiVarEdit.comboBoxTypeJson->itemText( i ))
        {
            uiVarEdit.comboBoxTypeJson->setCurrentIndex( i );
            break;
        }
    }
}

void CVarEditor::findFile()
{
    QString fName = util::FindFile();
    if (!fName.isEmpty())
    {
        uiVarEdit.lineEditValue->setText( fName );
    }
}

void CVarEditor::findFolder()
{
    QString fName = util::FindFolder();
    if (!fName.isEmpty())
    {
        QLineEdit* le = uiVarEdit.lineEditValue;
        QString oldText = le->text();
        if (oldText.isEmpty())
        {
            le->setText( fName );
        }
        else if (le->hasSelectedText())
        {
            bool isInsertSeparator = le->selectionEnd() == oldText.length() ? false : true;
            if (le->selectionStart() > 0)
            {
                fName.insert( 0, LIST_PATH_SEPARATOR );
            }
            if (le->selectionEnd() < oldText.length())
            {
                fName.append( LIST_PATH_SEPARATOR );
            }
            oldText.replace( le->selectionStart(), le->selectionLength(), fName );
            le->setText( oldText );
        }
        else
        {
            int pos = le->cursorPosition();
            if (pos == 0)
            {
                fName.append( LIST_PATH_SEPARATOR );
                oldText.insert( 0, fName );
            }
            else if (pos == oldText.length())
            {
                fName.insert( 0, LIST_PATH_SEPARATOR );
                oldText.append( fName );
            }
            else
            {
                fName.insert( 0, LIST_PATH_SEPARATOR );
                fName.append( LIST_PATH_SEPARATOR );
                oldText.insert( pos, fName );
            }
            le->setText( oldText );
        }
    }
}

void CVarEditor::changedType( int index )
{
    QString text = uiVarEdit.comboBoxTypeJson->currentText();
    //m_currentStrType = CAppConfig::StringTypeToProjectType( text );
    QString value = uiVarEdit.lineEditValue->text();
    checkValue( value );
}

void CVarEditor::checkValue( const QString& text )
{
    bool check = CAppConfig::CheckStringValue( text, m_currentStrType );
    if (check)
    {
        uiVarEdit.lineEditValue->setStyleSheet( QString::fromUtf8( "background-color: rgb(85, 255, 127);" ) );
        uiVarEdit.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( true );
    }
    else
    {
        uiVarEdit.lineEditValue->setStyleSheet( QString::fromUtf8( "background-color: rgb( 255, 161, 158 );" ) );
        uiVarEdit.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    }
}

void CVarEditor::selectColor()
{
    // select color
    QColor color = QColorDialog::getColor(QColor{}, this, tr("Select color"), QColorDialog::ShowAlphaChannel);
    int red = color.red();
    int green = color.green();
    int blue = color.blue();
    QString text = QString::asprintf("rgb( %d, %d, %d )", red, green, blue);
    uiVarEdit.lineEditValue->setText(text);
}

