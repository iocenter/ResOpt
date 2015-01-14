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

## Compilation

To compile ResOpt you need the Qt framework (v5.0.2) and two libraries: Bonmin (v1.5) and NOMAD. The following steps have been tested on Ubuntu.

The steps (and the libs file used by qmake, `resopt_libs.pri`) assume that you use the following folder structure:
```
parent_folder (any name)
|- ResOpt
|- Bonmin-stable
|- nomad
```

### Download the ResOpt code

1. Create a parent directory which will contain the ResOpt code, as well as the libraries it needs.
2. Clone the ResOpt repository by running `git clone git@github.com:iocenter/ResOpt.git`.

### Required packages for compilation
You will need some packages to compile the libraries. Download these by running the following:
```sudo apt-get install git subversion make libblas-dev liblapack-dev \
libmumps-dev gcc g++ gfortran fis-gtm libgl1-mesa-dev libglu1-mesa-dev \
libglew-dev```

### Qt
1. [Download Qt Online Installer](http://qt-project.org/downloads)
2. Navigate to the folder the installer was downloaded to and make it executable by running `chmod +x qt-opensource-linux-x64-VERSION.run`.
3. Start the installer by running `./qt-opensource-linux-x64-VERSION.run`.
4. Select Qt 5.0.2 Tools; unselect newer versions (unless you need them for something else), and install.

### Bonmin
Bonmin needs to be compiled from source. The official guide for installing bonmin is found [here](https://projects.coin-or.org/Bonmin/wiki/GettingStarted) and the guide to get the third party dependencies is found [here](https://projects.coin-or.org/Bonmin/wiki/ThirdParty).

1. Enter the parent directory and download the Bonmin source by running `svn co https://projects.coin-or.org/svn/Bonmin/stable/1.5 Bonmin-stable`.

## Use

## Example
