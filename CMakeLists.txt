# CMakeLists.txt
# -------------------------------------------------------
# An initial version of the cmake build for qlipper.
# WORK IN PROGRESS. USE qmake STILL FOR NOW, PLEASE!
# Currently tested on unix only.
# -------------------------------------------------------
# UNIX - usage (for OSX see below)
# 1. cd qlipper/root/dir
# 2. mkdir build
# 3. cd build
# 4. cmake ../qlipper-git -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=debug (or release, by default)
# 5. make
# 6. make install
# -------------------------------------------------------
# WINDOWS - usage
# You have to have MinGW installed  (it works with MinGW). If you use different compiler, then "make" steps will differ.
# You have to have CMake installed.
# I recommend using cmake-gui to generate and configure MakeFiles like this.
# 1. Run cmake-gui and select folder which contains top-level CMakelists.txt file into "Where the is source code" box. This can be also done in command line via simpler cmake.exe.
# 2. Select folder which you want to contain compiled binaries into "Where to build the binaries" box.
# 3. Hit Configure button (make any changes to variables first) and select MinGW Makefiles as generator. Choose "Specify native compilers".
# 4. Now input path to C++ compiler. For example "C:/QtSDK/mingw/bin/g++.exe", then hit "Finish".
# 5. Project will configure now, hit "Configure" button again. Then hit "Generate" button and close cmake-gui.
# 6. Run your command line as ADMINISTRATOR and navigate to your "Where to build the binaries" folder.
# 7. Run "mingw32-make.exe" command. Note that -j3 parameter may speed-up building. Your files gets compiled now.
# 8. Run "mingw32-make.exe install" command. Your files gets copied into directory specified by CMAKE_INSTALL_PREFIX variable.
# 9. You are done.
# 10. Run "make install" to install application to CMAKE_INSTALL_PREFIX (which is se to C:\Program Files\qlipper\ by default - can be changed in step 3).
# -------------------------------------------------------
# MAC OS X
# 1. cd qlipper/root/dir
# 2. mkdir build
# 3. cd build
# 4. cmake ../qlipper-git -DCMAKE_INSTALL_PREFIX=../release -DCMAKE_BUILD_TYPE=debug (or release, by default)
# 5. make
# 6. make install
#    now is the qlipper.app bundle created. But it can run only in current machine.
#    If you need "redistributable" binary call:
# 7. make bundle
#    It will copy all dependecies (libs/resources) into bundle tree
# -------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)

project(qlipper)


set(QLIPPER_VERSION 5.1.2)

option(ENABLE_NETWORK_CLIPBOARD_SHARING "Enables experimental network support for clipboard sharing" OFF)
option(ENABLE_LXQT_AUTOSTART "Enables autostart for LXQt" OFF)
option(USE_SYSTEM_QXT "Use system Qxt Library for global shortcuts; off for Qt6" OFF)


LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Qt6Widgets REQUIRED)
find_package(Qt6LinguistTools REQUIRED)

if (USE_SYSTEM_QXT)
    find_package(Qxt)
    message(STATUS "Use system Qxt Library: " ${USE_SYSTEM_QXT})
    if (USE_SYSTEM_QXT AND NOT QXT_FOUND)
        message(FATAL_ERROR "System Qxt not found")
    endif ()
endif()


set(EXE_NAME "qlipper")
if (APPLE)
    set(EXE_NAME "QLipper")
endif (APPLE)


add_definitions(-DIS_CMAKE)
add_definitions(-DQLIPPER_VERSION="${QLIPPER_VERSION}")
if (ENABLE_NETWORK_CLIPBOARD_SHARING)
    add_definitions(-DENABLE_NETWORK_CLIPBOARD_SHARING)
    find_package(Qt6Network REQUIRED)
endif (ENABLE_NETWORK_CLIPBOARD_SHARING)


