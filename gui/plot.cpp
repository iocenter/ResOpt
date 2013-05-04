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


#include "plot.h"
#include "mainwindow.h"

#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>

namespace ResOptGui
{

Plot::Plot(MainWindow *mw, QWidget *parent) :
    QWidget(parent),
    p_mainwindow(mw),
    m_custom_plot(this),
    m_max(10),
    m_min(0)
{
    QGridLayout *layout = new QGridLayout;

    setLayout(layout);

    // setting up the plot
    layout->addWidget(&m_custom_plot, 0, 0, 1, 3);

    m_custom_plot.setTitle("Objective Value");

    m_custom_plot.setInteraction(QCustomPlot::iSelectItems);
   // m_custom_plot.setSelectionTolerance(10);


    m_custom_plot.addGraph();

    m_custom_plot.graph(0)->setLineStyle(QCPGraph::lsNone);
    m_custom_plot.graph(0)->setScatterStyle(QCP::ssDisc);

    // graph for feasibility
    m_custom_plot.addGraph();
    m_custom_plot.graph(1)->setLineStyle(QCPGraph::lsNone);
    m_custom_plot.graph(1)->setScatterStyle(QCP::ssDisc);
    m_custom_plot.graph(1)->setPen(QPen(Qt::red));
    //m_custom_plot.graph(1)->setScatterSize(10);


    m_custom_plot.xAxis->setLabel("Model Evaluation #");
    m_custom_plot.xAxis->setRange(0, 5);
    m_custom_plot.xAxis->setAutoTickStep(false);
    m_custom_plot.xAxis->setTickStep(1.0);

    m_custom_plot.yAxis->setLabel("Objective Value");
    m_custom_plot.yAxis->setNumberFormat("gbc");

    m_custom_plot.setRangeDrag(Qt::Vertical);
    m_custom_plot.setRangeZoom(Qt::Vertical);
    m_custom_plot.setRangeDrag(Qt::Horizontal);

    // setting up the x-axis slider
    p_sld_xaxis = new QSlider(Qt::Horizontal, this);
    p_sld_xaxis->setMaximum(5);
    p_sld_xaxis->setMinimum(1);
    p_sld_xaxis->setValue(5);

    layout->addWidget(p_sld_xaxis, 1, 0, 1, 3);

    connect(p_sld_xaxis, SIGNAL(valueChanged(int)), this, SLOT(onXAxisSliderChanged()));

    // setting up the clear button
    p_btn_clear = new QPushButton("Clear Plot", this);
    connect(p_btn_clear, SIGNAL(clicked()), this, SLOT(clearCases()));

    layout->addWidget(p_btn_clear, 2, 0);

    // setting up the rerun button
    p_btn_rerun = new QPushButton("Run Selected", this);
    p_btn_rerun->setDisabled(true);

    layout->addWidget(p_btn_rerun, 2, 1);



    connect(p_mainwindow, SIGNAL(runFinished()), this, SLOT(onSelectionChanged()));
    connect(&m_custom_plot, SIGNAL(selectionChangedByUser()), this, SLOT(onSelectionChanged()));
    connect(p_btn_rerun, SIGNAL(clicked()), this, SLOT(rerunSelectedCase()));



}

Plot::~Plot()
{
    for(int i = 0; i < m_cases.size(); ++i)
    {
        delete m_cases.at(i);
    }
    m_cases.resize(0);
}

//-----------------------------------------------------------------------------------------------
// Adds data from the case to the plot
//-----------------------------------------------------------------------------------------------
void Plot::addCase(Case *c)
{
    // adding the case to the vector
    m_cases.push_back(new Case(*c));


    m_custom_plot.graph(0)->addData(m_cases.size(), c->objectiveValue());

    // adding point to graph 1 if infeasible
    if(!p_mainwindow->runner()->isFeasible(c)) m_custom_plot.graph(1)->addData(m_cases.size(), c->objectiveValue());

    // x axis range
    if(m_cases.size() >= 5) m_custom_plot.xAxis->setRange(m_cases.size() - p_sld_xaxis->value(), m_cases.size() + 1);

    // y axis range
    if(c->objectiveValue() < m_min) m_min = c->objectiveValue();
    if(c->objectiveValue() > m_max) m_max = c->objectiveValue();

    double padding = (m_max - m_min)*0.1;

    m_custom_plot.yAxis->setRange(m_min - padding, m_max + padding);



    // checking if the x-axis tick step must change
    if(m_cases.size() > 10)
    {
        int tick = m_cases.size() / 10;
        m_custom_plot.xAxis->setTickStep(tick);
    }


    // adding tracer
    QCPItemTracer *tracer = new QCPItemTracer(&m_custom_plot);
    tracer->setGraph(m_custom_plot.graph(0));
    tracer->setGraphKey(m_cases.size());
    tracer->setStyle(QCPItemTracer::tsPlus);
    tracer->setSize(20);
    QPen pen(Qt::blue);
    pen.setWidth(0);
    pen.setStyle(Qt::NoPen);
    tracer->setPen(pen);

    QPen pen_sel(Qt::green);
    pen_sel.setWidth(1);
    tracer->setSelectedPen(pen_sel);
    tracer->setSelectable(true);

    m_custom_plot.addItem(tracer);

    // updating the slider max
    p_sld_xaxis->setMaximum(m_cases.size());
    p_sld_xaxis->setValue(m_cases.size());



   // m_custom_plot.replot();

}


//-----------------------------------------------------------------------------------------------
// Clears the cases from the plot
//-----------------------------------------------------------------------------------------------
void Plot::clearCases()
{
    m_custom_plot.clearItems();

    for(int i = 0; i < m_custom_plot.graphCount(); ++i)
    {
        m_custom_plot.graph(i)->clearData();
    }



    for(int i = 0; i < m_cases.size(); ++i) delete m_cases.at(i);
    m_cases.resize(0);

    m_max = 10;
    m_min = 0;

    m_custom_plot.xAxis->setRange(0, 5);
    m_custom_plot.yAxis->setRange(0, 5);

    p_sld_xaxis->setMaximum(5);
    p_sld_xaxis->setValue(5);
    //m_custom_plot.replot();

    p_btn_rerun->setDisabled(true);
}

//-----------------------------------------------------------------------------------------------
// called when the selection on the plot has changed
//-----------------------------------------------------------------------------------------------
void Plot::onSelectionChanged()
{
    if(m_custom_plot.selectedItems().size() == 1 && !p_mainwindow->isRunning()) p_btn_rerun->setEnabled(true);
    else p_btn_rerun->setEnabled(false);
}


//-----------------------------------------------------------------------------------------------
// reruns the selected case
//-----------------------------------------------------------------------------------------------
void Plot::rerunSelectedCase()
{
    if(!p_mainwindow->isRunning() && m_custom_plot.selectedItems().size() == 1)
    {
        // finding the case corresponding to the selected tracer
        QCPItemTracer *tracer = dynamic_cast<QCPItemTracer*>(m_custom_plot.selectedItems().at(0));

        if(tracer != 0)
        {
            int case_no = tracer->graphKey() - 1;

            // checknig that the case_no is within vector bounds
            if(case_no < m_cases.size() && case_no >= 0)
            {
                p_mainwindow->runCase(m_cases.at(case_no));
                p_btn_rerun->setDisabled(true);

            }

        }
    }
}

//-----------------------------------------------------------------------------------------------
// Saves the plot to PDF
//-----------------------------------------------------------------------------------------------
void Plot::savePlot(const QString &fileName)
{
    m_custom_plot.savePdf(fileName);
}


//-----------------------------------------------------------------------------------------------
// Changed the x axis of the plot when slider changes
//-----------------------------------------------------------------------------------------------
void Plot::onXAxisSliderChanged()
{

    if(m_cases.size() >= 5) m_custom_plot.xAxis->setRange(m_cases.size() - p_sld_xaxis->value(), m_cases.size() + 1);

    m_custom_plot.replot();
}

} // namespace
