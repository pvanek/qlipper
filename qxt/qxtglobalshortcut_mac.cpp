/****************************************************************************
 **
 ** Copyright (C) Qxt Foundation. Some rights reserved.
 **
 ** This file is part of the QxtGui module of the Qxt library.
 **
 ** This library is free software; you can redistribute it and/or modify it
 ** under the terms of the Common Public License, version 1.0, as published
 ** by IBM, and/or under the terms of the GNU Lesser General Public License,
 ** version 2.1, as published by the Free Software Foundation.
 **
 ** This file is provided "AS IS", without WARRANTIES OR CONDITIONS OF ANY
 ** KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT LIMITATION, ANY
 ** WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY OR
 ** FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** You should have received a copy of the CPL and the LGPL along with this
 ** file. See the LICENSE file and the cpl1.0.txt/lgpl-2.1.txt files
 ** included with the source distribution for more information.
 ** If you did not receive a copy of the licenses, contact the Qxt Foundation.
 **
 ** <http://libqxt.org>  <foundation@libqxt.org>
 **
 ****************************************************************************/
#include <Carbon/Carbon.h>
#include "qxtglobalshortcut_p.h"
#include <QMap>
#include <QHash>
#include <QtDebug>
#include <QApplication>

typedef QPair<uint, uint> Identifier;
static QMap<quint32, EventHotKeyRef> keyRefs;
static QHash<Identifier, quint32> keyIDs;
static quint32 hotKeySerial = 0;
static bool qxt_mac_handler_installed = false;

OSStatus qxt_mac_handle_hot_key(EventHandlerCallRef nextHandler, EventRef event, void* data)
{
    Q_UNUSED(nextHandler);
    Q_UNUSED(data);
    if (GetEventClass(event) == kEventClassKeyboard && GetEventKind(event) == kEventHotKeyPressed)
    {
        EventHotKeyID keyID;
        GetEventParameter(event, kEventParamDirectObject, typeEventHotKeyID, NULL, sizeof(keyID), NULL, &keyID);
        Identifier id = keyIDs.key(keyID.id);
        QxtGlobalShortcutPrivate::activateShortcut(id.second, id.first);
    }
    return noErr;
}

quint32 QxtGlobalShortcutPrivate::nativeModifiers(Qt::KeyboardModifiers modifiers)
{
    quint32 native = 0;
    if (modifiers & Qt::ShiftModifier)
        native |= shiftKeyBit;
    if (modifiers & Qt::ControlModifier)
        native |= cmdKey;
    if (modifiers & Qt::AltModifier)
        native |= optionKey;
    if (modifiers & Qt::MetaModifier)
        native |= controlKey;
    if (modifiers & Qt::KeypadModifier)
        native |= kEventKeyModifierNumLockMask;
    return native;
}

