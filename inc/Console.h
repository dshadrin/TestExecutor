#ifndef CONSOLE_H
#define CONSOLE_H

#include <QtGui>
#include <QPlainTextEdit>
#include "AppConfig.h"
#include "ExternalProcess.h"

class Console : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit Console( /*VectorValues params,*/ QWidget *parent = nullptr);
    void output(QString);
    void scrollDown();
protected:
    void keyPressEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void RunProcess();

private:
    void onEnter();
    void insertPrompt(bool insertNewBlock = true);
    void historyAdd(QString);
    void historyBack();
    void historyForward();

public Q_SLOTS:
    void OnOutput( QString str );
    void RunCommand( const std::string& );

Q_SIGNALS:
    void onCommand(QString);

private:
    QString prompt;
    QStringList* history;
//    VectorValues m_params;
    CExternalProcess m_process;
    int historyPos;
    bool isLocked;
    DECLARE_MODULE_TAG;
};

#endif // CONSOLE_H
