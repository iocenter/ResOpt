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


#include "plotstreams.h"


#include <QHBoxLayout>

namespace ResOptGui
{

PlotStreams::PlotStreams(const QString &title, QVector<Stream*> streams, QWidget *parent) :
    QWidget(parent),
    m_custom_plot(this),
    m_streams(streams)
{
    setWindowTitle(title);
    setAttribute(Qt::WA_DeleteOnClose);

    QHBoxLayout *layout = new QHBoxLayout;

    setLayout(layout);

    layout->addWidget(&m_custom_plot);

   // m_custom_plot.setTitle("Objective Value");



    //graph(0)->addData(0,1);
    //graph(0)->addData(1,2);

    m_custom_plot.xAxis->setLabel("Time");
    m_custom_plot.yAxis->setLabel("Value");

    m_custom_plot.setAutoAddPlottableToLegend(true);
    m_custom_plot.legend->setVisible(true);


    m_custom_plot.setRangeDrag(Qt::Vertical);
    m_custom_plot.setRangeZoom(Qt::Vertical);

    plotData();

    resize(800, 600);

    show();

}

//-----------------------------------------------------------------------------------------------
// Sets up the plot with the selected data type
//-----------------------------------------------------------------------------------------------
void PlotStreams::plotData()
{
    // setting up the graphs
    m_custom_plot.addGraph();
    m_custom_plot.graph(0)->setName("Oil");
    m_custom_plot.graph(0)->setPen(QPen(Qt::green));

    m_custom_plot.addGraph();
    m_custom_plot.graph(1)->setName("Gas");
    m_custom_plot.graph(1)->setPen(QPen(Qt::red));

    m_custom_plot.addGraph();
    m_custom_plot.graph(2)->setName("Water");
    m_custom_plot.graph(2)->setPen(QPen(Qt::blue));

    m_custom_plot.addGraph();
    m_custom_plot.graph(3)->setName("Pressure");
    m_custom_plot.graph(3)->setPen(QPen(Qt::black));



    int max, min = 0;

    double t_previous = 0;

    for(int i = 0; i < m_streams.size(); ++i)
    {
        //oil
        m_custom_plot.graph(0)->addData(t_previous, m_streams.at(i)->oilRate(true));
        m_custom_plot.graph(0)->addData(m_streams.at(i)->time(), m_streams.at(i)->oilRate(true));

        if(max < m_streams.at(i)->oilRate(true)) max = m_streams.at(i)->oilRate(true);

        // gas
        m_custom_plot.graph(1)->addData(t_previous, m_streams.at(i)->gasRate(true));
        m_custom_plot.graph(1)->addData(m_streams.at(i)->time(), m_streams.at(i)->gasRate(true));

        if(max < m_streams.at(i)->gasRate(true)) max = m_streams.at(i)->gasRate(true);

        // water
        m_custom_plot.graph(2)->addData(t_previous, m_streams.at(i)->waterRate(true));
        m_custom_plot.graph(2)->addData(m_streams.at(i)->time(), m_streams.at(i)->waterRate(true));

        if(max < m_streams.at(i)->waterRate(true)) max = m_streams.at(i)->waterRate(true);

        // pressure
        m_custom_plot.graph(3)->addData(t_previous, m_streams.at(i)->pressure(true));
        m_custom_plot.graph(3)->addData(m_streams.at(i)->time(), m_streams.at(i)->pressure(true));

        if(max < m_streams.at(i)->pressure(true)) max = m_streams.at(i)->pressure(true);


        // updating the time of the previous stream
        t_previous = m_streams.at(i)->time() + 0.1;
    }



    // setting the axis range
    m_custom_plot.yAxis->setRange(min, max + 0.1*(max-min));

    m_custom_plot.xAxis->setRange(0, m_streams.last()->time()*1.05);

    m_custom_plot.replot();
}



} // namespace
