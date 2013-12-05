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


#include "masterrunner.h"

#include "runner.h"
#include "modelreader.h"
#include "model.h"
#include "reservoir.h"
#include "case.h"

#include <QDir>
#include <iostream>


using std::cout;
using std::endl;

namespace ResOpt
{

MasterRunner::MasterRunner(const QString &driver_file, int parallel_runs, QObject *parent) :
    QObject(parent),
    m_driver_file(driver_file),
    m_parallel_runs(parallel_runs)
{
}

MasterRunner::~MasterRunner()
{
    for(int i = 0; i < m_runners.size(); ++i) m_runners.at(i)->deleteLater();
}


//-----------------------------------------------------------------------------------------------
// initializes the runners etc
//-----------------------------------------------------------------------------------------------
bool MasterRunner::initialize()
{
    cout << "Initializing MasterRunner" << endl;


    // getting the path of the base file
    QFileInfo info(m_driver_file);
    m_path = info.absoluteDir().absolutePath();
    QString driver_file_name = info.fileName();

    //QDir dir(m_path);
    //if(!dir.exists("mr")) dir.mkdir("mr");



    // initializing the runners
    for(int i = 0; i < m_parallel_runs; ++i)
    {
        // create sub-folder
        //QDir dir_i(m_path + "/mr");
        //if(!dir_i.exists(QString::number(i))) dir_i.mkdir(QString::number(i));

        // driver file name for the runnner
        QString driver_file_i = m_path + "/mr/" + QString::number(i) + "/" + driver_file_name;


        // creating runner
        Runner *r = new Runner(driver_file_i);



        cout << "Initializing runner" << endl;

        r->initialize();

        // connecting signals
        connect(r, SIGNAL(runnerFinished(Runner*,Case*)), this, SLOT(onRunnerFinished(Runner*,Case*)));


        m_runners.push_back(r);
        m_runner_states.push_back(false);


    }

}

//-----------------------------------------------------------------------------------------------
// run the optimization
//-----------------------------------------------------------------------------------------------
void MasterRunner::run()
{
    cout << "Running MasterRunner"  << endl;

    if(m_runners.size() != m_parallel_runs) initialize();


    emit optimizationFinished();


}


//-----------------------------------------------------------------------------------------------
// called when a runner finishes
//-----------------------------------------------------------------------------------------------
void MasterRunner::onRunnerFinished(Runner *r, Case *c)
{



}


} // namespace
