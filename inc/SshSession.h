/*
* Copyright (C) 2019 Dmitriy Shadrin
* All rights reserved.
*/

#pragma once

#include "SshTypes.h"
#include <QObject>
// #include <atomic>

//////////////////////////////////////////////////////////////////////////

// Session operations
class CSession : public QObject
{
    Q_OBJECT
public:
    explicit CSession( QObject* parent = nullptr );
    ~CSession() noexcept;

private:
    void AddRef() noexcept;
    void DecRef() noexcept;

private:
    static tth::mutex s_mtx;
    static uint32_t s_counter;
};
