# ResOpt

From the manual:

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

1. Enter the parent directory and download the Bonmin source by running `svn co https://projects.coin-or.org/svn/Bonmin/stable/1.5 Bonmin-stable`.

2. We need to fetch some third-party dependencies to be able to compile Bonmin. In v1.5, some of the links in the `get.*` scripts inside `Bonmin-stable/ThirdParty/*` are dead and need fixed. (The scripts in the more recent v1.7 work, but v1.7 results in compile errors with ResOpt.)
  
  1. **ASL:** The .tgz file `Bonmin-stable/ThirdParty/ASL/get.ASL` wants to download no longer exists, so the files need to be downloaded manually. All the required files are downloaded by running the following command:
  ```
  wget -r -l 1 http://www.netlib.org/ampl/solvers/
  ```
  
    After downloading the files, copy the `solvers` dierctory from the downloaded `www.netlib.org/ampl/` directory into the `Bonmin-stable/ThirdParty/ASL' directory. 

    When the files have been downloaded, the lines in `get.ASL' responslible for downloading, untaring and cleanup need to be commented out. The result shoud look like this:
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

## 2. Use

## 3. Example
