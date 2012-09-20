#include "meoptimizer.h"
#include <iostream>
#include "case.h"
#include "runner.h"


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
