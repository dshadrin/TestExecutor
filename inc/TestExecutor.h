#pragma once

#include <QMainWindow>
#include "ui_TestExecutor.h"

class TestExecutor : public QMainWindow
{
    Q_OBJECT

public:
    TestExecutor(QWidget *parent = Q_NULLPTR);

protected:
    void closeEvent( QCloseEvent* event ) override;

private:
    void writeSettings();
    void readSettings();

private:
    Ui::TestExecutorClass ui;
};
