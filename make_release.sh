#!/bin/bash

VERSION=`grep "QLIPPER_VERSION =" qlipper2.pro`
VERSION=${VERSION/QLIPPER_VERSION =/""}
VERSION=${VERSION//" "/""}

echo "Tarball for qlipper ${VERSION}"

DIR=qlipper-${VERSION}

if [ -d "${DIR}" ]
then
    echo "Directory ${DIR} already exists. Exiting."
    exit 1
fi

mkdir ${DIR}

cp -r COPYING README macosx qkeysequencewidget qlipper.desktop qlipper2.pro qmenuview qxt src ts ${DIR}
tar -cjf ${DIR}.tar.bz2 ${DIR}
