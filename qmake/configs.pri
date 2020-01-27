include(version.pri)

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-
CONFIG(release){
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3
}

DEFINES += PROJ_VERSION=$$VERSION

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
contains(QT_ARCH, x86_64){
    LibFolderPattern     = $$PREFIX/lib64
} else {
    LibFolderPattern     = $$PREFIX/lib
}
LibIncludeFolderPattern = ./include
BinFolderPattern        = ./bin
BuildFolderPattern      = ./build
TestBinFolder           = ./test
UnitTestBinFolder       = ./unitTest
ConfigFolderPattern     = ./conf

BaseLibraryFolder        = $$PRJDIR/out/$$LibFolderPattern
BaseLibraryIncludeFolder = $$PRJDIR/out/$$LibIncludeFolderPattern
BaseBinFolder            = $$PRJDIR/out/$$BinFolderPattern
BaseTestBinFolder        = $$PRJDIR/out/$$TestBinFolder
BaseUnitTestBinFolder    = $$PRJDIR/out/$$UnitTestBinFolder
BaseBuildFolder          = $$PRJDIR/out/$$BuildFolderPattern/$$ProjectName
BaseConfigFolder         = $$PRJDIR/out/$$ConfigFolderPattern

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
INCLUDEPATH += $$PRJDIR \
               $$PRJDIR/src \
               $$PRJDIR/libsrc \
               $$BaseLibraryIncludeFolder \
               $$PREFIX/include \
               $(HOME)/local/include \
               $$DependencyIncludePaths/

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
DependencyLibPaths      +=   $$BaseLibraryFolder \
                             $$PRJDIR/out/lib64 \
                             $$PRJDIR/out/lib \
                             $$PREFIX/lib64 \
                             $$PREFIX/lib \
                             $(HOME)/local/lib \
                             $(HOME)/local/lib64 \

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-
win32: DEFINES += _WINDOWS NOMINMAX
FullDependencySearchPaths = $$DependencyLibPaths
unix:
  FullDependencySearchPaths+=  /usr/lib \
                               /usr/lib64 \
                               /usr/local/lib \
                               /usr/local/lib64 \
                               /lib/x86_64-linux-gnu


QMAKE_LIBDIR +=  $$FullDependencySearchPaths

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
defineTest(addSubdirs) {
    for(subdirs, 1) {
        entries = $$files($$subdirs)
        for(entry, entries) {
            name = $$replace(entry, [/\\\\], _)
            SUBDIRS += $$name
            eval ($${name}.subdir = $$entry)
            for(dep, 2):eval ($${name}.depends += $$replace(dep, [/\\\\], _))
            export ($${name}.subdir)
            export ($${name}.depends)
        }
    }
    export (SUBDIRS)
}
#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-
HEADERS+= $$VERSIONINGHEADER


