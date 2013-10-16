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


#ifndef INSPECTOROPTIMIZER_H
#define INSPECTOROPTIMIZER_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

namespace ResOpt
{
class Runner;
}

using ResOpt::Runner;

namespace ResOptGui
{

class MainWindow;

class InspectorOptimizer : public QWidget
{
    Q_OBJECT
private:
    Runner *p_runner;
    MainWindow *p_main_window;

    QComboBox *p_algorithm;
    QLineEdit *p_max_iter;
    QLineEdit *p_max_iter_cont;

    QPushButton *p_btn_close;
    QPushButton *p_btn_ok;

    int m_alg_in_runner;


    void construct();

public:
    explicit InspectorOptimizer(Runner *r, MainWindow *mw, QWidget *parent = 0);
    
signals:
    void sendMsg(QString);
    
public slots:
    void saveAndClose();
    
};

} // namespace

#endif // INSPECTOROPTIMIZER_H
