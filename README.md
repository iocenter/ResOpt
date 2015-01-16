# ResOpt

From the ResOpt User Guide:

> ResOpt is an optimization framework for integrated production networks in the petroleum industry. The framework makes it easy to connect a reservoir simulator to a production network consisting of pipe lines, separators, pumps, and compressors. The integrated reservoir-network model can be optimized with respect to:

> * Well controls.
> * Well and pipe line routing in the gathering network.
> * Investment planning.
> * Well locations in the reservoir model.

> A suite of optimization algorithms are supported. Some of the algorithms are generic, while others are custom-built to solve production optimization problems.

ResOpt is an open source framework, and is released under the terms of the GNU General Public License (GPL).

ResOpt was created by Alexander Oma Juell.

## 1. Compilation

To compile ResOpt you need the Qt framework (v5.0.2) and two libraries: Bonmin (v1.5) and NOMAD. You will also need to install some packages available using `apt-get`. The following steps have been tested on Ubuntu.

The steps (and the libs file used by qmake, `resopt_libs.pri`) assume that you use the following folder structure:
```
parent_folder (any name)
|- ResOpt
|- Bonmin-stable
|- nomad
```

### 1.2. Download the ResOpt code

1. Create a parent directory which will contain the ResOpt code, as well as the libraries it needs.
2. Clone the ResOpt repository by running
```
git clone git@github.com:iocenter/ResOpt.git
```

### 1.3. Required packages for compilation
You will need some packages to compile the libraries. Download these by running the following:
```
sudo apt-get install git subversion make libblas-dev liblapack-dev \
libmumps-dev gcc g++ gfortran fis-gtm libgl1-mesa-dev libglu1-mesa-dev \
libglew-dev
```

