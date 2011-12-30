QLIPPER_VERSION = 2.0.0

TARGET = qlipper
TEMPLATE = app
CONFIG += release #debug
OBJECTS_DIR += build
MOC_DIR += build
UI_DIR += build

INCLUDEPATH += qkeysequencewidget \
               qmenuview \
               qxt \
               src

DEFINES += QLIPPER_VERSION=\\\"$${QLIPPER_VERSION}\\\"
DEFINES += QXT_STATIC

SOURCES += \
    src/main.cpp \
    qkeysequencewidget/qkeysequencewidget.cpp \
    qmenuview/qmenuview.cpp \
    src/qlippermodel.cpp \
    src/qlippersystray.cpp \
    src/qlipperitem.cpp \
    src/qlippermenuview.cpp \
    src/qlipperpreferences.cpp \
    src/qlipperpreferencesdialog.cpp

HEADERS += \
    qkeysequencewidget/qkeysequencewidget.h \
    qkeysequencewidget/qkeysequencewidget_p.h \
    qxt/qxtglobalshortcut.h \
    qxt/qxtglobalshortcut_p.h \
    qxt/qxtglobal.h \
    qmenuview/qmenuview.h \
    qmenuview/qmenuview_p.h \
    src/qlippermodel.h \
    src/qlippersystray.h \
    src/qlipperitem.h \
    src/qlippermenuview.h \
    src/config.h \
    src/qlipperpreferences.h \
    src/qlipperpreferencesdialog.h

!os2 {
    SOURCES += qxt/qxtglobal.cpp \
    qxt/qxtglobalshortcut.cpp
}


unix:!macx {
    SOURCES += qxt/qxtglobalshortcut_x11.cpp
    CONFIG += link_pkgconfig
    PKGCONFIG += x11
    LIB += -L/usr/X11R6/lib -lX11
}
macx {
    SOURCES += qxt/qxtglobalshortcut_mac.cpp
    LIBS += -framework Carbon -framework Cocoa
}
win32 {
    SOURCES += qxt/qxtglobalshortcut_win.cpp
}

RESOURCES += \
    src/qlipper.qrc

FORMS += \
    src/qlipperpreferencesdialog.ui

TRANSLATIONS += \
    ts/qlipper.cs.ts


translations_qm.files = ts/*.qm

# Unix installer.
unix:!macx {
    isEmpty(INSTALL_PREFIX):INSTALL_PREFIX = /usr/local

    target.path = $$INSTALL_PREFIX/bin

    desktop.path = $$INSTALL_PREFIX/share/applications
    desktop.files = qlipper.desktop

    icons.path = $$INSTALL_PREFIX/share/pixmaps
    icons.files = icons/qlipper.png

    translations_qm.files = ts/*.qm
    translations_qm.path = $$INSTALL_PREFIX/share/qlipper/translations
    DEFINES += TRANSLATION_DIR=\\\"$$INSTALL_PREFIX/share/qlipper/translations\\\"

    INSTALLS += target desktop icons translations_qm
}

macx {
    TARGET = Qlipper
    ICON = macosx/qlipper.icns
    QMAKE_INFO_PLIST = macosx/Info.plist
}
