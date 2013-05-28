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


#include "inspectorinjwell.h"

#include "inspectorvariable.h"
#include "inspectorwellcontrol.h"
#include "plotstreams.h"

#include "injectionwell.h"
#include "intvariable.h"
#include "realvariable.h"
#include "wellcontrol.h"



#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>


using ResOpt::IntVariable;
using ResOpt::RealVariable;
using ResOpt::WellControl;

namespace ResOptGui
{

InspectorInjWell::InspectorInjWell(InjectionWell *well, QWidget *parent) :
    QWidget(parent),
    p_well(well),
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
void InspectorInjWell::construct()
{
    setWindowTitle("Injection Well " + p_well->name() + " Properties");

    QGridLayout *layout = new QGridLayout(this);

    setLayout(layout);

    // setting up the control variables
    QGroupBox *box_control = new QGroupBox("Control Variables", this);
    box_control->setStyleSheet("QGroupBox{border:2px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");

    QVBoxLayout *layout_control = new QVBoxLayout(box_control);
    box_control->setLayout(layout_control);


    for(int i = 0; i < p_well->numberOfControls(); ++i)
    {
        WellControl *wc = p_well->control(i);
        InspectorWellControl *iwc = new InspectorWellControl(wc->endTime(), wc->controlVar()->value(), wc->controlVar()->max(), wc->controlVar()->min(), wc->type(), this, i == 0);
        m_controls.push_back(iwc);
        layout_control->addWidget(iwc);
    }


    layout->addWidget(box_control, 0, 0, 1, 3);

    // setting up the bhp constraints


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
void InspectorInjWell::saveAndClose()
{

    emit sendMsg("Saving variable values for Injection Well " + p_well->name() + " to model...");

    // saving control variables
    for(int i = 0; i < p_well->numberOfControls(); ++i)
    {
        p_well->control(i)->controlVar()->setValue(m_controls.at(i)->value());
        p_well->control(i)->controlVar()->setMax(m_controls.at(i)->max());
        p_well->control(i)->controlVar()->setMin(m_controls.at(i)->min());

        p_well->control(i)->setType(m_controls.at(i)->type());
    }

    close();
}

//-----------------------------------------------------------------------------------------------
// Opens the streams plot for the separator
//-----------------------------------------------------------------------------------------------
void InspectorInjWell::openPlot()
{
    QString title = "Plots for Injection Well " + p_well->name();

    PlotStreams *plt = new PlotStreams(title, p_well->streams());
}



} // namespace
