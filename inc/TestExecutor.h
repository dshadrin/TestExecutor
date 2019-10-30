#pragma once

#include "Config.h"
#include <QMainWindow>
#include "ui_TestExecutor.h"

class TestExecutor : public QMainWindow
{
    Q_OBJECT

public:
    TestExecutor(QWidget *parent = Q_NULLPTR);

    CConfig& GetConfig() { return m_config; }

protected:
    void closeEvent( QCloseEvent* event ) override;

private:
    void writeSettings();
    void readSettings();

private Q_SLOTS:
    void OptionsDialog();

private:
    Ui::TestExecutorClass ui;
    CConfig m_config;
};