# source code and compilation
set (QLIPPER_HEADERS
    qkeysequencewidget/qkeysequencewidget.h
    qkeysequencewidget/qkeysequencewidget_p.h
    qmenuview/qmenuview.h
    qmenuview/qmenuview_p.h
    src/qlippermodel.h
    src/qlipperpreferencesdialog.h
    src/qlippersystray.h
    src/qlippernetwork.h
    src/clipboardwrap.h
)
set (QLIPPER_SOURCES 
    qkeysequencewidget/qkeysequencewidget.cpp
    qmenuview/qmenuview.cpp
    src/main.cpp
    src/qlipperitem.cpp
    src/qlippermodel.cpp
    src/qlipperpreferences.cpp
    src/qlipperpreferencesdialog.cpp
    src/qlippersystray.cpp
    src/qlippernetwork.cpp
    src/clipboardwrap.cpp
)
if (WIN32)
    set(QLIPPER_SOURCES ${QLIPPER_SOURCES} src/icons/qlipper.rc)
endif (WIN32)
if (UNIX)
    list(APPEND QLIPPER_HEADERS src/signalhandler.h)
    list(APPEND QLIPPER_SOURCES src/signalhandler.cpp)
endif (UNIX)

if (NOT QXT_FOUND)

    set (HAVE_QXT 1)
    if (APPLE)
        set (QLIPPER_SOURCES ${QLIPPER_SOURCES} qxt/qxtglobalshortcut_mac.cpp)
    elseif (WIN32)
        set (QLIPPER_SOURCES ${QLIPPER_SOURCES} qxt/qxtglobalshortcut_win.cpp)
    elseif (UNIX)
        set (QLIPPER_SOURCES ${QLIPPER_SOURCES} qxt/qxtglobalshortcut_x11.cpp)
        include_directories(${Qt6Gui_PRIVATE_INCLUDE_DIRS})
    else ()
        set (HAVE_QXT 0)
        add_definitions(-DNO_QXT)
    endif ()

    if (HAVE_QXT)
        set (QLIPPER_SOURCES ${QLIPPER_SOURCES}
            qxt/qxtglobalshortcut.cpp
        )
        set (QLIPPER_HEADERS ${QLIPPER_HEADERS}
            qxt/qxtglobalshortcut.h
        )
    endif (HAVE_QXT)

endif (NOT QXT_FOUND)

if (APPLE)
    set (QLIPPER_SOURCES ${QLIPPER_SOURCES} macosx/qlipper.icns)
    FIND_LIBRARY(CARBON_LIBRARY Carbon REQURED)
    MESSAGE(STATUS "CARBON_LIBRARY: ${CARBON_LIBRARY}")
elseif (UNIX)
    FIND_PACKAGE(X11 REQUIRED)
    MESSAGE(STATUS "X11_X11_LIB: ${X11_X11_LIB}")
endif ()


set (QLIPPER_FORMS
    src/qlipperpreferencesdialog.ui
)

set (QLIPPER_RESOURCES
    src/qlipper.qrc
)

set (QLIPPER_TRANSLATIONS
    ts/qlipper_ca.ts
    ts/qlipper_cs.ts
    ts/qlipper_de.ts
    ts/qlipper_es.ts
    ts/qlipper_ja.ts
    ts/qlipper_lt.ts
    ts/qlipper_pl.ts
    ts/qlipper_sr.ts
    ts/qlipper_sv.ts
    ts/qlipper_it.ts
    ts/qlipper_zh_CN.ts
    ts/qlipper_pt.ts
)

QT6_WRAP_UI( QLIPPER_UI ${QLIPPER_FORMS} )
QT6_WRAP_CPP( QLIPPER_MOC ${QLIPPER_HEADERS} )
QT6_ADD_RESOURCES( QLIPPER_RCC ${QLIPPER_RESOURCES} )
QT6_ADD_TRANSLATION( QLIPPER_QM ${QLIPPER_TRANSLATIONS} )

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/qkeysequencewidget
    ${CMAKE_CURRENT_SOURCE_DIR}/qxt
    ${CMAKE_CURRENT_SOURCE_DIR}/qmenuview
)
if (NOT QXT_FOUND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qxt)
endif (NOT QXT_FOUND)


