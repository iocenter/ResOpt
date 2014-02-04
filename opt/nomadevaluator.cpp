#include "nomadevaluator.h"

#include <tr1/memory>
#include <iostream>

#include "nomadoptimizer.h"
#include "runner.h"
#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "case.h"

using std::tr1::shared_ptr;
using std::endl;
using std::cout;

namespace ResOpt
{

NomadEvaluator::NomadEvaluator(const NOMAD::Parameters &p, NomadOptimizer *o)
    : NOMAD::Evaluator(p),
      p_optimizer(o)
{
}

//-----------------------------------------------------------------------------------------------
// evaluates the model
//-----------------------------------------------------------------------------------------------
bool NomadEvaluator::eval_x(NOMAD::Eval_Point &x, const NOMAD::Double &h_max, bool &count_eval) const
{
    // generating a case from the evaluation point
    Case *c = generateCase(x);

    // sending the case off for evaluation
    p_optimizer->runCase(c);

    // extracting the objective
    x.set_bb_output(0, -c->objectiveValue());





    // extracting the constraint values
    // the constraints in NOMAD must be on the form: c <= 0

    for(int i = 0; i < c->numberOfConstraints(); ++i)
    {
        double val_input;
        double val = c->constraintValue(i);
        double max = p_optimizer->runner()->model()->constraints().at(i)->max();
        double min = p_optimizer->runner()->model()->constraints().at(i)->min();


        if(val > max) val_input = val - max;

        else if(val < min) val_input = min - val;
        else
        {
            double u_slack = max - val;
            double l_slack = val - min;

            val_input = (u_slack > l_slack) ? -u_slack : -l_slack;
        }

        x.set_bb_output(i+1, val_input);
    }

    // deleting the case from the heap
    //delete c;

    return true;


}


//-----------------------------------------------------------------------------------------------
// generates a case from an Eval_Point
//-----------------------------------------------------------------------------------------------
Case* NomadEvaluator::generateCase(const NOMAD::Eval_Point &x) const
{
    Case *c = new Case();

    int var_num = 0;

    for(int i = 0; i < p_optimizer->runner()->model()->realVariables().size(); ++i)
    {
        c->addRealVariableValue(x[var_num].value());
        ++var_num;
    }

    for(int i = 0; i < p_optimizer->runner()->model()->binaryVariables().size(); ++i)
    {
        c->addBinaryVariableValue(x[var_num].value());
        ++var_num;
    }

    for(int i = 0; i < p_optimizer->runner()->model()->integerVariables().size(); ++i)
    {
        c->addIntegerVariableValue(x[var_num].value());
        ++var_num;
    }



    return c;

}

} // namespace ResOpt
