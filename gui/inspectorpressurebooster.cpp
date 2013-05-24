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


#include "inspectorpressurebooster.h"

#include "inspectorvariable.h"
#include "inspectorvariableinstall.h"
#include "plotstreams.h"

#include "pressurebooster.h"
#include "intvariable.h"
#include "realvariable.h"

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>


using ResOpt::IntVariable;
using ResOpt::RealVariable;

namespace ResOptGui
{

InspectorPressureBooster::InspectorPressureBooster(PressureBooster *boost, QWidget *parent) :
    QWidget(parent),
    p_booster(boost),
    m_btn_close("Close", this),
    m_btn_ok("Ok", this),
    m_btn_plot("Plot", this)
{

    setAttribute(Qt::WA_DeleteOnClose);


    construct();

    show();
}


//-----------------------------------------------------------------------------------------------
// constructs the view
//-----------------------------------------------------------------------------------------------
void InspectorPressureBooster::construct()
{
    setWindowTitle("Booster #" + QString::number(p_booster->number()) + " Properties");

    QGridLayout *layout = new QGridLayout(this);

    setLayout(layout);

    // setting up the variables
    QGroupBox *box_var = new QGroupBox("Variables", this);
    box_var->setStyleSheet("QGroupBox{border:2px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");

    QVBoxLayout *layout_var = new QVBoxLayout(box_var);
    box_var->setLayout(layout_var);

    QVector<double> schedule;
    for(int i = 0; i < p_booster->numberOfStreams(); ++i)
    {
        schedule.push_back(p_booster->stream(i)->time());
    }

    p_var_install = new InspectorVariableInstall(p_booster->installTime()->value(), p_booster->installTime()->max(), p_booster->installTime()->min(), schedule, this, true);
    layout_var->addWidget(p_var_install);

    p_var_capacity = new InspectorVariable("Capacity:", p_booster->capacityVariable()->value(), p_booster->capacityVariable()->max(), p_booster->capacityVariable()->min(), this);
    layout_var->addWidget(p_var_capacity);

    p_var_boostpressure = new InspectorVariable("Boosting pressure:", p_booster->pressureVariable()->value(), p_booster->pressureVariable()->max(), p_booster->pressureVariable()->min(), this);
    layout_var->addWidget(p_var_boostpressure);

    layout->addWidget(box_var, 0, 0, 1, 3);


    // setting up the buttons
    layout->addWidget(&m_btn_ok, 1, 0);
    connect(&m_btn_ok, SIGNAL(clicked()), this, SLOT(saveAndClose()));

    layout->addWidget(&m_btn_plot, 1, 1);
    connect(&m_btn_plot, SIGNAL(clicked()), this, SLOT(openPlot()));

    layout->addWidget(&m_btn_close, 1, 2);
    connect(&m_btn_close, SIGNAL(clicked()), this, SLOT(close()));



}


//-----------------------------------------------------------------------------------------------
// Saves the current values to the model, and closes the window
//-----------------------------------------------------------------------------------------------
void InspectorPressureBooster::saveAndClose()
{
    // install time
    p_booster->installTime()->setValue(p_var_install->value());
    p_booster->installTime()->setMax(p_var_install->max());
    p_booster->installTime()->setMin(p_var_install->min());

    // capacity
    p_booster->capacityVariable()->setValue(p_var_capacity->value());
    p_booster->capacityVariable()->setMax(p_var_capacity->max());
    p_booster->capacityVariable()->setMin(p_var_capacity->min());

    // boost pressure
    p_booster->pressureVariable()->setValue(p_var_boostpressure->value());
    p_booster->pressureVariable()->setMax(p_var_boostpressure->max());
    p_booster->pressureVariable()->setMin(p_var_boostpressure->min());

    emit sendMsg("Saving variable values for Booster #" + QString::number(p_booster->number()) + " to model...");

    close();
}

//-----------------------------------------------------------------------------------------------
// Opens the streams plot for the separator
//-----------------------------------------------------------------------------------------------
void InspectorPressureBooster::openPlot()
{
    QString title = "Plots for Booster #" + QString::number(p_booster->number());

    PlotStreams *plt = new PlotStreams(title, p_booster->streams());
}



} // namespace
