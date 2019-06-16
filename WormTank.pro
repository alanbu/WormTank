TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    wormtank.cpp \
    worm.cpp \
    brain.cpp \
    randombrain.cpp \
    simplebrain.cpp \
    braincell.cpp \
    iobrain.cpp \
    simplewithnexttobrain.cpp \
    wormtankmodelview.cpp \
    startwindow.cpp \
    simplewithnexttofood.cpp \
    foodrangebrain.cpp \
    foodandwormrangebrain.cpp \
    mutantbrain.cpp \
    foodandwormcountbrain.cpp \
    foodcountbrain.cpp \
    foodwithnexttomemorybrain.cpp \
    league.cpp \
    leaguetable.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    wormtank.h \
    worm.h \
    brain.h \
    randombrain.h \
    simplebrain.h \
    braincell.h \
    iobrain.h \
    simplewithnexttobrain.h \
    direction.h \
    wormtankmodelview.h \
    startwindow.h \
    simplewithnexttofood.h \
    foodrangebrain.h \
    foodandwormrangebrain.h \
    mutantbrain.h \
    random.h \
    foodandwormcountbrain.h \
    foodcountbrain.h \
    wormcount.h \
    foodwithnexttomemorybrain.h \
    league.h \
    leaguetable.h \
    leagueline.h \
    match.h

CONFIG += c++11

DISTFILES += \
    docs/index.html