add_executable( ${EXE_NAME} WIN32 MACOSX_BUNDLE 
    ${QLIPPER_SOURCES}
    ${QLIPPER_UI}
    ${QLIPPER_MOC}
    ${QLIPPER_RCC}
    ${QLIPPER_QM}
)
set_property(TARGET ${EXE_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${EXE_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries( ${EXE_NAME}
    Qt6::Widgets
)

if (ENABLE_NETWORK_CLIPBOARD_SHARING)
    target_link_libraries( ${EXE_NAME}
        Qt6::Network
    )
endif(ENABLE_NETWORK_CLIPBOARD_SHARING)

# single app and optional network features
#target_link_libraries( ${EXE_NAME} ${QT_QTNETWORK_LIBRARY})

if (APPLE)
    target_link_libraries(${EXE_NAME} ${CARBON_LIBRARY})
elseif (UNIX)
    target_link_libraries(${EXE_NAME} ${X11_X11_LIB})
endif ()


if (QXT_FOUND)
    target_link_libraries(${EXE_NAME} ${QXT_CORE_LIB} ${QXT_GUI_LIB})
endif (QXT_FOUND)


# installation
if (APPLE)
    # mac's bundle install
    SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/macosx/Info.plist.in")
    SET(MACOSX_BUNDLE_ICON_FILE qlipper.icns)
    SET(MACOSX_BUNDLE_INFO_STRING "${EXE_NAME} ${QLIPPER_VERSION}")
    SET(MACOSX_BUNDLE_GUI_IDENTIFIER "cz.yarpen.qlipper")
    SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${QLIPPER_VERSION}")
    SET(MACOSX_BUNDLE_BUNDLE_NAME "${EXE_NAME}")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${QLIPPER_VERSION}")
    SET(MACOSX_BUNDLE_BUNDLE_VERSION "${QLIPPER_VERSION}")
    SET(MACOSX_BUNDLE_COPYRIGHT "(c) Petr Vanek")
    SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/macosx/qlipper.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    INSTALL(TARGETS ${EXE_NAME} BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX})
    install(FILES ${QLIPPER_QM} DESTINATION ${CMAKE_INSTALL_PREFIX}/${EXE_NAME}.app/Contents/Resources/translations)

    # create a "transportable" bundle - all libs into the bundle: "make bundle" after make install
    configure_file(macosx/bundle.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/bundle.cmake @ONLY)
    add_custom_target(bundle ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/bundle.cmake)

elseif (WIN32)
    install(TARGETS ${EXE_NAME} RUNTIME DESTINATION ./)
    install(FILES ${QLIPPER_QM} DESTINATION ./translations)
elseif (UNIX)
    install(TARGETS ${EXE_NAME} RUNTIME DESTINATION bin)
    install(FILES qlipper.desktop DESTINATION share/applications)
    install(FILES src/icons/qlipper.png DESTINATION share/icons/hicolor/128x128/apps)
    install(FILES ${QLIPPER_QM} DESTINATION share/qlipper/translations)
    add_definitions(-DTRANSLATION_DIR="${CMAKE_INSTALL_PREFIX}/share/qlipper/translations")
endif ()



# make dist custom target
SET(CPACK_PACKAGE_NAME "qlipper")
SET(CPACK_PACKAGE_VERSION ${QLIPPER_VERSION})
SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
SET(CPACK_IGNORE_FILES "/CVS/;/\\\\.svn/;/\\\\.git/;\\\\.swp$;\\\\.#;/#;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;\\\\.qm$;/build/;/release/;\\\\.diff$;.DS_Store'")
SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})
INCLUDE(CPack)
# simulate autotools' "make dist"
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)


# make lupdate
# it generates new translation files
add_custom_target(lupdate
    ${QT_QMAKE_EXECUTABLE} -project -o ${CMAKE_CURRENT_BINARY_DIR}/qlipper.pro
    COMMAND ${QT_LUPDATE_EXECUTABLE} -noobsolete ${CMAKE_CURRENT_BINARY_DIR}/qlipper.pro
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
# lrelease
add_custom_target(lrelease
    ${QT_QMAKE_EXECUTABLE} -project -o ${CMAKE_CURRENT_BINARY_DIR}/qlipper.pro
    COMMAND ${QT_LRELEASE_EXECUTABLE} -compress ${CMAKE_CURRENT_BINARY_DIR}/qlipper.pro
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# merged from lxqt-common
if (ENABLE_LXQT_AUTOSTART)
    set(LXQTBT_MINIMUM_VERSION "2.0.0")
    find_package(lxqt2-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)
    include(LXQtConfigVars)
    include(LXQtTranslateDesktop)
    add_subdirectory(lxqt-autostart)
endif ()
