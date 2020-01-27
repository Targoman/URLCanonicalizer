#/*******************************************************************************
# * URLCanonicalizer a robust and high speed URL normalizer/canonicalizer       *
# *                                                                             *
# * Copyright 2018 by Targoman Intelligent Processing Co Pjc.<http://tip.co.ir> *
# *                                                                             *
# *                                                                             *
# * URLCanonicalizer is free software: you can redistribute it and/or modify    *
# * it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE as published by *
# * the Free Software Foundation, either version 3 of the License, or           *
# * (at your option) any later version.                                         *
# *                                                                             *
# * URLCanonicalizer is distributed in the hope that it will be useful,         *
# * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
# * GNU LESSER GENERAL PUBLIC LICENSE for more details.                         *
# * You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE    *
# * along with URLCanonicalizer. If not, see <http://www.gnu.org/licenses/>.    *
# *                                                                             *
# *******************************************************************************/
PRJDIR = ".."

# +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
HEADERS +=

# +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
SOURCES += \
    main.cpp \

################################################################################
#                       DO NOT CHANGE ANYTHING BELOW                           #
################################################################################
################################################################################
ConfigFile = $$PRJDIR/qmake/configs.pri
!exists($$ConfigFile){
error("**** Unable to find Configuration file $$ConfigFile ****")
}
include ($$ConfigFile)

CONFIG      += console
QT-=gui

TEMPLATE = app
TARGET = test_$$ProjectName

DESTDIR      = $$BaseTestBinFolder
MOC_DIR      = $$BuildFolderPattern/$$TARGET/moc
OBJECTS_DIR  = $$BuildFolderPattern/$$TARGET/obj
RCC_DIR      = $$BuildFolderPattern/$$TARGET/rcc
UI_DIR       = $$BuildFolderPattern/$$TARGET/ui

INCLUDEPATH += $$BasePath/libsrc \
               $$BasePath/libsrc/lib$$ProjectName

QMAKE_LIBDIR += $$BaseLibraryFolder
LIBS += -l$$ProjectName

DISTFILES += \
