
#include "evolutionarystrategyoptimizer.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "case.h"
#include "model.h"
#include "runner.h"
#include "realvariable.h"
#include "binaryvariable.h"

namespace ResOpt
{

EvolutionaryStrategyOptimizer::EvolutionaryStrategyOptimizer(Runner *r)
    : Optimizer(r),
      n_parents(2),
      n_children(4),
      mut_range(0.1),
      destab_range(0.5),
      mut_precision(16),
      consider_past(true)
{
}



//-----------------------------------------------------------------------------------------------
// initializes the optimizer
//-----------------------------------------------------------------------------------------------
void EvolutionaryStrategyOptimizer::initialize()
{
}

//-----------------------------------------------------------------------------------------------
// starts the optimization
//-----------------------------------------------------------------------------------------------
void EvolutionaryStrategyOptimizer::start()
{
}

//-----------------------------------------------------------------------------------------------
// starts the optimization
//-----------------------------------------------------------------------------------------------
void EvolutionaryStrategyOptimizer::getChildren()
{
}

//-----------------------------------------------------------------------------------------------
// generates a random set of variable values
//-----------------------------------------------------------------------------------------------
Case* EvolutionaryStrategyOptimizer::randomChild()
{

    Case *c = new Case();

    // creating random values of the real variables
    for(int i = 0; i < runner()->model()->realVariables().size(); ++i)
    {

        double rand_no =  0.0 + 1.0*rand() / RAND_MAX;
        double v = runner()->model()->realVariables().at(i)->min() + rand_no * (runner()->model()->realVariables().at(i)->max() - runner()->model()->realVariables().at(i)->min());

        c->addRealVariableValue(v);
    }

    // creating random values of the binary variables
    for(int i = 0; i < runner()->model()->binaryVariables().size(); ++i)
    {

        double rand_no =  0.0 + 1.0*rand() / RAND_MAX;
        int v = (rand_no > 0.5) ? 1 : 0;

        c->addBinaryVariableValue(v);
    }


    return c;

}

//-----------------------------------------------------------------------------------------------
// mutates a case
//-----------------------------------------------------------------------------------------------
void EvolutionaryStrategyOptimizer::mutate(Case *c, bool destabilize)
{
    double range = (destabilize) ? destab_range : mut_range;

    // mutating the real variables
    for(int i = 0; i < runner()->model()->realVariables().size(); ++i)
    {
        double b_upper = runner()->model()->realVariables().at(i)->max();
        double b_lower = runner()->model()->realVariables().at(i)->min();

        double u = 0.0 + 1.0*rand()/RAND_MAX;
        double s = -1.0 + 2.0*rand()/RAND_MAX;
        double x = range*(b_upper - b_lower);
        double a = pow(2, -u*mut_precision);

        double mut_mult = s*x*a;	//total mutation multiplier

        double var_val = c->realVariableValue(i) + mut_mult;


        //checking that the murated specimin is within the allowed bounds
        if(var_val < b_lower) var_val = b_lower;
        if(var_val > b_upper) var_val = b_upper;

        c->setRealVariableValue(i, var_val);
    }

    // mutating binary variables
    for(int i = 0; i < runner()->model()->binaryVariables().size(); ++i)
    {
        double b_upper = 1;
        double b_lower = 0;

        double u = 0.0 + 1.0*rand()/RAND_MAX;
        double s = -1.0 + 2.0*rand()/RAND_MAX;
        double x = range*(b_upper - b_lower);
        double a = pow(2, -u*mut_precision);

        double mut_mult = s*x*a;	//total mutation multiplier

        double var_val = c->binaryVariableValue(i) + mut_mult;


        c->setBinaryVariableValue(i, (var_val > 0.5) ? 1 : 0);
    }


}

} // namespace ResOpt
