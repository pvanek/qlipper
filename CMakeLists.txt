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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)

project(qlipper)


set(QLIPPER_VERSION 2.0.1)


# Find required libraries
find_package(Qt4 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})


set(EXE_NAME "qlipper")
if (APPLE)
    set(EXE_NAME "QLipper")
endif (APPLE)


add_definitions(-DIS_CMAKE)
add_definitions(-DQLIPPER_VERSION="${QLIPPER_VERSION}")


# source code and compilation
set (QLIPPER_SOURCES 
    qkeysequencewidget/qkeysequencewidget.cpp
    qmenuview/qmenuview.cpp
    src/main.cpp
    src/qlipperitem.cpp
    src/qlippermenuview.cpp
    src/qlippermodel.cpp
    src/qlipperpreferences.cpp
    src/qlipperpreferencesdialog.cpp
    src/qlippersystray.cpp
)
set (HAVE_QXT 1)
if (APPLE)
    set (QLIPPER_SOURCES ${QLIPPER_SOURCES} macosx/qlipper.icns)
    set (QLIPPER_SOURCES ${QLIPPER_SOURCES} qxt/qxtglobalshortcut_mac.cpp)
    message(STATUS "Global shortcut for MAC")

   FIND_LIBRARY(CARBON_LIBRARY Carbon REQURED)
   MESSAGE(STATUS "CARBON_LIBRARY: ${CARBON_LIBRARY}")
elseif (WIN32)
    set (QLIPPER_SOURCES ${QLIPPER_SOURCES} qxt/qxtglobalshortcut_win.cpp)
    message(STATUS "Global shortcut for WIN")
elseif (UNIX)
    set (QLIPPER_SOURCES ${QLIPPER_SOURCES} qxt/qxtglobalshortcut_x11.cpp)
    message(STATUS "Global shortcut for X11")
else ()
    set (HAVE_QXT 0)
    message (WARNING "Support for global keyboard shortcut is disabled")
endif ()

if (HAVE_QXT)
    set (QLIPPER_SOURCES ${QLIPPER_SOURCES}
            qxt/qxtglobal.cpp
            qxt/qxtglobalshortcut.cpp
        )
endif (HAVE_QXT)


set (QLIPPER_HEADERS
    qkeysequencewidget/qkeysequencewidget.h
    qkeysequencewidget/qkeysequencewidget_p.h
    qmenuview/qmenuview.h
    qmenuview/qmenuview_p.h
    src/config.h
    src/qlipperitem.h
    src/qlippermenuview.h
    src/qlippermodel.h
    src/qlipperpreferences.h
    src/qlipperpreferencesdialog.h
    src/qlippersystray.h
    src/qlippertypes.h
)

if (HAVE_QXT)
    set (QLIPPER_HEADERS ${QLIPPER_HEADERS}
            qxt/qxtglobal.h
            qxt/qxtglobalshortcut.h
            qxt/qxtglobalshortcut_p.h
        )
else (HAVE_QXT)
    add_definitions(-DNO_QXT)
endif (HAVE_QXT)

set (QLIPPER_FORMS
    src/qlipperpreferencesdialog.ui
)

set (QLIPPER_RESOURCES
    src/qlipper.qrc
)

set (QLIPPER_TRANSLATIONS
#    ts/qlipper.cs.ts
#    ts/qlipper.sr.ts
)

QT4_WRAP_UI( QLIPPER_UI ${QLIPPER_FORMS} )
QT4_WRAP_CPP( QLIPPER_MOC ${QLIPPER_HEADERS} )
QT4_ADD_RESOURCES( QLIPPER_RCC ${QLIPPER_RESOURCES} )
QT4_ADD_TRANSLATION( QLIPPER_QM ${QLIPPER_TRANSLATIONS} )

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/qkeysequencewidget
    ${CMAKE_CURRENT_SOURCE_DIR}/qxt
    ${CMAKE_CURRENT_SOURCE_DIR}/qmenuview
)

add_executable( ${EXE_NAME} WIN32 MACOSX_BUNDLE 
    ${QLIPPER_SOURCES}
    ${QLIPPER_UI}
    ${QLIPPER_MOC}
    ${QLIPPER_RCC}
    ${QLIPPER_QM}
)

target_link_libraries( ${EXE_NAME}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTMAIN_LIBRARY}
)
if (APPLE)
    target_link_libraries(${EXE_NAME} ${CARBON_LIBRARY})
endif (APPLE)


# installation
if (APPLE)
    # mac's bundle install
    SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/macosx/Info.plist.in")
   	SET(MACOSX_BUNDLE_ICON_FILE qlipper.icns)
   	SET(MACOSX_BUNDLE_INFO_STRING "${EXE_NAME} ${QLIPPER_VERSION}")
    SET(MACOSX_BUNDLE_GUI_IDENTIFIER "com.googlecode.rss-guard")
    SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${QLIPPER_VERSION}")
    SET(MACOSX_BUNDLE_BUNDLE_NAME "${EXE_NAME}")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${QLIPPER_VERSION}")
    SET(MACOSX_BUNDLE_BUNDLE_VERSION "${QLIPPER_VERSION}")
    SET(MACOSX_BUNDLE_COPYRIGHT "(c) Martin Rotter")
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
    install(FILES src/icons/qlipper.png DESTINATION share/pixmaps)
	install(FILES ${QLIPPER_QM} DESTINATION share/qlipper/translations)
endif ()



# make dist custom target
SET(CPACK_PACKAGE_NAME "qlipper")
SET(CPACK_PACKAGE_VERSION ${QLIPPER_VERSION})
SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
SET(CPACK_IGNORE_FILES "/CVS/;/\\\\.svn/;/\\\\.git/;\\\\.swp$;\\\\.#;/#;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;\\\\.qm$;/build/;\\\\.diff$;.DS_Store'")
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