quint32 QxtGlobalShortcutPrivate::nativeKeycode(Qt::Key key)
{
	quint32 INVALID_KEYCODE = 0xff;

	switch (key)
	{
		case Qt::Key_A:
			return kVK_ANSI_A;
		case Qt::Key_S:
			return kVK_ANSI_S;
		case Qt::Key_D:
			return kVK_ANSI_D;
		case Qt::Key_F:
			return kVK_ANSI_F;
		case Qt::Key_H:
			return kVK_ANSI_H;
		case Qt::Key_G:
			return kVK_ANSI_G;
		case Qt::Key_Z:
			return kVK_ANSI_Z;
		case Qt::Key_X:
			return kVK_ANSI_X;
		case Qt::Key_C:
			return kVK_ANSI_C;
		case Qt::Key_V:
			return kVK_ANSI_V;
		case Qt::Key_B:
			return kVK_ANSI_B;
		case Qt::Key_Q:
			return kVK_ANSI_Q;
		case Qt::Key_W:
			return kVK_ANSI_W;
		case Qt::Key_E:
			return kVK_ANSI_E;
		case Qt::Key_R:
			return kVK_ANSI_R;
		case Qt::Key_Y:
			return kVK_ANSI_Y;
		case Qt::Key_T:
			return kVK_ANSI_T;
		case Qt::Key_1:
			return kVK_ANSI_1;
		case Qt::Key_2:
			return kVK_ANSI_2;
		case Qt::Key_3:
			return kVK_ANSI_3;
		case Qt::Key_4:
			return kVK_ANSI_4;
		case Qt::Key_6:
			return kVK_ANSI_6;
		case Qt::Key_5:
			return kVK_ANSI_5;
		case Qt::Key_Equal:
			return kVK_ANSI_Equal;
		case Qt::Key_9:
			return kVK_ANSI_9;
		case Qt::Key_7:
			return kVK_ANSI_7;
		case Qt::Key_Minus:
			return kVK_ANSI_Minus;
		case Qt::Key_8:
			return kVK_ANSI_8;
		case Qt::Key_0:
			return kVK_ANSI_0;
		case Qt::Key_BracketRight:
			return kVK_ANSI_RightBracket;
		case Qt::Key_O:
			return kVK_ANSI_O;
		case Qt::Key_U:
			return kVK_ANSI_U;
		case Qt::Key_BracketLeft:
			return kVK_ANSI_LeftBracket;
		case Qt::Key_I:
			return kVK_ANSI_I;
		case Qt::Key_P:
			return kVK_ANSI_P;
		case Qt::Key_L:
			return kVK_ANSI_L;
		case Qt::Key_J:
			return kVK_ANSI_J;
		case Qt::Key_QuoteDbl:
			return kVK_ANSI_Quote;
		case Qt::Key_K:
			return kVK_ANSI_K;
		case Qt::Key_Semicolon:
			return kVK_ANSI_Semicolon;
		case Qt::Key_Backslash:
			return kVK_ANSI_Backslash;
		case Qt::Key_Comma:
			return kVK_ANSI_Comma;
		case Qt::Key_Slash:
			return kVK_ANSI_Slash;
		case Qt::Key_N:
			return kVK_ANSI_N;
		case Qt::Key_M:
			return kVK_ANSI_M;
		case Qt::Key_Period:
			return kVK_ANSI_Period;
		case Qt::Key_Dead_Grave:
			return kVK_ANSI_Grave;
		case Qt::Key_Asterisk:
			return kVK_ANSI_KeypadMultiply;
		case Qt::Key_Plus:
			return kVK_ANSI_KeypadPlus;
		case Qt::Key_Clear:
			return kVK_ANSI_KeypadClear;
		case Qt::Key_Return:
			return kVK_Return;
		case Qt::Key_Enter:
			return kVK_ANSI_KeypadEnter;
		case Qt::Key_Tab:
			return kVK_Tab;
		case Qt::Key_Space:
			return kVK_Space;
		case Qt::Key_Backspace:
			return kVK_Delete;
		case Qt::Key_Escape:
			return kVK_Escape;
		case Qt::Key_Control:
			return kVK_Command;
		case Qt::Key_Shift:
			return kVK_Shift;
		case Qt::Key_CapsLock:
			return kVK_CapsLock;
		case Qt::Key_Option:
			return kVK_Option;
		case Qt::Key_Meta:
			return kVK_Control;
		case Qt::Key_F17:
			return kVK_F17;
		case Qt::Key_VolumeUp:
			return kVK_VolumeUp;
		case Qt::Key_VolumeDown:
			return kVK_VolumeDown;
		case Qt::Key_F18:
			return kVK_F18;
		case Qt::Key_F19:
			return kVK_F19;
		case Qt::Key_F20:
			return kVK_F20;
		case Qt::Key_F5:
			return kVK_F5;
		case Qt::Key_F6:
			return kVK_F6;
		case Qt::Key_F7:
			return kVK_F7;
		case Qt::Key_F3:
			return kVK_F3;
		case Qt::Key_F8:
			return kVK_F8;
		case Qt::Key_F9:
			return kVK_F9;
		case Qt::Key_F11:
			return kVK_F11;
		case Qt::Key_F13:
			return kVK_F13;
		case Qt::Key_F16:
			return kVK_F16;
		case Qt::Key_F14:
			return kVK_F14;
		case Qt::Key_F10:
			return kVK_F10;
		case Qt::Key_F12:
			return kVK_F12;
		case Qt::Key_F15:
			return kVK_F15;
		case Qt::Key_Help:
			return kVK_Help;
		case Qt::Key_Home:
			return kVK_Home;
		case Qt::Key_PageUp:
			return kVK_PageUp;
		case Qt::Key_Delete:
			return kVK_ForwardDelete;
		case Qt::Key_F4:
			return kVK_F4;
		case Qt::Key_End:
			return kVK_End;
		case Qt::Key_F2:
			return kVK_F2;
		case Qt::Key_PageDown:
			return kVK_PageDown;
		case Qt::Key_F1:
			return kVK_F1;
		case Qt::Key_Left:
			return kVK_LeftArrow;
		case Qt::Key_Right:
			return kVK_RightArrow;
		case Qt::Key_Down:
			return kVK_DownArrow;
		case Qt::Key_Up:
			return kVK_UpArrow;
		default:
			return INVALID_KEYCODE;
	}
}

bool QxtGlobalShortcutPrivate::registerShortcut(quint32 nativeKey, quint32 nativeMods)
{
    if (!qxt_mac_handler_installed)
    {
        EventTypeSpec t;
        t.eventClass = kEventClassKeyboard;
        t.eventKind = kEventHotKeyPressed;
        InstallApplicationEventHandler(&qxt_mac_handle_hot_key, 1, &t, NULL, NULL);
    }

    EventHotKeyID keyID;
    keyID.signature = 'cute';
    keyID.id = ++hotKeySerial;

    EventHotKeyRef ref = 0;
    bool rv = !RegisterEventHotKey(nativeKey, nativeMods, keyID, GetApplicationEventTarget(), 0, &ref);
    if (rv)
    {
        keyIDs.insert(Identifier(nativeMods, nativeKey), keyID.id);
        keyRefs.insert(keyID.id, ref);
    }
    return rv;
}

bool QxtGlobalShortcutPrivate::unregisterShortcut(quint32 nativeKey, quint32 nativeMods)
{
    Identifier id(nativeMods, nativeKey);
    if (!keyIDs.contains(id)) return false;

    EventHotKeyRef ref = keyRefs.take(keyIDs[id]);
    keyIDs.remove(id);
    return !UnregisterEventHotKey(ref);
}

