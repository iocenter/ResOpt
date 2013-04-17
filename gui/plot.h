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


#ifndef PLOT_H
#define PLOT_H

#include <QtGui/QWidget>
#include <QVector>
#include "qcustomplot.h"


#include "case.h"

using ResOpt::Case;

class QPushButton;

namespace ResOptGui
{

class MainWindow;

class Plot : public QWidget
{
    Q_OBJECT

private:
    MainWindow *p_mainwindow;

    double m_max;
    double m_min;

    QPushButton *p_btn_clear;
    QPushButton *p_btn_rerun;

    QCustomPlot m_custom_plot;
    QVector<Case*> m_cases;


public:
    Plot(MainWindow *mw, QWidget *parent = 0);
    ~Plot();



public slots:

    void addCase(Case *c);
    void clearCases();
    void onSelectionChanged();
    void rerunSelectedCase();
};


} // namespace

#endif // PLOT_H
