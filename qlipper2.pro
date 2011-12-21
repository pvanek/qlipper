TARGET = qlipper2
TEMPLATE = app
CONFIG += debug
OBJECTS_DIR += build
MOC_DIR += build
UI_DIR += build

INCLUDEPATH += qkeysequencewidget \
               qmenuview \
               qxt


SOURCES += \
    main.cpp \
    qkeysequencewidget/qkeysequencewidget.cpp \
    qmenuview/qmenuview.cpp \
    qlippermodel.cpp \
    qlippersystray.cpp \
    qlipperitem.cpp \
    qlippermenuview.cpp \
    qlipperpreferences.cpp

HEADERS += \
    qkeysequencewidget/qkeysequencewidget.h \
    qkeysequencewidget/qkeysequencewidget_p.h \
    qxt/qxtglobalshortcut.h \
    qxt/qxtglobalshortcut_p.h \
    qxt/qxtglobal.h \
    qmenuview/qmenuview.h \
    qmenuview/qmenuview_p.h \
    qlippermodel.h \
    qlippersystray.h \
    qlipperitem.h \
    qlippermenuview.h \
    config.h \
    qlipperpreferences.h

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
    qlipper.qrc














