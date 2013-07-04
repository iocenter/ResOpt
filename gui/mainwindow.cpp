/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2013 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include "mainwindow.h"
#include "modelscene.h"
#include "console.h"
#include "plot.h"
#include "inspectoroptimizer.h"

#include "optimizer.h"
#include "model.h"
#include "case.h"
#include "casequeue.h"
#include "objective.h"
#include "well.h"
#include "pipe.h"
#include "capacity.h"
#include "userconstraint.h"
#include "reservoirsimulator.h"
#include "reservoir.h"

#include <QtWidgets/QAction>
#include <QKeySequence>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>

#include <QDir>


using ResOpt::Case;
using ResOpt::CaseQueue;
using ResOpt::Optimizer;
using ResOpt::Model;
using ResOpt::Objective;
using ResOpt::Well;
using ResOpt::Pipe;
using ResOpt::Capacity;
using ResOpt::UserConstraint;
using ResOpt::ReservoirSimulator;
using ResOpt::Reservoir;

namespace ResOptGui
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    p_runner(0),
    p_obj_inpector(0),
    m_running(false),
    m_paused(false)
{

    // setting up the model scene
    p_model_scene = new ModelScene(this);
    connect(p_model_scene, SIGNAL(sendMsg(QString)), this, SIGNAL(sendMsg(QString)));

    // setting up the model view
    QHBoxLayout *layout = new QHBoxLayout;

    p_model_view = new QGraphicsView(p_model_scene);
    p_model_view->setMinimumSize(900, 550);



    layout->addWidget(p_model_view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    //setCentralWidget(widget);

    // setting up the plot
    p_plot = new Plot(this);


    // setting up the tab widget
    p_tab_widget = new QTabWidget(this);

    p_tab_widget->addTab(widget, "Model");
    p_tab_widget->addTab(p_plot, "Plot");



    setCentralWidget(p_tab_widget);
    setWindowTitle("ResOpt GUI");



    // setting up the console
    p_console = new Console(this);
    p_console->setAllowedAreas(Qt::BottomDockWidgetArea);
    connect(this, SIGNAL(sendMsg(QString)), p_console, SLOT(printMsg(QString)));

    addDockWidget(Qt::BottomDockWidgetArea, p_console);


    // creating the menus
    createMenus();



}

MainWindow::~MainWindow()
{
    if(p_runner != 0) delete p_runner;

    p_runner = 0;
}


//-----------------------------------------------------------------------------------------------
// creates the menu items
//-----------------------------------------------------------------------------------------------
void MainWindow::createMenus()
{
    // ----------- creating file menu ---------------------
    QMenu *p_file_menu = menuBar()->addMenu("File");

    // load model
    QAction *p_act_load =  p_file_menu->addAction("Load Model");
    p_act_load->setShortcut(QKeySequence::Open);
    connect(p_act_load, SIGNAL(triggered()), this, SLOT(loadModel()));

    // save model
    QAction *p_act_savemodel = p_file_menu->addAction("Save Model");
    p_act_savemodel->setShortcut(QKeySequence::Save);
    connect(p_act_savemodel, SIGNAL(triggered()), this, SLOT(saveModelAs()));

    // save plot
    QAction *p_act_saveplot = p_file_menu->addAction("Save Plot");
    connect(p_act_saveplot, SIGNAL(triggered()), this, SLOT(savePlot()));


    // ---------- create optimization menu -------------------
    QMenu *p_opt_menu = menuBar()->addMenu("Optimization");

    // start
    QAction *p_act_start = p_opt_menu->addAction("Start");
    connect(p_act_start, SIGNAL(triggered()), this, SLOT(runModel()));


    // properties
    QAction *p_act_opt_prop = p_opt_menu->addAction("Properties");
    p_act_opt_prop->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    connect(p_act_opt_prop, SIGNAL(triggered()), this, SLOT(openOptimizerInspector()));



    // ---------- creating toolbar -------------------

    setIconSize(QSize(40, 40));

    QToolBar *toolbar = addToolBar("Tools");
    p_act_startbutton = toolbar->addAction(QIcon(":new/images/play"), "Start Optimization");
    p_act_startbutton->setShortcut(Qt::CTRL + Qt::Key_R);

    connect(p_act_startbutton, SIGNAL(triggered()), this, SLOT(onStartButtonTriggered()));

}


//-----------------------------------------------------------------------------------------------
// Select model file, and set up runner
//-----------------------------------------------------------------------------------------------
void MainWindow::loadModel()
{

    // letting the user specify the driver file
    QString fileName = QFileDialog::getOpenFileName(this, "Load Project", "/Users", "ResOpt Driver File (*.dat)");


    // setting up the runner with the specified driver file
    if(p_runner == 0)
    {
        p_runner = new Runner(fileName);

        p_runner->initialize();

        p_model_scene->buildSceneFromModel(p_runner->model());


        // connecting to plot
        connect(p_runner, SIGNAL(newCaseFinished(Case*)), p_plot, SLOT(addCase(Case*)));

        // connecting to finished signal
        connect(p_runner, SIGNAL(optimizationFinished()), this, SLOT(onOptimizationFinished()));
    }

}

//-----------------------------------------------------------------------------------------------
// Run the optimization
//-----------------------------------------------------------------------------------------------
void MainWindow::runModel()
{

    // checking that a model is loaded
    if(p_runner == 0) emit sendMsg("No model is loaded, unable to run!");
    else
    {
        setRunningState();

        emit sendMsg("Updating model...");
        p_runner->initializeLaunchers();

        // refreshing variables and con lists
        p_runner->model()->realVariables(true);
        p_runner->model()->binaryVariables(true);
        p_runner->model()->integerVariables(true);
        p_runner->model()->constraints(true);

        p_runner->optimizer()->initialize();


        emit sendMsg("Starting optimization...");
        p_runner->run();
    }

}

//-----------------------------------------------------------------------------------------------
// Runs a single case
//-----------------------------------------------------------------------------------------------
void MainWindow::runCase(Case *c)
{

    if(!m_running)
    {
        setRunningState();

        emit sendMsg("Re-running case...");
        connect(p_runner, SIGNAL(casesFinished()), this, SLOT(onCaseFinished()));

        CaseQueue *cq = new CaseQueue();

        cq->push_back(c);

        p_runner->evaluate(cq, 0);

        //delete cq;
        //cq = 0;
    }
    else emit sendMsg("Could not evaluate case, model is running!");

}

//-----------------------------------------------------------------------------------------------
// Things to do after the runner has finished the optimization
//-----------------------------------------------------------------------------------------------
void MainWindow::onOptimizationFinished()
{
    emit sendMsg("Optimization finished!");

    p_runner->transferModelStateFromLauncher();

    setFinishedState();
    emit runFinished();
}

//-----------------------------------------------------------------------------------------------
// Things to do after a single case has finished running
//-----------------------------------------------------------------------------------------------
void MainWindow::onCaseFinished()
{
    disconnect(p_runner, SIGNAL(casesFinished()), this, SLOT(onCaseFinished()));

    emit sendMsg("Case finished!");

    p_runner->transferModelStateFromLauncher();

    setFinishedState();

    emit runFinished();


}


//-----------------------------------------------------------------------------------------------
// Opens the optimizer inspector
//-----------------------------------------------------------------------------------------------
void MainWindow::openOptimizerInspector()
{
    if(p_runner != 0)
    {
        if(p_obj_inpector == 0) p_obj_inpector = new InspectorOptimizer(p_runner);
        else p_obj_inpector->show();

    }
    else emit sendMsg("A project must be loaded before the optimizer properties can be opened!");
}


//-----------------------------------------------------------------------------------------------
// Saves the plot to a pdf file
//-----------------------------------------------------------------------------------------------
void MainWindow::savePlot()
{
    // getting the file name
    QString fileName = QFileDialog::getSaveFileName(this, "Save Plot to PDF", QDir::currentPath(), "Pdf file (*.pdf)");

    emit sendMsg("Saving the plot to file...");
    p_plot->savePlot(fileName);


}

//-----------------------------------------------------------------------------------------------
// Saves the model description to file
//-----------------------------------------------------------------------------------------------
void MainWindow::saveModelAs()
{
    // getting the file name
    QString fileName = QFileDialog::getSaveFileName(this, "Save Model Description", QDir::currentPath(), "Driver file (*.dat)");



    // openging the driver file
    QFile driver(fileName);


    if(!driver.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit sendMsg("Could not write to driver file: " + fileName);


    }
    else
    {
        emit sendMsg("Saving the model to file...");

        driver.resize(0);

        QTextStream out(&driver);

        // --- starting to write the model description

        // model type
        out << p_runner->model()->description();

        // simulator
        out << p_runner->reservoirSimulator()->description();

        // debug file
        if(p_runner->hasDebugFile()) out << "DEBUG " << p_runner->debugFileName() << "\n\n";

        // optimizer
        out << p_runner->optimizer()->description();

        // objective
        out << p_runner->model()->objective()->description();

        // master schedule
        out << "START MASTERSCHEDULE\n";
        for(int i = 0; i < p_runner->model()->masterSchedule().size(); ++i)
        {
            out << " " << p_runner->model()->masterSchedule().at(i) << "\n";
        }
        out << "END MASTERSCHEDULE\n\n";

        // reservoir
        out << p_runner->model()->reservoir()->description();

        // wells
        for(int i = 0; i < p_runner->model()->numberOfWells(); ++i)
        {
            out << p_runner->model()->well(i)->description();
        }

        // pipes
        for(int i = 0; i < p_runner->model()->numberOfPipes(); ++i)
        {
            out << p_runner->model()->pipe(i)->description();
        }

        // capacities
        for(int i = 0; i < p_runner->model()->numberOfCapacities(); ++i)
        {
            out << p_runner->model()->capacity(i)->description();
        }

        // user constraints
        if(p_runner->model()->numberOfUserDefinedConstraints() > 0)
        {
            out << "START CONSTRAINTS\n";
            for(int i = 0; i < p_runner->model()->numberOfUserDefinedConstraints(); ++i)
            {
                out << " " << p_runner->model()->userDefinedConstraint(i)->expression() << "\n";
            }
            out << "END CONSTRAINTS\n\n";
        }

        out.flush();
    }





}


//-----------------------------------------------------------------------------------------------
// called when the start/pause button is triggered
//-----------------------------------------------------------------------------------------------
void MainWindow::onStartButtonTriggered()
{
    if(m_running)
    {
        if(m_paused)
        {
            emit sendMsg("Resuming optimization...");
            p_runner->setPaused(false);
            p_act_startbutton->setIcon(QIcon(":new/images/pause"));
            p_act_startbutton->setText("Pause Optimization");
            m_paused = false;


        }
        else
        {
            emit sendMsg("Pausing optimization...");
            p_runner->setPaused(true);
            p_act_startbutton->setIcon(QIcon(":new/images/play"));
            m_paused = true;
            p_act_startbutton->setDisabled(true);

            connect(p_runner, SIGNAL(newCaseFinished(Case*)), this, SLOT(onLastCaseBeforePause(Case*)));



        }
    }

    else
    {
        runModel();
    }
}


//-----------------------------------------------------------------------------------------------
// sets all gui changes when model run starts
//-----------------------------------------------------------------------------------------------
void MainWindow::setRunningState()
{
    m_running = true;
    p_act_startbutton->setIcon(QIcon(":new/images/pause"));
    p_act_startbutton->setText("Pause Optimization");

}

//-----------------------------------------------------------------------------------------------
// sets all gui changes when model run finishes
//-----------------------------------------------------------------------------------------------
void MainWindow::setFinishedState()
{
    m_running = false;
    p_act_startbutton->setIcon(QIcon(":new/images/play"));
    p_act_startbutton->setText("Start Optimization");
}

//-----------------------------------------------------------------------------------------------
// transfers the model state before pause
//-----------------------------------------------------------------------------------------------
void MainWindow::onLastCaseBeforePause(Case *c)
{
    emit sendMsg("Transfering last case before pause...");
    p_runner->transferModelStateFromLauncher();

    disconnect(p_runner, SIGNAL(newCaseFinished(Case*)), this, SLOT(onLastCaseBeforePause(Case*)));

    p_act_startbutton->setDisabled(false);
    p_act_startbutton->setText("Resume Optimization");

}

} // namespace
