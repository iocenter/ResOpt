#include "meoptimizer.h"
#include <iostream>
#include "case.h"
#include "runner.h"
#include <tr1/memory>   //for å vite hva shared pointer
#include "realvariable.h"
#include "intvariable.h"
#include "binaryvariable.h"
#include "model.h"


using std::tr1::shared_ptr;
using std::cout;
using std::endl;

namespace ResOpt
{

MeOptimizer::MeOptimizer(Runner *r)
    : Optimizer(r)
{
}

MeOptimizer::~MeOptimizer()
{
    // nothing to do
}


//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void MeOptimizer::initialize()
{
    // nothing to do here
    // number of cont.variables
    int numb_cvar = runner()->model()->realVariables().size();
    QVector<shared_ptr<RealVariable> > real_var = runner()->model()->realVariables();
    real_var.at(0)->max();
    cout << "Initializing RUN_ONCE..." << endl;
}


//-----------------------------------------------------------------------------------------------
// Starts the optimizer
//-----------------------------------------------------------------------------------------------
void MeOptimizer::start()
{


    cout << "Starting ME..." << endl;

    // creating a case for the current values in the model
    Case *c = new Case(runner()->model());


    // launching the runner on the case
    runCase(c);



    delete c;



    // letting the runner know that the optimization has finished
    emit finished();

    cout << "Optimization finished..." << endl;
}
} //end namespace
