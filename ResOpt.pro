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

# bonmin libraries:
LIBS += -L/home/aleksaju/bonmin/Bonmin-1.5.3/build/lib -lbonmin -lipopt -lCbc -lCgl -lOsiClp -lOsi -lClp -lCoinUtils

# ipopt libraries (from ipopt_addlibs_cpp.txt):
LIBS += -L/home/aleksaju/bonmin/Bonmin-1.5.3/build/lib -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1 -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../../x86_64-linux-gnu -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../../../lib -L/lib/x86_64-linux-gnu -L/lib/../lib -L/usr/lib/../lib -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../.. -lipopt -ldl -lcoinmumps -lpthread -lcoinlapack -lcoinblas -lgfortran -lm -lgcc_s -lquadmath

# cgl libraries (from cgl_addlibs.txt):
LIBS += -L/home/aleksaju/bonmin/Bonmin-1.5.3/build/lib -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1 -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../../x86_64-linux-gnu -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../../../lib -L/lib/x86_64-linux-gnu -L/lib/../lib -L/usr/lib/../lib -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../.. -lCgl -lOsiClp -lClp -lOsi -lcoinmumps -lpthread -lCoinUtils -lcoinlapack -lcoinblas -lgfortran -lm -lgcc_s -lquadmath

# osi libraries (from osi_addlibs.txt):
LIBS += -L/home/aleksaju/bonmin/Bonmin-1.5.3/build/lib -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1 -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../../x86_64-linux-gnu -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../../../lib -L/lib/x86_64-linux-gnu -L/lib/../lib -L/usr/lib/../lib -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../.. -lOsi -lCoinUtils -lcoinlapack -lcoinblas -lgfortran -lm -lgcc_s -lquadmath

# clp libraries (from clp_addlibs.txt):
LIBS += -L/home/aleksaju/bonmin/Bonmin-1.5.3/build/lib -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1 -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../../x86_64-linux-gnu -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../../../lib -L/lib/x86_64-linux-gnu -L/lib/../lib -L/usr/lib/../lib -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../.. -lClp -lcoinmumps -lpthread -lCoinUtils -lcoinlapack -lcoinblas -lgfortran -lm -lgcc_s -lquadmath

# coinutils libraries (from coinutils_addlibs.txt):
LIBS += -L/home/aleksaju/bonmin/Bonmin-1.5.3/build/lib -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1 -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../../x86_64-linux-gnu -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../../../lib -L/lib/x86_64-linux-gnu -L/lib/../lib -L/usr/lib/../lib -L/usr/lib/gcc/x86_64-linux-gnu/4.6.1/../../.. -lCoinUtils -lcoinlapack -lcoinblas -lgfortran -lm -lgcc_s -lquadmath

# NOMAD library
LIBS += /home/aleksaju/nomad/nomad.3.5.1/lib/nomad.a

# bonmin header files
INCLUDEPATH += /home/aleksaju/bonmin/Bonmin-1.5.3/build/include/coin


#NOMAD header files
INCLUDEPATH += /home/aleksaju/nomad/nomad.3.5.1/src

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
    separator.cpp \
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
    nomadevaluator.cpp

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
    separator.h \
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
    nomadevaluator.h