### 1.4. Install Qt
1. [Download Qt Online Installer](http://qt-project.org/downloads)
2. Navigate to the folder the installer was downloaded to and make it executable by running
  ```
  chmod +x qt-opensource-linux-x64-VERSION.run
  ```
3. Start the installer by running
  ```
  ./qt-opensource-linux-x64-VERSION.run
  ```
4. Select Qt 5.0.2 Tools; unselect newer versions (unless you need them for something else), and install.

### 1.5. Get the Bonmin library
Bonmin needs to be compiled from source. The official guide for installing bonmin is found [here](https://projects.coin-or.org/Bonmin/wiki/GettingStarted) and the guide to get the third party dependencies is found [here](https://projects.coin-or.org/Bonmin/wiki/ThirdParty).

1. Enter the parent directory and download the Bonmin source by running
  ```
  svn co https://projects.coin-or.org/svn/Bonmin/stable/1.5 Bonmin-stable
  ```

2. We need to fetch some third-party dependencies to be able to compile Bonmin. In v1.5, some of the links in the `get.*` scripts inside `Bonmin-stable/ThirdParty/*` are dead and need to be fixed. (The scripts in the more recent v1.7 work, but v1.7 results in compile errors with ResOpt.)

  1. **ASL:** The .tgz file `Bonmin-stable/ThirdParty/ASL/get.ASL` wants to download no longer exists, so the files need to be downloaded manually. All the required files are downloaded by running the following command:
  ```
  wget -r -l 1 http://www.netlib.org/ampl/solvers/
  ```

    After downloading the files, copy the `solvers` dierctory from the downloaded `www.netlib.org/ampl/` directory into the `Bonmin-stable/ThirdParty/ASL` directory.

    When the files have been downloaded, the lines in `get.ASL` responslible for downloading, untaring and cleanup need to be commented out. The result shoud look like this:
    ```
#!/bin/sh

set -e

wgetcmd=wget
wgetcount=`which wget 2>/dev/null | wc -l`
if test ! $wgetcount = 1; then
  echo "Utility wget not found in your PATH."
  if test `uname` = Darwin; then
    wgetcmd=ftp
    echo "Using ftp command instead."
  else
    exit -1
  fi
fi

echo " "
echo "Running script for downloading the source code for the ASL"
echo " "

#rm -f solvers.tar
#echo "Downloading the source code from www.netlib.org..."
#$wgetcmd http://www.netlib.org/ampl/solvers.tar

#rm -rf solvers

#echo "Unpacking the source code..."
#tar xf solvers.tar

#echo "Uncompressing the source files..."
#gunzip -fr solvers

echo "Adding No_dtoa to CFLAGS..."
cd solvers
mv makefile.u makefile.u.orig
sed -e 's/CFLAGS = /CFLAGS = -DNo_dtoa /g' makefile.u.orig > makefile.u
cd ..

#echo "Deleting the tar file..."
#rm solvers.tar

echo " "
echo "Done downloading the source code for ASL."
echo " "
    ```

  2. **Blas:** The url in `Bonmin-stable/ThirdParty/Blas/get.Blas` is dead, and needs to be fixed. Simply change the address `ftp://www.netlib.org/blas/blas.tgz` in the script to `http://www.netlib.org/blas/blas.tgz`.

  3. **Lapack:** The same goes for get.Lapack. Change the address `ftp://www.netlib.org/lapack/lapack-${lapack_ver}.tgz`  to `http://www.netlib.org/lapack/lapack-${lapack_ver}.tgz`.

3. We now download the required third-party code by executing
  ```
  cd Bonmin-stable/ThirdParty/ASL/
  ./get.ASL
  cd ../Blas
  ./get.Blas
  cd ../Lapack
  ./get.Lapack
  cd ../Mumps
  ./get.Mumps
  ```

4. We now create a new directory called `build` inside the `Bonmin-stable` directory and enter it. We then configure, compile, test and install Bonmin:
  ```
  cd Bonmin-stable
  mkdir build
  cd build
  ../configure -C
  make
  make test
  make install
  ```

### 1.6. Get the Nomad library
1. Download NOMAD [here](https://www.gerad.ca/nomad/PHP_Forms/Download.php).
2. Extract bonmin into the parent directory..
3. Enter the install directory and run the install script:
```
cd nomad/install
./install.sh
```

### 1.7. Open and Compile ResOpt in Qt Creator
1. Open the ResOpt project file, `ResOpt/ResOpt.pro` in Qt Creator. Make sure `Desktop Qt 5.0.2 GCC 64bit` is selected when configuring.

2. Make sure the paths in `resopt_libs.pri` correspond to your own folder structure. If it does not, edit it.

3. You should now be able to compile ResOpt.

## 2. Third-party dependencies
ResOpt requires some third-party applications to perform optimizations. Mainly, it requires a reservoir simulator. The supported simulators are MRST (developed by SINTEF) and GPRS (developed at Stanford). The preferred simulator is MRST.

### 2.1. MRST
MRST, or MATLAB Reservoir Simulation Toolbox, can be downloaded [here](http://www.sintef.no/Projectweb/MRST/Downloadable-Resources/Download/). Simply extract the archive to some convenient place. The MRST keyword in the ResOpt driver file will need to point to this location. In the example driver files, the keyword is set as follows:
```
MRST ~/Documents/MATLAB/mrst-2014b/
```

### 2.2. MATLAB
MATLAB from MathWorks must be installed in order to use MRST as the reservoir simulator in ResOpt. The location of the MATLAB executable must be set in the driver file when running MATLAB with MRST. The default in the supported operating systems is as follows:

```
MATLAB /usr/local/MATLAB/R2014b/bin/matlab         ! Linux
MATLAB /Applications/MATLAB_R2014b.app/bin/matlab  ! Mac OS X
MATLAB matlab                                      ! Windows
```

## 3. Running ResOpt
ResOpt requires a driver file to run. A suite of examples is found in the `examples` folder.

### 3.1. Example with MRST
**Location**: `/examples/MRST/simple`
**Requirements**: MRST and MATLAB

This example consists of a single production well, producing through a 500 meter long vertical wellbore. Cumulative oil production is maximized by regressing on the flowing bottomhole pressure in the well.

The example consists of four files:
* `driver.dat`:
* `SIMPLE10x5x10.txt`: Description of the reservoir model. The model is described using the same syntax as Schlumberger's ECLIPSE100.
* `pipe.dat`: Description of the pipe.
* `initStateADI.m`: Set the initial state of the reservoir.

#### 3.1.2. Before running
Make sure the paths for MRST and MATLAB are set correctly in the `driver.dat` file. By default they are set as follows:
```
!----------------- Reservoir Definition ----------------------------
START RESERVOIR
 NAME RES1
 FILE SIMPLE10x5x10.txt
 MRST ~/Documents/MATLAB/mrst-2014b/
 MATLAB /usr/local/MATLAB/R2014a/bin/matlab
 TIME 365.0
 PHASES 0 1 1             ! gas oil water
 DENS 0.06054 49.1 64.79  ! gas oil water
END RESERVOIR
```

## 4. Driver file
A ResOpt model is created through a driver file. This section will explain all keywords that go into a driver file. `/examples/MRST/simple/driver.dat` will be used for examples.


##### driver.dat
```
!-------------------------------------------------------------------
! Driver file for ResOpt
!-------------------------------------------------------------------
! ALL PRICES SCALED 1e-6
ADJOINTS_COUPLED MODEL ! this _must_ be the first keyword, either COUPLED or DECOUPLED

SIMULATOR MRST_BATCH ! GPRS VLP MRST_BATCH

DEBUG debug.dat


!---------------- Optimization Algorithm ---------------------------
START OPTIMIZER
 TYPE IPOPT !BONMIN, RUNONCE, NOMAD
 ITERATIONS 10000
 CONT_ITER 500
 PERTURBATION 0.0001   ! O.O5
 PARALLELRUNS 1 !integer, or "IDEAL" for the ideal number of parallel runs on this machine
 TERMINATION 0.7 5
END OPTIMIZER


!------------------- Objective definition -------------------------
START OBJECTIVE
 TYPE CUMOIL  !NPV, CUMOIL, CUMGAS

 ! these keywords are only needed for NPV:
 DCF 0.10
 OILPRICE   1e-4
 GASPRICE     5e-6
 WATERPRICE -1e-5
END OBJECTIVE


!------------------- Master Schedule -------------------------------
START MASTERSCHEDULE
 10
 50
 100
 200
 365
END MASTERSCHEDULE


!----------------- Reservoir Definition ----------------------------
START RESERVOIR
 NAME RES1
 FILE SIMPLE10x5x10.txt
 MRST ~/Documents/MATLAB/mrst-2014b/
 MATLAB /usr/local/MATLAB/R2014a/bin/matlab
 TIME 365.0
 PHASES 0 1 1             ! gas oil water
 DENS 0.06054 49.1 64.79  ! gas oil water
END RESERVOIR


!-------------------- Well: p1 ----------------------------------
START WELL
 NAME p1
 TYPE P
 GROUP grp1
 BHPLIMIT 1  ! Only used when well is on rate control

 START OUTLETPIPES
! pipe#  fraction
  1 1.0
 END OUTLETPIPES


 START CONNECTIONS
  1  1  1  5  100   ! Original 1 1 1 5 100
 END CONNECTIONS

 START SCHEDULE
! t_end value   max min type
 10 300 350 50  BHP
 50 300 350 50  BHP
 100    300 350 50  BHP
 200    300 350 50  BHP
 365    300 350 50  BHP
 END SCHEDULE

END WELL


!----------------------- Pipe 1 -----------------------------------
START PIPE
 NUMBER 1
 FILE pipe.dat

 OUTLETPRESSURE 20 BARA

END PIPE

EOF
```

##### SIMPLE10x5x10.txt
```
RUNSPEC
TITLE
   SIMPLE 2-PHASE PROBLEM

DIMENS
   20   10    6  /

EQLDIMS
    1  100   10    1    1 /

TABDIMS
    1    1   16   12    1   12 /

WELLDIMS
    3    1    1    2 /

START
  1 'JAN' 2013  /

UNIFIN

GRID    ================================================================
INIT
--     ARRAY  VALUE     ------- BOX ------
EQUALS
      'PORO'     0.3    /
      'PERMX'   50      /
      'PERMY'   50      /
      'PERMZ'    25      /
      'TOPS'  1000      /
      'DX'      60      /
      'DY'      60      /
      'DZ'      3      1 20 1 10 1 6/
/

PROPS    ===============================================================
SWOF
    0.1500       0.0    1.0000         0.0
    0.2000    0.0059    0.8521         0.0
    0.2500    0.0237    0.7160         0.0
    0.3000    0.0533    0.5917         0.0
    0.3500    0.0947    0.4793         0.0
    0.4000    0.1479    0.3787         0.0
    0.4500    0.2130    0.2899         0.0
    0.5000    0.2899    0.2130         0.0
    0.5500    0.3787    0.1479         0.0
    0.6000    0.4793    0.0947         0.0
    0.6500    0.5917    0.0533         0.0
    0.7000    0.7160    0.0237         0.0
    0.7500    0.8521    0.0059         0.0
    0.8000    1.0000       0.0         0.0
/

/Users/eirikhaug/Dropbox/Master Oppgave/case29/SIMPLE10x5x10.txt
DENSITY
--   O       W     G
    962    1080   0.82   /


PVTW
--    REF.PRES.  REF. FVF  COMPRESSIBILITY  REF.VISC.  VISCOSIBILITY
       234        1.012        4.28e-5        0.48       0.00E+00 /

PVCDO
-- REF.PRES.   FVF   COMPRESSIBILITY  REF.VISC.  VISCOSIBILITY
   234         1.065    6.65e-5         5.0     3.8e-3   /

ROCK
-- REF.PRES   COMPRESSIBILITY
   234          3.0e-5       /


SOLUTION ===============================================================
EQUIL
--depth  pressure   woc
   1000      200   1020   /

SUMMARY  ===============================================================
FOPR
WBHP
/

SCHEDULE ===============================================================

INCLUDE
 schedule.inc /
END     ================================================================
```

##### pipe.dat
```
!-------------------------------------------------------------------
! Pressure drop file for Pipe 1
!-------------------------------------------------------------------
! The following correlations are supported:
!
! ALIAS     FULL NAME
! -----     ---------
!
! BB73      Beggs and Brill, 1973, "A Study of Two-Phase Flow in Inclined Pipes"
!
!-------------------------------------------------------------------

CORRELATION BB73

DIAMETER 0.12       ! m
LENGTH   500        ! m. Distanse ned til brønn (ca)
ANGLE    90     ! degrees
TEMPERATURE 60  ! C

GASGRAVITY   0.65   ! specific gravity (air = 1)
OILDENSITY   700    ! kg / m^3
WATERDENSITY 1000   ! kg / m^3

OILVISCOSITY    2.0 ! cp
WATERVISCOSITY  0.9 ! cp

EOF
```

##### initStateADI.m
```
function state = initStateADI(G, f, deck)
eql      = deck.SOLUTION.EQUIL;
datDepth = eql(1);
datPres  = eql(2);
woc      = eql(3);

g = norm(gravity);

% approx by linear pressure drop/linear bo from datDepth - woc
pwoc = ADI(datPres, 1);
nRes = inf;
rhoDat = f.rhoOS./f.BO(datPres);
while nRes/norm(pwoc.val) > 1e-12
    rho    = f.rhoOS./f.BO(pwoc);
    eq = pwoc - datPres - g*(woc-datDepth)*(rho+rhoDat)/2;
    dp = -eq.val./eq.jac{1};
    pwoc  = ADI(pwoc.val+dp, 1);
    nRes = norm(eq.val);
end

% approx by linear pressure drop/linear bw from woc - bottom
bottom = max(G.cells.centroids(:,3));
pbottom = ADI(pwoc.val, 1);
nRes = inf;
rhowoc = f.rhoWS./f.BW(pwoc.val);
while nRes/norm(pbottom.val) > 1e-12
    rho    = f.rhoWS./f.BW(pbottom);
    eq = pbottom - pwoc.val - g*(bottom-woc)*(rho+rhowoc)/2;
    dp = -eq.val./eq.jac{1};
    pbottom  = ADI(pbottom.val+dp, 1);
    nRes = norm(eq.val);
end

tab = [datDepth datPres; woc pwoc.val; bottom pbottom.val];
state.pressure = interp1q(tab(:,1), tab(:,2), G.cells.centroids(:,3));

%--------------------------------------------------------------------------

tab = deck.PROPS.SWOF{1};
maxs = tab(end,1);
mins = tab(1,1);

sw = ones(G.cells.num, 1)*mins;
sw(G.cells.centroids(:,3) > woc) = maxs;

state.s = [sw 1-sw];
state.flux = zeros(G.faces.num, 1);
```
