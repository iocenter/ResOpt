#include "ipoptoptimizer.h"
#include "ipoptinterface.h"
#include "runner.h"
#include "model.h"
#include "reservoirsimulator.h"

#include <iostream>
#include <QString>

using std::cout;
using std::endl;

namespace ResOpt
{

IpoptOptimizer::IpoptOptimizer(Runner *r)
    : Optimizer(r)
{}

IpoptOptimizer::~IpoptOptimizer()
{
    // nothing to do, the SmartPtr will delete itself
}

//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void IpoptOptimizer::initialize()
{

    cout << "Initializing Ipopt..." << endl;

    // Checking that the problem don't have any binary or integer variables
    if (runner()->model()->binaryVariables().size() > 0
            || runner()->model()->integerVariables().size() > 0)
    {
        cout << endl << "### Runtime Error ###" << endl
             << "Ipopt cannot solve problems with binary or integer variables... " << endl
             << "Please choose another solver." << endl << endl;
        exit(1);
    }

    // setting up the TNLP
    p_tnlp = new IpoptInterface(this);

    // Create an instance of the IpoptApplication
    // We are using the factory, since this allows us to compile this
    // example with an Ipopt Windows DLL
    app = IpoptApplicationFactory();

    // Change some options
    // Note: The following choices are only examples, they might not be
    //       suitable for your optimization problem.
    //app->Options()->SetNumericValue("tol", 1e-5);
    //app->Options()->SetStringValue("mu_strategy", "adaptive");
    //m_bonmin.readOptionsString("output_file " + runner()->reservoirSimulator()->folder().toStdString() + "/ipopt.out\n");
    app->Options()->SetStringValue("output_file", runner()->reservoirSimulator()->folder().toStdString() + "/ipopt.out");

    // Solver options
    app->Options()->SetIntegerValue("max_iter", maxIterations());

    // Derivative checks
    //app->Options()->SetStringValue("check_derivatives_for_naninf", "yes"); // no (default) or yes (may produce a lot of output)
    //app->Options()->SetStringValue("derivative_test", "first-order"); // none (default)

    // reading options file
    //runner()->reservoirSimulator()->folder().toStdString() + "/ipopt.opt"
    // Quasi-Newton approximation of Hessian of Lagrangian
    app->Options()->SetStringValue("hessian_approximation", "limited-memory"); // exact (default, no approx) or limited-memory (quasi-Newton)
    //app->Options()->SetStringValue("jac_c_constant", "no");
    //app->Options()->SetStringValue("jac_d_constant", "no");

    //app->Options()->SetStringValue("limited_memory_update_type", "bfgs"); // BFGS (default) or SR1 (not working well)

    // Initialize the IpoptApplication and process the options
    ApplicationReturnStatus status;
    app->Initialize();


    if (status != Solve_Succeeded)
    {
        std::cout << std::endl << std::endl << "*** Error during initialization!" << std::endl;
        //return (int) status;
        exit(1);
    }
    else
    {
        // changing the status to initialized
        setInitialized(true);
    }

}

//-----------------------------------------------------------------------------------------------
// Starts the optimization
//-----------------------------------------------------------------------------------------------
void IpoptOptimizer::start()
{

    // checking if everything is initialized
    if(!isInitialized()) initialize();

    //Set up done, now let's run Ipopt
    ApplicationReturnStatus status;
    status = app->OptimizeTNLP(GetRawPtr(p_tnlp));

    if (status == Solve_Succeeded)
    {
        std::cout << std::endl << std::endl << "*** The problem solved!" << std::endl;


    }
    else
    {
        std::cout << std::endl << std::endl << "*** The problem FAILED!" << std::endl;
    }

    // As the SmartPtrs go out of scope, the reference count
    // will be decremented and the objects will automatically
    // be deleted.
    //return (int) status;

    // letting the runner know the optimization has finished
    emit finished();
}

//-----------------------------------------------------------------------------------------------
// generates a description for driver file
//-----------------------------------------------------------------------------------------------
QString IpoptOptimizer::description() const
{
    QString str("START OPTIMIZER\n");
    str.append(" TYPE IPOPT \n");
    str.append(" ITERATIONS " + QString::number(maxIterations()) + "\n");
    str.append(" PERTURBATION " + QString::number(pertrurbationSize()) + "\n");
    str.append(" PARALLELRUNS " + QString::number(parallelRuns()) + "\n");
    str.append("END OPTIMIZER\n\n");
    return str;
}


} // namespace ResOpt

