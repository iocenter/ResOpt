/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2012 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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


/*!@mainpage ResOpt Documentation

\section sec_intro Introduction

This is the documentation for the Reservoir Optimization Framework (ResOpt). ResOpt is a framework for model integration and optimization.
The core functionallity of ResOpt is to connect a reservoir simulator to a simple pipe network, and to do optimization on the resulting model.


\section sec_running Running the framework

ResOpt is run from the command line like this:

@code
ResOpt 'driver file'
@endcode

\section sec_ex Example driver file

@code
!-------------------------------------------------------------------
! Driver file for ResOpt
!-------------------------------------------------------------------

!---------------- Optimization Algorithm ---------------------------
START OPTIMIZER
 TYPE RUNONCE  !BONMIN, RUNONCE
 ITERATIONS 60
 PERTURBATION 0.05
 PARALLELRUNS 1 !integer, or "IDEAL" for the ideal number of parallel runs on this machine
END OPTIMIZER

!----------------- Reservoir Definition ----------------------------
START RESERVOIR
 NAME RES1
 FILE res_spe1.in
 TIME 3650
 PHASES 1 1 1             ! gas oil water
 DENS 0.06054 49.1 64.79  ! gas oil water
END RESERVOIR

!------------------- Master Schedule -------------------------------
START MASTERSCHEDULE
 100
 300
 3650
END MASTERSCHEDULE

!-------------------- Well: prod1 ----------------------------------
START WELL
 NAME prod1
 TYPE P
 GROUP grp1
 BHPLIMIT 100  ! Only used when well is on rate control

 START OUTLETPIPES
! pipe#  fraction
  1      1.0
  2      0.0
 END OUTLETPIPES



 START CONNECTIONS
  299  9410
 END CONNECTIONS

 START SCHEDULE
! t_end  value   max   min     type
  100    2000    2100  1800    BHP !BHP, GAS, OIL, WAT
  300    1950    2100  1800    BHP
  3650   1900    2100  1800    BHP
 END SCHEDULE
END WELL

!-------------------- Well: inj1 -----------------------------------
START WELL
 NAME inj1
 TYPE I
 GROUP grp1
 BHPLIMIT 1000000       ! Only used when well is on rate control
 BHPINJ GAS           ! Only used for injectors, the injected phase when well is on BHP control (WATER, GAS)


 START CONNECTIONS
  0  100000
 END CONNECTIONS

 START SCHEDULE
! t_end  value    max     min     type
  100     5000    8000    1000    BHP
  300     5000    8000    1000    BHP
  3650    5000    8000    1000    BHP
 END SCHEDULE
END WELL


!------------------- Objective definition -------------------------
START OBJECTIVE
 TYPE CUMOIL  !NPV, CUMOIL, CUMGAS

 ! these keywords are only needed for NPV:
 DCF 10
 OILPRICE   100
 GASPRICE     5
 WATERPRICE -10
END OBJECTIVE

!----------------------- Pipe 1 -----------------------------------
START PIPE
 NUMBER 1
 FILE pipe1.dat

 START OUTLETPIPES
! pipe#  fraction
  3      1.0
 END OUTLETPIPES

END PIPE

!----------------------- Pipe 1 -----------------------------------
START PIPE
 NUMBER 2
 FILE pipe2.dat

 START OUTLETPIPES
! pipe#  fraction
  3      1.0
 END OUTLETPIPES

END PIPE

!----------------------- Pipe 2 -----------------------------------
START PIPE
 NUMBER 3
 FILE pipe3.dat

 OUTLETPRESSURE 200
END PIPE

!------------------- Separator: sep1 ------------------------------
START CAPACITY
 NAME cap1

 PIPES 3

 WATER 10000
 GAS   1e6
! OIL   1000
! LIQ   10000

END CAPACITY

EOF

@endcode

*/


#include <QtCore>
#include <QtWidgets/QApplication>
#include <iostream>


#include "runner.h"
#include "gui/mainwindow.h"
#include "par/masterrunner.h"

using namespace ResOpt;
using namespace ResOptGui;
using std::cout;
using std::endl;



int main(int argc, char *argv[])
{

    QCoreApplication *a = 0;
    Runner *r = 0;


    if(argc == 2)
    {

        a = new QCoreApplication(argc, argv);
        r = new Runner(argv[1]);

        QObject::connect(r,SIGNAL(optimizationFinished()), a, SLOT(quit()));
        QTimer::singleShot(0, r, SLOT(run()));
    }
    else if(argc == 1)
    {
        // to launch in GUI mode
/*
        a = new QApplication(argc, argv);

        MainWindow *p_mw = new MainWindow();


        //p_mw->show();

        p_mw->showMaximized();
*/

        // to launch in console mode with default driver file name
/*
        a = new QCoreApplication(argc, argv);
        r = new Runner("/home/aleksaju/Work/postdoc/ResOpt/build-ResOpt-Desktop_Qt_5_0_2_GCC_64bit-Release/eirik_1day/driver.dat");

        QObject::connect(r,SIGNAL(runnerFinished(Runner*, Case*)), a, SLOT(quit()));
        QTimer::singleShot(0, r, SLOT(run()));
*/

        // to launch MasterRunner
//*
        a = new QCoreApplication(argc, argv);


        //r = new Runner("/home/aleksaju/Work/postdoc/ResOpt/build-ResOpt-Desktop_Qt_5_0_2_GCC_64bit-Release/eirik_1day/driver.dat");
        MasterRunner *mr = new MasterRunner("/home/aleksaju/Work/postdoc/ResOpt/build-ResOpt-Desktop_Qt_5_0_2_GCC_64bit-Release/par_test/driver.dat", 2);

        QObject::connect(mr,SIGNAL(optimizationFinished()), a, SLOT(quit()));
        QTimer::singleShot(0, mr, SLOT(run()));
//*/


    }
    else
    {
        cout << "Wrong input arguments!" << endl
             << "Correct usage: .\\ResOpt driver_file" << endl;
        a->exit(1);
    }

    
    return a->exec();
}
