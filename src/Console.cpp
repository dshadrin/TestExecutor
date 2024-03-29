#include "Console.h"
#include <QScrollBar>
#include <QProcess>
//#include <QStringDecoder>
//#include <QStringEncoder>

IMPLEMENT_MODULE_TAG( Console, "ECON" );

Console::Console( /*VectorValues params,*/ QWidget *parent) :
    QPlainTextEdit(parent),
//    m_params(params),
    m_process( this )
{
    prompt = "#> ";

    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);

    history = new QStringList;
    historyPos = 0;
    insertPrompt(false);
    isLocked = false;

    QObject::connect( &m_process, &CExternalProcess::OnString, this, &Console::OnOutput );
    RunProcess();
}

void Console::keyPressEvent(QKeyEvent *event)
{
    if(isLocked)
	return;
    if(event->key() >= 0x20 && event->key() <= 0x7e
       && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier))
	QPlainTextEdit::keyPressEvent(event);
    if(event->key() == Qt::Key_Backspace
       && event->modifiers() == Qt::NoModifier
       && textCursor().positionInBlock() > prompt.length())
	QPlainTextEdit::keyPressEvent(event);
    if(event->key() == Qt::Key_Return && event->modifiers() == Qt::NoModifier)
	onEnter();
    if(event->key() == Qt::Key_Up && event->modifiers() == Qt::NoModifier)
	historyBack();
    if(event->key() == Qt::Key_Down && event->modifiers() == Qt::NoModifier)
	historyForward();
}

void Console::mousePressEvent(QMouseEvent *)
{
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *){}

void Console::contextMenuEvent(QContextMenuEvent *){}

void Console::onEnter()
{
    /*
    if(textCursor().positionInBlock() == prompt.length())
    {
        insertPrompt();
        return;
    }
    QString cmd = textCursor().block().text().mid(prompt.length());
    isLocked = true;
    historyAdd(cmd);
    emit onCommand(cmd);

    QProcess process;
#ifdef WIN32
    process.start( "cmd", QStringList() << "/C" << cmd );
#else
    process.start( cmd );
#endif
    if ( !process.waitForStarted() || !process.waitForFinished() )
    {
        return;
    }

    QString str = QString::fromLocal8Bit( process.readAllStandardOutput() );
    output( str );
    */
}

void Console::output(QString s)
{
    textCursor().insertBlock();
    QTextCharFormat format;
    format.setForeground(Qt::white);
    format.setFont( QFont( "Lucida Console", 8 ) );
    textCursor().setBlockCharFormat(format);
    textCursor().insertText(s);
//    insertPrompt();
    isLocked = false;
}

void Console::insertPrompt(bool insertNewBlock)
{
    if(insertNewBlock)
	textCursor().insertBlock();
    QTextCharFormat format;
    format.setForeground(Qt::green);
    textCursor().setBlockCharFormat(format);
    textCursor().insertText(prompt);
    scrollDown();
}

void Console::scrollDown()
{
    QScrollBar *vbar = verticalScrollBar();
    vbar->setValue(vbar->maximum());
}

void Console::historyAdd(QString cmd)
{
    history->append(cmd);
    historyPos = history->length();
}

void Console::historyBack()
{
    if(!historyPos)
	return;
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(prompt + history->at(historyPos-1));
    setTextCursor(cursor);
    historyPos--;
}

void Console::historyForward()
{
    if(historyPos == history->length())
	return;
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    if(historyPos == history->length() - 1)
	cursor.insertText(prompt);
    else
	cursor.insertText(prompt + history->at(historyPos + 1));
    setTextCursor(cursor);
    historyPos++;
}

void Console::RunProcess()
{
    std::string envStr;
    //for (auto& p : m_params)
    //{
    //    if (p.name == "environment")
    //    {
    //        envStr = p.value.toStdString();
    //        break;
    //    }
    //}
    m_process.Run( envStr );
}

void Console::RunCommand( const std::string& rCmd )
{
    std::string exeStr;
    //for (auto& p : m_params)
    //{
    //    if (p.name == "execution-file")
    //    {
    //        exeStr = p.value.toStdString();
    //        break;
    //    }
    //}
    exeStr += " " + rCmd;
    LOG_INFO << "Run command: " << exeStr;
    m_process.PutCommand( exeStr );
}

void Console::OnOutput( QString str )
{
    output( str );
}

