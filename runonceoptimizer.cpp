#include "runonceoptimizer.h"

#include <iostream>
#include "runner.h"

using std::cout;
using std::endl;


RunonceOptimizer::RunonceOptimizer(Runner *r)
    : Optimizer(r)
{
}


//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void RunonceOptimizer::initialize()
{
    // nothing to do here

    cout << "Initializing RUN_ONCE..." << endl;
}


//-----------------------------------------------------------------------------------------------
// Starts the optimizer
//-----------------------------------------------------------------------------------------------
void RunonceOptimizer::start()
{
    cout << "Starting RUN_ONCE..." << endl;

    // launching the runner for one iteration
    runner()->evaluate();
}
