#-------------------------------------------------
#
# Project created by QtCreator 2012-04-20T03:56:34
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ResOpt
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += static


include(resopt_libs.pri)

QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

TEMPLATE = app



SOURCES += main.cpp \
    well.cpp \
    variable.cpp \
    realvariable.cpp \
    intvariable.cpp \
    wellcontrol.cpp \
    reservoir.cpp \
    model.cpp \
    pipe.cpp \
    stream.cpp \
    constraint.cpp \
    wellconnection.cpp \
    objective.cpp \
    npvobjective.cpp \
    cumoilobjective.cpp \
    cumgasobjective.cpp \
    endpipe.cpp \
    midpipe.cpp \
    pressuredropcalculator.cpp \
    beggsbrillcalculator.cpp \
    reservoirsimulator.cpp \
    gprssimulator.cpp \
    runner.cpp \
    modelreader.cpp \
    optimizer.cpp \
    bonminoptimizer.cpp \
    bonmininterface.cpp \
    binaryvariable.cpp \
    productionwell.cpp \
    injectionwell.cpp \
    pipeconnection.cpp \
    runonceoptimizer.cpp \
    launcher.cpp \
    case.cpp \
    casequeue.cpp \
    capacity.cpp \
    cost.cpp \
    coupledmodel.cpp \
    decoupledmodel.cpp \
    inputratevariable.cpp \
    materialbalanceconstraint.cpp \
    component.cpp \
    evolutionarystrategyoptimizer.cpp \
    userconstraint.cpp \
    vlptable.cpp \
    vlpsimulator.cpp \
    dptable.cpp \
    pipereader.cpp \
    dptablecalculator.cpp \
    nomadoptimizer.cpp \
    nomadevaluator.cpp \
    separator.cpp \
    ipoptinterface.cpp \
    ipoptoptimizer.cpp \
    meoptimizer.cpp

HEADERS += \
    well.h \
    variable.h \
    realvariable.h \
    intvariable.h \
    wellcontrol.h \
    reservoir.h \
    model.h \
    pipe.h \
    stream.h \
    constraint.h \
    wellconnection.h \
    objective.h \
    npvobjective.h \
    cumoilobjective.h \
    cumgasobjective.h \
    endpipe.h \
    midpipe.h \
    pressuredropcalculator.h \
    beggsbrillcalculator.h \
    reservoirsimulator.h \
    gprssimulator.h \
    runner.h \
    modelreader.h \
    optimizer.h \
    bonminoptimizer.h \
    bonmininterface.h \
    binaryvariable.h \
    productionwell.h \
    injectionwell.h \
    pipeconnection.h \
    runonceoptimizer.h \
    launcher.h \
    case.h \
    casequeue.h \
    capacity.h \
    cost.h \
    coupledmodel.h \
    decoupledmodel.h \
    inputratevariable.h \
    materialbalanceconstraint.h \
    component.h \
    evolutionarystrategyoptimizer.h \
    userconstraint.h \
    vlpsimulator.h \
    vlptable.h \
    dptable.h \
    pipereader.h \
    dptablecalculator.h \
    nomadoptimizer.h \
    nomadevaluator.h \
    separator.h \
    ipoptinterface.h \
    ipoptoptimizer.h \
    meoptimizer.h
