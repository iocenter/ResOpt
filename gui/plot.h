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

#include <QtWidgets/QWidget>
#include <QVector>
#include "qcustomplot.h"


#include "case.h"

using ResOpt::Case;

class QPushButton;
class QSlider;
class QComboBox;

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
    QSlider *p_sld_xaxis;
    QComboBox *p_series;

    QCustomPlot m_custom_plot;
    QVector<Case*> m_cases;
    QVector<Case*> m_plotted_cases;

    bool m_user_changed_slider;
    bool m_series_updated;

    void updateSeriesList(Case *c);
    void addToPlot(Case *c, bool replot=true);


public:
    Plot(MainWindow *mw, QWidget *parent = 0);
    ~Plot();

    void savePlot(const QString &fileName);

public slots:

    void addCase(Case *c);
    void clearCases();
    void onSelectionChanged();
    void rerunSelectedCase();
    void onXAxisSliderChanged();
    void onXAxisSliderPressed();
    void onSeriesSelectionChanged(int index);

};


} // namespace

#endif // PLOT_H
