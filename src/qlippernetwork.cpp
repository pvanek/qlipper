/*
Qlipper - clipboard history manager
Copyright (C) 2012 Petr Vanek <petr@scribus.info>

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

#include "qlippernetwork.h"
#include "qlipperitem.h"
#include "qlipperpreferences.h"
#include <QtDebug>
#ifdef ENABLE_NETWORK
#include <QtNetwork/QHostInfo>
#endif


QlipperNetwork::QlipperNetwork(QObject *parent)
    : QObject(parent)
{
    setObjectName("qlipperNetwork");

#ifdef ENABLE_NETWORK
    QString hostname(QHostInfo::localHostName());
    if (hostname.isEmpty())
        hostname = "unknown";

    QString uname;
#ifdef Q_OS_UNIX
    // Mac, linux, and the other unices.
    uname = qgetenv("USER");
#endif
#ifdef Q_WS_WIN
        // Windows.
    uname = qgetenv("USERNAME");
#endif
    if (uname.isEmpty())
        uname = "unknown";

    m_id = uname + "@" + hostname;

    m_socket = new QUdpSocket(this);
    bool r = m_socket->bind(QlipperPreferences::Instance()->networkPort(), QUdpSocket::ShareAddress);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readData()));
    if (!r)
        qDebug() << "socket bound:" << r << m_socket->error() << m_socket->errorString();
#endif
}

void QlipperNetwork::sendData(const ClipboardContent &value)
{
#ifdef ENABLE_NETWORK
    if (!QlipperPreferences::Instance()->networkSend())
        return;

    QByteArray serialized;
    QDataStream ds(&serialized, QIODevice::WriteOnly);

    ds << m_id << value;

    qDebug() << "SENDING:" << serialized;
    QByteArray data = qCompress(serialized);
    int r = m_socket->writeDatagram(data.data(), data.size(), QHostAddress::Broadcast, QlipperPreferences::Instance()->networkPort());
    if (r == -1)
        qDebug() << "error to send" << m_socket->error() << m_socket->errorString();
#endif
}

void QlipperNetwork::readData()
{
#ifdef ENABLE_NETWORK
    if (!QlipperPreferences::Instance()->networkReceive())
        return;

    QString remoteId;

    while (m_socket->hasPendingDatagrams())
    {
        QByteArray data;
        data.resize(m_socket->pendingDatagramSize());
        m_socket->readDatagram(data.data(), data.size());
        QByteArray value = qUncompress(data);
        QDataStream ds(value);
        ds >> remoteId;
        // ignore own data
        if (m_id == remoteId)
             continue;

        ClipboardContent c;
        ds >> c;
        qDebug() << "RECEIVED:" << c;
        QlipperItem item(QClipboard::Clipboard, QlipperItem::PlainText, c);
        item.toClipboard();
    }
#endif
}

