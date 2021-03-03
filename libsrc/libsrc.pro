################################################################################
#   QBuildSystem
#
#   Copyright(c) 2021 by Targoman Intelligent Processing <http://tip.co.ir>
#
#   Redistribution and use in source and binary forms are allowed under the
#   terms of BSD License 2.0.
################################################################################
HEADERS += \
    libURLCanonicalizer/URLCanonicalizer.h

SOURCES += \
    libURLCanonicalizer/URLCanonicalizer.cpp \
    libURLCanonicalizer/URLConverters.cpp

OTHER_FILES += \

################################################################################
include($$QBUILD_PATH/templates/libConfigs.pri)



