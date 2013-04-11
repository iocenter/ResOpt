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

#include <QtGui/QMainWindow>
#include <QtGui/QWidget>

#include "runner.h"

class QGraphicsView;
class QTabWidget;

using ResOpt::Runner;

namespace ResOptGui
{

class ModelScene;
class Console;
class Plot;
class InspectorOptimizer;

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


    Runner *p_runner;


    void createMenus();

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();
    
signals:
    void sendMsg(QString);
    
public slots:
    void loadModel();
    void runModel();
    void onOptimizationFinished();
    void openOptimizerInspector();
    
};

} // namespace ResOptGui

#endif // MAINWINDOW_H
