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


#include "inspectorendpipe.h"

#include "plotstreams.h"

#include "endpipe.h"
#include "stream.h"

using ResOpt::Stream;

#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QDoubleValidator>


namespace ResOptGui
{

InspectorEndPipe::InspectorEndPipe(EndPipe *pipe, QWidget *parent) :
    QWidget(parent),
    p_pipe(pipe),
    m_lbl_pout("Minimum Outlet Pressure:", this),
    m_led_pout(this),
    m_cbx_pout(this),
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
void InspectorEndPipe::construct()
{
    setWindowTitle("End Pipe #" + QString::number(p_pipe->number()) + " Properties");

    QGridLayout *layout = new QGridLayout(this);

    setLayout(layout);

    // ---- setting up the outlet pressure  -----
    layout->addWidget(&m_lbl_pout, 0, 0);


    // the value
    m_led_pout.setText(QString::number(p_pipe->outletPressure()));
    m_led_pout.setValidator(new QDoubleValidator(this));

    layout->addWidget(&m_led_pout, 0, 1);

    // the unit type
    m_cbx_pout.addItem("BARA", Stream::METRIC);
    m_cbx_pout.addItem("PSIA", Stream::FIELD);

    m_cbx_pout.setCurrentIndex(m_cbx_pout.findData(p_pipe->outletUnit()));

    layout->addWidget(&m_cbx_pout, 0, 2);


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
void InspectorEndPipe::saveAndClose()
{

    emit sendMsg("Saving variable values for End Pipe #" + QString::number(p_pipe->number()) + " to model...");

    // saving outlet pressure
    p_pipe->setOutletPressure(m_led_pout.text().replace(",",".").toDouble());

    if(m_cbx_pout.itemData(m_cbx_pout.currentIndex()) == Stream::METRIC) p_pipe->setOutletUnit(Stream::METRIC);
    else p_pipe->setOutletUnit(Stream::FIELD);


    close();
}

//-----------------------------------------------------------------------------------------------
// Opens the streams plot for the separator
//-----------------------------------------------------------------------------------------------
void InspectorEndPipe::openPlot()
{
    QString title = "Plots for End Pipe #" + QString::number(p_pipe->number());

    PlotStreams *plt = new PlotStreams(title, p_pipe->streams());
}



} // namespace
