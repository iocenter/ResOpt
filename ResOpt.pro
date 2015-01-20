#-------------------------------------------------
#
# Project created by QtCreator 2012-04-20T03:56:34
#
#-------------------------------------------------

QT       += core
QT       += gui
QT       += svg
QT       += printsupport
QT       += widgets

TARGET = ResOpt
CONFIG   += console
CONFIG   += app_bundle
#CONFIG   += static
#CONFIG += x86

include(resopt_libs.pri)

#QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

QMAKE_CXXFLAGS_WARN_ON=-w
QMAKE_CFLAGS_WARN_ON=-w

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
    binaryvariable.cpp \
    productionwell.cpp \
    injectionwell.cpp \
    pipeconnection.cpp \
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
    userconstraint.cpp \
    vlptable.cpp \
    vlpsimulator.cpp \
    dptable.cpp \
    pipereader.cpp \
    dptablecalculator.cpp \
    separator.cpp \
    mrstbatchsimulator.cpp \
    pressurebooster.cpp \
    gui/mainwindow.cpp \
    gui/modelscene.cpp \
    gui/connector.cpp \
    gui/modelitemseparator.cpp \
    gui/modelitemprodwell.cpp \
    gui/modeliteminjwell.cpp \
    gui/modelitem.cpp \
    gui/modelitemmidpipe.cpp \
    gui/modelitemendpipe.cpp \
    gui/modelitempressurebooster.cpp \
    gui/inspectorseparator.cpp \
    gui/modelitemcapacity.cpp \
    gui/inspectorvariable.cpp \
    gui/console.cpp \
    gui/plot.cpp \
    gui/qcustomplot.cpp \
    gui/plotstreams.cpp \
    gui/inspectoroptimizer.cpp \
    gui/inspectorprodwell.cpp \
    gui/inspectorwellcontrol.cpp \
    gui/inspectorconstraint.cpp \
    adjoint.cpp \
    adjointscoupledmodel.cpp \
    adjointcollection.cpp \
    derivative.cpp \
    gui/inspectorinjwell.cpp \
    gui/inspectorpressurebooster.cpp \
    gui/inspectorcapacity.cpp \
    gui/inspectorendpipe.cpp \
    gui/inspectorvariableinstall.cpp \
    wellconnectionvariable.cpp \
    gui/inspectorwellconnectionvariable.cpp \
    opt/minlpevaluator.cpp \
    opt/bonmininterface.cpp \
    opt/bonmininterfacegradients.cpp \
    opt/lshoptimizer.cpp \
    opt/lshnomadevaluator.cpp \
    opt/lshipoptinterface.cpp \
    opt/ipoptoptimizer.cpp \
    opt/ipoptinterface.cpp \
    opt/evolutionarystrategyoptimizer.cpp \
    opt/eroptoptimizer.cpp \
    opt/bonminoptimizer.cpp \
    opt/runonceoptimizer.cpp \
    opt/optimizer.cpp \
    opt/nomadoptimizer.cpp \
    opt/nomadipoptoptimizer.cpp \
    opt/nomadipoptevaluator.cpp \
    opt/nomadevaluator.cpp \
    opt/minlpipoptinterface.cpp \
    par/masterrunner.cpp \
    par/masteroptimizer.cpp \
    gui/inspectorgaslift.cpp \
    wellpath.cpp \
    gui/inspectorwellpath.cpp \
    gui/inspectorheelvariable.cpp \
    gui/inspectorconstant.cpp \
    gui/modelitemreservoir.cpp \
    gui/inspectorreservoir.cpp \
    logger.cpp

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
    binaryvariable.h \
    productionwell.h \
    injectionwell.h \
    pipeconnection.h \
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
    userconstraint.h \
    vlpsimulator.h \
    vlptable.h \
    dptable.h \
    pipereader.h \
    dptablecalculator.h \
    separator.h \
    mrstbatchsimulator.h \
    pressurebooster.h \
    gui/mainwindow.h \
    gui/modelscene.h \
    gui/connector.h \
    gui/modelitemseparator.h \
    gui/modelitemprodwell.h \
    gui/modeliteminjwell.h \
    gui/modelitem.h \
    gui/modelitemmidpipe.h \
    gui/modelitemendpipe.h \
    gui/modelitempressurebooster.h \
    gui/inspectorseparator.h \
    gui/modelitemcapacity.h \
    gui/inspectorvariable.h \
    gui/console.h \
    gui/plot.h \
    gui/qcustomplot.h \
    gui/plotstreams.h \
    gui/inspectoroptimizer.h \
    gui/inspectorprodwell.h \
    gui/inspectorwellcontrol.h \
    gui/inspectorconstraint.h \
    adjoint.h \
    adjointscoupledmodel.h \
    adjointcollection.h \
    derivative.h \
    gui/inspectorinjwell.h \
    gui/inspectorpressurebooster.h \
    gui/inspectorcapacity.h \
    gui/inspectorendpipe.h \
    gui/inspectorvariableinstall.h \
    wellconnectionvariable.h \
    gui/inspectorwellconnectionvariable.h \
    opt/minlpevaluator.h \
    opt/bonmininterface.h \
    opt/bonmininterfacegradients.h \
    opt/lshoptimizer.h \
    opt/lshnomadevaluator.h \
    opt/lshipoptinterface.h \
    opt/ipoptoptimizer.h \
    opt/ipoptinterface.h \
    opt/evolutionarystrategyoptimizer.h \
    opt/eroptoptimizer.h \
    opt/bonminoptimizer.h \
    opt/runonceoptimizer.h \
    opt/optimizer.h \
    opt/nomadoptimizer.h \
    opt/nomadipoptoptimizer.h \
    opt/nomadipoptevaluator.h \
    opt/nomadevaluator.h \
    opt/minlpipoptinterface.h \
    par/masterrunner.h \
    par/masteroptimizer.h \
    gui/inspectorgaslift.h \
    wellpath.h \
    gui/inspectorwellpath.h \
    gui/inspectorheelvariable.h \
    gui/inspectorconstant.h \
    gui/modelitemreservoir.h \
    gui/inspectorreservoir.h \
    logger.h

RESOURCES += \
    gui/images.qrc

OTHER_FILES += \
    resopt_libs.pri
