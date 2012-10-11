#include "nomadoptimizer.h"

#include <exception>
#include <tr1/memory>

#include "nomad.hpp"
#include "nomadevaluator.h"
#include "runner.h"
#include "model.h"
#include "binaryvariable.h"
#include "realvariable.h"
#include "intvariable.h"
#include "constraint.h"
#include "objective.h"
#include "case.h"

using namespace std;
using std::tr1::shared_ptr;

namespace ResOpt
{

NomadOptimizer::NomadOptimizer(Runner *r)
    : Optimizer(r),
      p_evaluator(0),
      p_param(0),
      p_disp(0)

{
}

NomadOptimizer::~NomadOptimizer()
{
    if(p_evaluator != 0) delete p_evaluator;
    if(p_param != 0) delete p_param;
    if(p_disp != 0) delete p_disp;

}

//-----------------------------------------------------------------------------------------------
// Initializes NOMAD
//-----------------------------------------------------------------------------------------------
void NomadOptimizer::initialize()
{
      // display:
      p_disp = new NOMAD::Display( std::cout );

      p_disp->precision ( NOMAD::DISPLAY_PRECISION_STD );



}

//-----------------------------------------------------------------------------------------------
// Starts the optimization
//-----------------------------------------------------------------------------------------------
void NomadOptimizer::start()
{
    cout << "Starting the NOMAD optimizer..." << endl;

    if(p_disp == 0) initialize();

    try
    {

        // NOMAD initializations:
        NOMAD::begin(0, 0); // hope this works, was: begin(argc, argv)



        // parameters creation:
        generateParameters();

        // custom evaluator creation:
        p_evaluator = new NomadEvaluator(*p_param, this);


        // algorithm creation and execution:
        NOMAD::Mads mads ( *p_param , p_evaluator );
        mads.run();

        // getting the best feasible solution, sending it to the runner if exists
        const NOMAD::Eval_Point *best_feas = mads.get_best_feasible();
        if(best_feas != NULL)
        {
            const NOMAD::Point outputs = best_feas->get_bb_outputs();

            // the variable values
            Case *c = p_evaluator->generateCase(*best_feas);

            // the objective
            c->setObjectiveValue(-outputs[0].value());

            // the constraints
            //for(int i = 1; i < outputs.size(); ++i) c->addConstraintValue(outputs[i].value());

            // sending it to the runner
            sendBestCaseToRunner(c);

            delete c;
        }

    }
    catch(exception &e)
    {
        cerr << "\nNOMAD has been interrupted (" << e.what() << ")\n\n";
    }

    NOMAD::Slave::stop_slaves (*p_disp);
    NOMAD::end();

    emit finished();


}


//-----------------------------------------------------------------------------------------------
// Sets up the parameters object
//-----------------------------------------------------------------------------------------------
void NomadOptimizer::generateParameters()
{

    p_param = new NOMAD::Parameters(*p_disp);

    // finding the number of variables
    int n_var = runner()->model()->realVariables().size() + runner()->model()->binaryVariables().size() + runner()->model()->integerVariables().size();

    p_param->set_DIMENSION (n_var);     // number of variables

    NOMAD::Point sp(n_var);             // starting point
    NOMAD::Point ub(n_var);             // upper bounds
    NOMAD::Point lb(n_var);             // lower bounds


    // setting all the values for each variable

    int var_num = 0;
    for(int i = 0; i < runner()->model()->realVariables().size(); ++i)
    {
        shared_ptr<RealVariable> v = runner()->model()->realVariables().at(i);

        // setting the type
        p_param->set_BB_INPUT_TYPE(var_num, NOMAD::CONTINUOUS);

        // setting the starting point
        sp[var_num] = v->value();

        // setting the upper bound
        ub[var_num] = v->max();

        //setting the lower bound
        lb[var_num] = v->min();

        ++var_num;
    }
    for(int i = 0; i < runner()->model()->binaryVariables().size(); ++i)
    {
        shared_ptr<BinaryVariable> v = runner()->model()->binaryVariables().at(i);

        // setting the type
        p_param->set_BB_INPUT_TYPE(var_num, NOMAD::BINARY);

        // setting the starting point
        sp[var_num] = v->value();

        // setting the upper bound
        ub[var_num] = v->max();

        //setting the lower bound
        lb[var_num] = v->min();

        ++var_num;
    }
    for(int i = 0; i < runner()->model()->integerVariables().size(); ++i)
    {
        shared_ptr<IntVariable> v = runner()->model()->integerVariables().at(i);

        // setting the type
        p_param->set_BB_INPUT_TYPE(var_num, NOMAD::INTEGER);

        // setting the starting point
        sp[var_num] = v->value();

        // setting the upper bound
        ub[var_num] = v->max();

        //setting the lower bound
        lb[var_num] = v->min();

        ++var_num;
    }



    // setting the starting point, upper and lower bounds to the parameters
    p_param->set_X0(sp);
    p_param->set_UPPER_BOUND(ub);
    p_param->set_LOWER_BOUND(lb);


    // setting the objective and constraint info
    int n_con = runner()->model()->constraints().size();

    vector<NOMAD::bb_output_type> bbot (n_con + 1); // definition of output types
    bbot[0] = NOMAD::OBJ;       // the first is the objective
    for(int i = 1; i < n_con + 1; ++i) bbot[i] = NOMAD::PB;     // the rest are constraints

    p_param->set_BB_OUTPUT_TYPE ( bbot );

    // not sure what this does...
    p_param->set_DISPLAY_STATS ( "bbe ( sol ) obj" );

    // setting the maximum number of iterations
    p_param->set_MAX_BB_EVAL(maxIterations());

    p_param->read("nomad_param.dat");


    // parameters validation:
    p_param->check();

    cout << "NOMAD Parameters:" << endl;
    cout << *p_param << endl;



}

} // namespace ResOpt
