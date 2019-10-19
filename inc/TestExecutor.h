#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TestExecutor.h"

class TestExecutor : public QMainWindow
{
    Q_OBJECT

public:
    TestExecutor(QWidget *parent = Q_NULLPTR);

private:
    Ui::TestExecutorClass ui;
};
