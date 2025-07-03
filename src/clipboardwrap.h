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

#ifndef CLIPBOARDWRAP_H
#define CLIPBOARDWRAP_H

#include <ksystemclipboard.h>
#include <QScopedPointer>

class QTimer;

class ClipboardWrap : public QObject
{
    Q_OBJECT

    class Creator;
    friend class Creator;

public:
    static ClipboardWrap *Instance();
    ~ClipboardWrap();

    const QMimeData * mimeData(QClipboard::Mode mode = QClipboard::Clipboard);
    void setMimeData(QMimeData * src, QClipboard::Mode mode = QClipboard::Clipboard);

signals:
    void changed(QClipboard::Mode mode);

private slots:
    void onChanged(QClipboard::Mode mode);
    void emitChanged();

private:
    ClipboardWrap();

    static QScopedPointer<ClipboardWrap> m_instance;

    KSystemClipboard * m_clip;
    bool m_shouldEmit;
    QClipboard::Mode m_change;
    QScopedPointer<QTimer> m_timer;
};

#endif // CLIPBOARDWRAP_H
