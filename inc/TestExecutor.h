#pragma once

#include "StdInc.h"
#include "Config.h"
#include <QMainWindow>
#include "ui_TestExecutor.h"
#include <boost/asio/io_context.hpp>

class TestExecutor : public QMainWindow
{
    Q_OBJECT

public:
    TestExecutor(QWidget *parent = Q_NULLPTR);
    ~TestExecutor();

    CConfig* GetConfig() { return m_config; }

protected:
    void closeEvent( QCloseEvent* event ) override;

private:
    void writeSettings();
    void readSettings();
    void ThreadIO();

private Q_SLOTS:
    void OptionsDialog();

private:
    Ui::TestExecutorClass ui;
    CConfig* m_config;
    boost::asio::io_context m_ioCtx;
    trd::thread m_thread;
};
