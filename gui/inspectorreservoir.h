/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2014 Aleksander O. Juell <aleksander.juell@ntnu.no>
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


#ifndef INSPECTORRESERVOIR_H
#define INSPECTORRESERVOIR_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QVector>
#include <QCheckBox>



namespace ResOpt
{
class Reservoir;
class ReservoirSimulator;
}

using ResOpt::Reservoir;
using ResOpt::ReservoirSimulator;

namespace ResOptGui
{

class InspectorReservoir : public QWidget
{
    Q_OBJECT
private:
    Reservoir *p_res;
    ReservoirSimulator *p_sim;



    QLabel *p_lbl_sim_type;
    QLineEdit *p_led_res_file;
    QLineEdit *p_led_mrst;
    QLineEdit *p_led_script;
    QLineEdit *p_led_matlab;

    QPushButton *p_btn_res_file;
    QPushButton *p_btn_mrst;
    QPushButton *p_btn_script;
    QPushButton *p_btn_matlab;

    QCheckBox *p_chk_script;
    QCheckBox *p_chk_keep_mat;




    QPushButton m_btn_close;
    QPushButton m_btn_ok;

    void construct();


public:
    explicit InspectorReservoir(Reservoir *res, ReservoirSimulator *sim, QWidget *parent = 0);

signals:
    void sendMsg(QString);

public slots:
    void saveAndClose();

    void browseResFile();
    void browseMrst();
    void browseMatlab();
    void browseScript();

    void onScriptStatusChanged(bool b);


};

} // namespace

#endif // INSPECTORRESERVOIR_H
