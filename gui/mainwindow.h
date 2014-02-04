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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QSize>

#include "runner.h"

class QGraphicsView;
class QTabWidget;
class QAction;
class QMovie;

namespace ResOpt
{
class Case;
}

using ResOpt::Runner;
using ResOpt::Case;

/**
 * @brief This namespace is used for all GUI elements.
 */
namespace ResOptGui
{

class ModelScene;
class Console;
class Plot;
class InspectorOptimizer;

/**
 * @brief Main GUI window for ResOpt
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    ModelScene *p_model_scene;
    QGraphicsView *p_model_view;

    QTabWidget *p_tab_widget;

    Console *p_console;
    Plot *p_plot;
    InspectorOptimizer *p_obj_inpector;

    QAction *p_act_startbutton;
    QAction *p_act_runoncebutton;





    Runner *p_runner;

    bool m_running;
    bool m_paused;


    void createMenus();

    void setRunningState();
    void setFinishedState();

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    virtual QSize sizeHint() {return QSize(800, 600);}

    bool isRunning() {return m_running;}
    bool isPaused() {return m_paused;}

    Runner* runner() {return p_runner;}
    
signals:
    void sendMsg(QString);
    void runFinished();
    
public slots:
    void loadModel();
    void runModel();
    void onOptimizationFinished(Runner *r, Case *c);
    void runCase(Case *c);
    void onCaseFinished();
    void openOptimizerInspector();
    void onStartButtonTriggered();
    void onRunonceButtonTriggered();
    void savePlot();
    void saveModelAs();
    void onLastCaseBeforePause(Case *c);

    void error(QString &message);
    
};

} // namespace ResOptGui

#endif // MAINWINDOW_H
