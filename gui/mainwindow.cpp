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

#include <QtGui/QAction>
#include <QtGui/QKeySequence>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGraphicsView>
#include <QtGui/QFileDialog>
#include <QtGui/QTabWidget>

#include <QDir>


using ResOpt::Case;
using ResOpt::CaseQueue;
using ResOpt::Optimizer;
using ResOpt::Model;

namespace ResOptGui
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    p_runner(0),
    p_obj_inpector(0),
    m_running(false)
{

    // setting up the model scene
    p_model_scene = new ModelScene(this);
    connect(p_model_scene, SIGNAL(sendMsg(QString)), this, SIGNAL(sendMsg(QString)));

    // setting up the model view
    QHBoxLayout *layout = new QHBoxLayout;

    p_model_view = new QGraphicsView(p_model_scene);
    p_model_view->setMinimumSize(800, 600);

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

    // load project
    QAction *p_act_load =  p_file_menu->addAction("Load Project");
    connect(p_act_load, SIGNAL(triggered()), this, SLOT(loadModel()));


    // ---------- create optimization menu -------------------
    QMenu *p_opt_menu = menuBar()->addMenu("Optimization");

    // start
    QAction *p_act_start = p_opt_menu->addAction("Start");
    connect(p_act_start, SIGNAL(triggered()), this, SLOT(runModel()));


    // properties
    QAction *p_act_opt_prop = p_opt_menu->addAction("Properties");
    connect(p_act_opt_prop, SIGNAL(triggered()), this, SLOT(openOptimizerInspector()));



}


//-----------------------------------------------------------------------------------------------
// Select model file, and set up runner
//-----------------------------------------------------------------------------------------------
void MainWindow::loadModel()
{
    // letting the user specify the driver file
    QString fileName = QFileDialog::getOpenFileName(this, "Load Project", QDir::currentPath(), "ResOpt Driver File (*.dat)");

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
        m_running = true;

        emit sendMsg("Updating launchers with current model...");
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

    m_running = false;

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

    m_running = false;

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

} // namespace
