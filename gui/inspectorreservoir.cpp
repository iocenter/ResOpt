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


#include "inspectorreservoir.h"

#include "plotstreams.h"

#include "reservoir.h"
#include "reservoirsimulator.h"
#include "mrstbatchsimulator.h"
#include "vlpsimulator.h"
#include "gprssimulator.h"



#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QHBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QDoubleValidator>
#include <QFileDialog>


using ResOpt::MrstBatchSimulator;
using ResOpt::VlpSimulator;
using ResOpt::GprsSimulator;

namespace ResOptGui
{

InspectorReservoir::InspectorReservoir(Reservoir *res, ReservoirSimulator *sim, QWidget *parent) :
    QWidget(parent),
    p_res(res),
    p_sim(sim),
    m_btn_close("Close", this),
    m_btn_ok("Ok", this)
{

    setAttribute(Qt::WA_DeleteOnClose);


    construct();

    show();
}


//-----------------------------------------------------------------------------------------------
// constructs the view
//-----------------------------------------------------------------------------------------------
void InspectorReservoir::construct()
{
    setWindowTitle("Reservoir Properties");

    QVBoxLayout *layout_main = new QVBoxLayout(this);

    setLayout(layout_main);

    QWidget *widget = new QWidget(this);
    QGridLayout *layout = new QGridLayout(widget);
    widget->setLayout(layout);

    int row = 0;
    // ---- setting up the reservoir simulator  -----
    QLabel *lbl_sim = new QLabel("Reservoir simulator: ", widget);
    layout->addWidget(lbl_sim, row, 1);


    p_lbl_sim_type = new QLabel(widget);
    p_lbl_sim_type->setText("Unknown...");

    if(dynamic_cast<MrstBatchSimulator*>(p_sim) != 0) p_lbl_sim_type->setText("MRST");
    else if(dynamic_cast<VlpSimulator*>(p_sim) != 0) p_lbl_sim_type->setText("VLP");
    else if(dynamic_cast<GprsSimulator*>(p_sim) != 0) p_lbl_sim_type->setText("GPRS");

    layout->addWidget(p_lbl_sim_type, row, 2);

    ++row;

    // ---- setting up the reservoir file  -----
    QLabel *lbl_res_file = new QLabel(widget);
    lbl_res_file->setText("Reservoir input file: ");
    layout->addWidget(lbl_res_file, row, 1);

    p_led_res_file = new QLineEdit(widget);
    p_led_res_file->setText(p_res->file());
    p_led_res_file->setMinimumWidth(300);
    layout->addWidget(p_led_res_file, row, 2);

    p_btn_res_file = new QPushButton("...", widget);
    layout->addWidget(p_btn_res_file, row, 3);
    connect(p_btn_res_file, SIGNAL(clicked()), this, SLOT(browseResFile()));

    ++row;


    // ---- setting up the mrst path  -----
    QLabel *lbl_mrst = new QLabel(widget);
    lbl_mrst->setText("MRST path: ");
    layout->addWidget(lbl_mrst, row, 1);

    p_led_mrst = new QLineEdit(widget);
    p_led_mrst->setText(p_res->mrstPath());
    p_led_mrst->setMinimumWidth(300);
    layout->addWidget(p_led_mrst, row, 2);

    p_btn_mrst = new QPushButton("...", widget);
    layout->addWidget(p_btn_mrst, row, 3);
    connect(p_btn_mrst, SIGNAL(clicked()), this, SLOT(browseMrst()));

    ++row;


    // ---- setting up the custom mrst script  -----
    QLabel *lbl_script = new QLabel(widget);
    lbl_script->setText("Custom MRST script: ");
    layout->addWidget(lbl_script, row, 1);

    p_led_script = new QLineEdit(widget);
    p_led_script->setText(p_res->mrstScript());
    p_led_script->setMinimumWidth(300);
    layout->addWidget(p_led_script, row, 2);

    p_btn_script = new QPushButton("...", widget);
    layout->addWidget(p_btn_script, row, 3);
    connect(p_btn_script, SIGNAL(clicked()), this, SLOT(browseScript()));

    p_chk_script = new QCheckBox(widget);
    layout->addWidget(p_chk_script, row, 0);

    if(p_res->useMrstScript())
    {
        p_chk_script->setChecked(true);
    }
    else
    {
        p_led_script->setDisabled(true);
        p_btn_script->setDisabled(true);
    }

    connect(p_chk_script, SIGNAL(clicked(bool)), this, SLOT(onScriptStatusChanged(bool)));



    ++row;


    // ---- setting up the mrst path  -----
    QLabel *lbl_matlab = new QLabel(widget);
    lbl_matlab->setText("MATLAB executable: ");
    layout->addWidget(lbl_matlab, row, 1);

    p_led_matlab = new QLineEdit(widget);
    p_led_matlab->setText(p_res->matlabPath());
    p_led_matlab->setMinimumWidth(300);
    layout->addWidget(p_led_matlab, row, 2);

    p_btn_matlab = new QPushButton("...", widget);
    layout->addWidget(p_btn_matlab, row, 3);
    connect(p_btn_matlab, SIGNAL(clicked()), this, SLOT(browseMatlab()));

    ++row;

    // ---- setting up the keep .mat file  -----
    QLabel *lbl_keep_mat = new QLabel(widget);
    lbl_keep_mat->setText("Keep .mat file: ");
    layout->addWidget(lbl_keep_mat, row, 1);

    p_chk_keep_mat = new QCheckBox(widget);
    p_chk_keep_mat->setChecked(p_res->keepMatFile());
    layout->addWidget(p_chk_keep_mat, row, 2);



    ++row;


    layout_main->addWidget(widget);

    // setting up the buttons
    QWidget *widget_btn = new QWidget(this);
    QHBoxLayout * layout_btn = new QHBoxLayout(widget_btn);
    widget_btn->setLayout(layout_btn);


    layout_btn->addWidget(&m_btn_ok);
    connect(&m_btn_ok, SIGNAL(clicked()), this, SLOT(saveAndClose()));


    layout_btn->addWidget(&m_btn_close);
    connect(&m_btn_close, SIGNAL(clicked()), this, SLOT(close()));


    layout_main->addWidget(widget_btn);

}


//-----------------------------------------------------------------------------------------------
// Saves the current values to the model, and closes the window
//-----------------------------------------------------------------------------------------------
void InspectorReservoir::saveAndClose()
{

    emit sendMsg("Saving reservoir properties to model...");

    // saving the reservoir input file
    p_res->setFile(p_led_res_file->text());

    // saving the mrst path
    p_res->setMrstPath(p_led_mrst->text());

    // saving the matlab path
    p_res->setMatlabPath(p_led_matlab->text());

    // saving the mrst script
    if(p_chk_script->isChecked())
    {
        p_res->setMrstScript(p_led_script->text());
    }
    else
    {
        p_res->setUseMrstScript(false);
    }

    // keep mat file?
    p_res->setKeepMatFile(p_chk_keep_mat->isChecked());




    close();
}


//-----------------------------------------------------------------------------------------------
// opens the file browser for the reservoir input file
//-----------------------------------------------------------------------------------------------
void InspectorReservoir::browseResFile()
{
    QDir dir(p_sim->folder());

    dir.cdUp();



    QString name = QFileDialog::getOpenFileName(this, "Reservoir Input File", dir.absolutePath(), "Reservoir Input File (*.*)");

    p_led_res_file->setText(dir.relativeFilePath(name));
}

//-----------------------------------------------------------------------------------------------
// opens the file browser for the mrst path
//-----------------------------------------------------------------------------------------------
void InspectorReservoir::browseMrst()
{
    QDir dir(p_sim->folder());

    dir.cdUp();




    QString name = QFileDialog::getExistingDirectory(this, "MRST Path", dir.absolutePath());

    p_led_mrst->setText(name);

}

//-----------------------------------------------------------------------------------------------
// opens the file browser for the custom mrst script
//-----------------------------------------------------------------------------------------------
void InspectorReservoir::browseScript()
{
    QDir dir(p_sim->folder());

    dir.cdUp();



    QString name = QFileDialog::getOpenFileName(this, "Custom MRST script", dir.absolutePath(), "Matlab file (*.m)");

    if(name.contains("..") == 0)
    {
        p_led_script->setText(dir.absoluteFilePath(name));
    }

    else p_led_script->setText(dir.relativeFilePath(name));
}


//-----------------------------------------------------------------------------------------------
// opens the file browser for the matlab path
//-----------------------------------------------------------------------------------------------
void InspectorReservoir::browseMatlab()
{
    QDir dir(p_sim->folder());

    dir.cdUp();


    QString name = QFileDialog::getOpenFileName(this, "MATLAB Executable", dir.absolutePath(), "Matlab executable (*)");



    p_led_matlab->setText(name);

}


//-----------------------------------------------------------------------------------------------
// enables/disables the scrip button and line edit
//-----------------------------------------------------------------------------------------------
void InspectorReservoir::onScriptStatusChanged(bool b)
{

    p_led_script->setEnabled(b);
    p_btn_script->setEnabled(b);
}



} // namespace
