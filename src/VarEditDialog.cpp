#include "VarEditDialog.h"
#include "Common.h"

//////////////////////////////////////////////////////////////////////////
CVarEditor::CVarEditor( const QString& name, QWidget* parent)
{
    uiVarEdit.setupUi( this );
    setWindowTitle( name );
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );
    setAttribute( Qt::WA_CustomWhatsThis );

    QObject::connect( uiVarEdit.pushButtonFiles, &QPushButton::clicked, this, &CVarEditor::findFile );
    QObject::connect( uiVarEdit.pushButtonFolders, &QPushButton::clicked, this, &CVarEditor::findFolder );
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
            auto selectionEnd = [](QLineEdit* le) -> int
            {
#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
                return le->selectionStart() + le->selectedText().length();
#else
                return le->selectionEnd();
#endif
            };
            auto selectionLength = [](QLineEdit* le) -> int
            {
#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
                return le->selectedText().length();
#else
                return le->selectionLength();
#endif
            };
            bool isInsertSeparator = selectionEnd(le) == oldText.length() ? false : true;
            if (le->selectionStart() > 0)
            {
                fName.insert( 0, LIST_PATH_SEPARATOR );
            }
            if (selectionEnd(le) < oldText.length())
            {
                fName.append( LIST_PATH_SEPARATOR );
            }
            oldText.replace( le->selectionStart(), selectionLength(le), fName );
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

