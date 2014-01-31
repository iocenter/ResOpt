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
#include "inspectorwellpath.h"
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
#include <QHBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QSizePolicy>


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
    p_inspector_wellpath(0),
    box_control(0),
    box_varcon(0),
    box_wellpath(0),
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

    QScrollArea *scroll_area = new QScrollArea(this);

    widget = new QWidget(this);
    widget->setMinimumSize(600, 300);

    scroll_area->setAlignment(Qt::AlignCenter);
    scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area->setWidget(widget);

    QSizePolicy policy = scroll_area->sizePolicy();
    policy.setVerticalStretch(1);
    policy.setHorizontalStretch(1);
    policy.setVerticalPolicy(QSizePolicy::Expanding);

    QVBoxLayout *layout_main = new QVBoxLayout(this);
    setLayout(layout_main);

    QVBoxLayout *layout = new QVBoxLayout(widget);
    int row = 0;

    widget->setLayout(layout);

    // ---- setting up the control variables -----
    box_control = new QGroupBox("Control Variables", widget);
    box_control->setStyleSheet("QGroupBox{border:2px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");
    box_control->setFixedWidth(500);
    QVBoxLayout *layout_control = new QVBoxLayout(box_control);

    box_control->setLayout(layout_control);


    for(int i = 0; i < p_well->numberOfControls(); ++i)
    {
        WellControl *wc = p_well->control(i);
        InspectorWellControl *iwc = new InspectorWellControl(wc->endTime(), wc->controlVar()->value(), wc->controlVar()->max(), wc->controlVar()->min(), wc->type(), widget, i == 0);
        m_controls.push_back(iwc);
        layout_control->addWidget(iwc);
    }

    // show/hide
    p_btn_control = new QPushButton("-", widget);
    p_btn_control->setFixedSize(25, 25);
    p_btn_control->setCheckable(true);
    p_btn_control->setChecked(false);
    connect(p_btn_control, SIGNAL(toggled(bool)), this, SLOT(hideControls(bool)));
    layout_control->addWidget(p_btn_control);


    layout->addWidget(box_control, 0, Qt::AlignHCenter);
    ++row;


    // ---- setting up the variable well connections
    if(p_well->hasVariableConnections())
    {
        box_varcon = new QGroupBox("Connection Variables", widget);
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
                                                                                        widget);
            m_varcons.push_back(iwcv);
            layout_varcon->addWidget(iwcv);

        }

        // show/hide
        p_btn_varcon = new QPushButton("-", widget);
        p_btn_varcon->setFixedSize(25, 25);
        p_btn_varcon->setCheckable(true);
        p_btn_varcon->setChecked(false);
        connect(p_btn_varcon, SIGNAL(toggled(bool)), this, SLOT(hideConnectionVariables(bool)));
        layout_varcon->addWidget(p_btn_varcon);


        layout->addWidget(box_varcon, 0, Qt::AlignHCenter);
        ++row;

    }

    // setting up the wellpath inspector
    if(p_well->hasWellPath())
    {
        box_wellpath = new QGroupBox("Well Path", widget);
        box_wellpath->setStyleSheet("QGroupBox{border:2px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");
        box_wellpath->setFixedWidth(500);
        QVBoxLayout *layout_wellpath = new QVBoxLayout(box_wellpath);
        box_wellpath->setLayout(layout_wellpath);

        // the inspector
        p_inspector_wellpath = new InspectorWellPath(p_well->wellPath(), widget);

        layout_wellpath->addWidget(p_inspector_wellpath);

        // show/hide
        p_btn_wellpath = new QPushButton("-", widget);
        p_btn_wellpath->setFixedSize(25, 25);
        p_btn_wellpath->setCheckable(true);
        p_btn_wellpath->setChecked(false);
        connect(p_btn_wellpath, SIGNAL(toggled(bool)), this, SLOT(hideWellPath(bool)));
        layout_wellpath->addWidget(p_btn_wellpath);


        layout->addWidget(box_wellpath, 0, Qt::AlignHCenter);
        ++row;

    }




    layout_main->addWidget(scroll_area, 1 , Qt::AlignHCenter);


    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // ---- setting up the buttons ----
    QWidget *widget_btn = new QWidget(this);
    QHBoxLayout *layout_btn = new QHBoxLayout(widget_btn);

    layout_btn->addWidget(&m_btn_ok);
    connect(&m_btn_ok, SIGNAL(clicked()), this, SLOT(saveAndClose()));

    layout_btn->addWidget(&m_btn_plot);
    connect(&m_btn_plot, SIGNAL(clicked()), this, SLOT(openPlot()));

    layout_btn->addWidget(&m_btn_close);
    connect(&m_btn_close, SIGNAL(clicked()), this, SLOT(close()));
    ++row;

    widget_btn->setLayout(layout_btn);

    layout_main->addWidget(widget_btn);



    widget->adjustSize();

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

    // saving the connection variables
    for(int i = 0; i < p_well->numberOfVariableConnections(); ++i)
    {
        p_well->variableConnection(i)->iVariable()->setValue(m_varcons.at(i)->i());
        p_well->variableConnection(i)->iVariable()->setMax(m_varcons.at(i)->iMax());
        p_well->variableConnection(i)->iVariable()->setMin(m_varcons.at(i)->iMin());

        p_well->variableConnection(i)->jVariable()->setValue(m_varcons.at(i)->j());
        p_well->variableConnection(i)->jVariable()->setMax(m_varcons.at(i)->jMax());
        p_well->variableConnection(i)->jVariable()->setMin(m_varcons.at(i)->jMin());

        p_well->variableConnection(i)->setK1(m_varcons.at(i)->k1());
        p_well->variableConnection(i)->setK2(m_varcons.at(i)->k2());
        p_well->variableConnection(i)->setWellIndex(m_varcons.at(i)->wi());
    }

    // saving the well path info
    if(p_inspector_wellpath != 0) p_inspector_wellpath->save();


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
    widget->adjustSize();
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
    widget->adjustSize();
}

//-----------------------------------------------------------------------------------------------
// hides or shows the well path
//-----------------------------------------------------------------------------------------------
void InspectorInjWell::hideWellPath(bool b)
{

    p_inspector_wellpath->setHidden(b);


    p_btn_wellpath->setText(b ? "+" : "-");

    if(box_wellpath != 0) box_wellpath->adjustSize();
    widget->adjustSize();
}


} // namespace
