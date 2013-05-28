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


#ifndef PLOTSTREAMZ_H
#define PLOTSTREAMZ_H

#include <QtWidgets/QWidget>
#include <QVector>

#include "qcustomplot.h"

#include "stream.h"

using ResOpt::Stream;

namespace ResOptGui
{

class PlotStreams : public QWidget
{
    Q_OBJECT

private:


    QCustomPlot m_custom_plot;

    QVector<Stream*> m_streams;

    void plotData();


public:
    PlotStreams(const QString &title, QVector<Stream*> streams, QWidget *parent = 0);



public slots:

};


} // namespace

#endif // PLOTSTREAMZ_H
