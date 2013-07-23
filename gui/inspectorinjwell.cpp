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
#include "inspectorwellconnectionvariable.h"
#include "plotstreams.h"

#include "injectionwell.h"
#include "intvariable.h"
#include "realvariable.h"
#include "wellcontrol.h"
#include "wellconnectionvariable.h"
#include "wellconnection.h"



#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>


using ResOpt::IntVariable;
using ResOpt::RealVariable;
using ResOpt::WellControl;
using ResOpt::WellConnectionVariable;
using ResOpt::WellConnection;

namespace ResOptGui
{

InspectorInjWell::InspectorInjWell(InjectionWell *well, QWidget *parent) :
    QWidget(parent),
    p_well(well),
    box_control(0),
    box_varcon(0),
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
    int row = 0;

    setLayout(layout);

    // ---- setting up the control variables -----
    box_control = new QGroupBox("Control Variables", this);
    box_control->setStyleSheet("QGroupBox{border:2px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");
    box_control->setFixedWidth(500);
    QVBoxLayout *layout_control = new QVBoxLayout(box_control);

    box_control->setLayout(layout_control);


    for(int i = 0; i < p_well->numberOfControls(); ++i)
    {
        WellControl *wc = p_well->control(i);
        InspectorWellControl *iwc = new InspectorWellControl(wc->endTime(), wc->controlVar()->value(), wc->controlVar()->max(), wc->controlVar()->min(), wc->type(), this, i == 0);
        m_controls.push_back(iwc);
        layout_control->addWidget(iwc);
    }

    // show/hide
    p_btn_control = new QPushButton("-", this);
    p_btn_control->setFixedSize(25, 25);
    p_btn_control->setCheckable(true);
    p_btn_control->setChecked(false);
    connect(p_btn_control, SIGNAL(toggled(bool)), this, SLOT(hideControls(bool)));
    layout_control->addWidget(p_btn_control);


    layout->addWidget(box_control, row, 0, 1, 3, Qt::AlignCenter);
    ++row;


    // ---- setting up the variable well connections
    if(p_well->hasVariableConnections())
    {
        box_varcon = new QGroupBox("Connection Variables", this);
        box_varcon->setStyleSheet("QGroupBox{border:2px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");
        box_varcon->setFixedWidth(500);
        QVBoxLayout *layout_varcon = new QVBoxLayout(box_varcon);
        //layout_varcon->setSizeConstraint(QLayout::SetFixedSize);
        box_varcon->setLayout(layout_varcon);

        for(int i = 0; i < p_well->numberOfVariableConnections(); ++i)
        {
            WellConnectionVariable *wcv = p_well->variableConnection(i);
            InspectorWellConnectionVariable *iwcv = new InspectorWellConnectionVariable(wcv->iVariable()->value(),
                                                                                        wcv->iVariable()->max(),
                                                                                        wcv->iVariable()->min(),
                                                                                        wcv->jVariable()->value(),
                                                                                        wcv->jVariable()->max(),
                                                                                        wcv->jVariable()->min(),
                                                                                        wcv->wellConnection()->k1(),
                                                                                        wcv->wellConnection()->k2(),
                                                                                        wcv->wellConnection()->wellIndex(),
                                                                                        this);
            m_varcons.push_back(iwcv);
            layout_varcon->addWidget(iwcv);

        }

        // show/hide
        p_btn_varcon = new QPushButton("-", this);
        p_btn_varcon->setFixedSize(25, 25);
        p_btn_varcon->setCheckable(true);
        p_btn_varcon->setChecked(false);
        connect(p_btn_varcon, SIGNAL(toggled(bool)), this, SLOT(hideConnectionVariables(bool)));
        layout_varcon->addWidget(p_btn_varcon);


        layout->addWidget(box_varcon, row, 0, 1, 3, Qt::AlignCenter);
        ++row;

    }


    // ---- setting up the buttons ----
    layout->addWidget(&m_btn_ok, row, 0);
    connect(&m_btn_ok, SIGNAL(clicked()), this, SLOT(saveAndClose()));

    layout->addWidget(&m_btn_plot, row, 1);
    connect(&m_btn_plot, SIGNAL(clicked()), this, SLOT(openPlot()));

    layout->addWidget(&m_btn_close, row, 2);
    connect(&m_btn_close, SIGNAL(clicked()), this, SLOT(close()));
    ++row;





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

//-----------------------------------------------------------------------------------------------
// hides or shows the controls
//-----------------------------------------------------------------------------------------------
void InspectorInjWell::hideControls(bool b)
{
    for(int i = 0; i < m_controls.size(); ++i)
    {
        m_controls.at(i)->setHidden(b);
    }

    p_btn_control->setText(b ? "+" : "-");

    box_control->adjustSize();
    this->adjustSize();
}

//-----------------------------------------------------------------------------------------------
// hides or shows the connection variables
//-----------------------------------------------------------------------------------------------
void InspectorInjWell::hideConnectionVariables(bool b)
{
    for(int i = 0; i < m_varcons.size(); ++i)
    {
        m_varcons.at(i)->setHidden(b);
    }

    p_btn_varcon->setText(b ? "+" : "-");

    if(box_varcon != 0) box_varcon->adjustSize();
    this->adjustSize();
}


} // namespace
