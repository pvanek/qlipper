/*
Qlipper - clipboard history manager
Copyright (C) 2015 Palo Kisa <palo.kisa@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "clipboardwrap.h"

#include <QApplication>
#include <QTimer>
#include <QDebug>

QScopedPointer<ClipboardWrap> ClipboardWrap::m_instance(0);

namespace
{
    class WorkGuard
    {
    public:
        WorkGuard(bool & guard)
            : mGuard(guard)
        {
            mGuard = false;
        }
        ~WorkGuard()
        {
            mGuard = true;
        }
    private:
        bool & mGuard;
    };
}

ClipboardWrap * ClipboardWrap::Instance()
{
    //instance can't be created in static initilization time because QApplication object doesn't exist in that time
    if (m_instance.isNull())
        m_instance.reset(new ClipboardWrap);
    return m_instance.data();
}

ClipboardWrap::ClipboardWrap()
    : m_clip(KSystemClipboard::instance())
    , m_shouldEmit(true)
    , m_timer(new QTimer)
{
    connect(m_clip, &KSystemClipboard::changed, this, &ClipboardWrap::onChanged);
    connect(m_timer.data(), &QTimer::timeout, this, &ClipboardWrap::emitChanged);

    //Note: the timer is here as a workaround for signal flood for primary selection
    //      from GTK apps
    m_timer->setInterval(0);
    m_timer->setSingleShot(true);
}

ClipboardWrap::~ClipboardWrap()
{
}

void ClipboardWrap::emitChanged()
{
    emit changed(m_change);
}

void ClipboardWrap::onChanged(QClipboard::Mode mode)
{
    if (m_shouldEmit)
    {
        m_change = mode;
        m_timer->start();
    }
}

const QMimeData * ClipboardWrap::mimeData(QClipboard::Mode mode)
{
    return m_clip->mimeData(mode);
}

void ClipboardWrap::setMimeData(QMimeData * src, QClipboard::Mode mode)
{
    WorkGuard g(m_shouldEmit);
    return m_clip->setMimeData(src, mode);
}
