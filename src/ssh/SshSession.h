/*
* Copyright (C) 2019 Dmitriy Shadrin
* All rights reserved.
*/

#pragma once

#include "SshTypes.h"
#include <QObject>
#include <atomic>

//////////////////////////////////////////////////////////////////////////
namespace ssh2
{
    // Session operations
    class CSession : public QObject
    {
        Q_OBJECT
    public:
        explicit class CSession( QObject* parent = nullptr );
        ~CSession() noexcept;

    private:
        void AddRef() noexcept;
        void DecRef() noexcept;

    private:
        static trds::mutex s_mtx;
        static uint32_t s_counter;
    };
}
