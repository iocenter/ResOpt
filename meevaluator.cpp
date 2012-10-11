#include "meevaluator.h"

#include <tr1/memory>
#include <iostream>

#include "meoptimizer.h"
#include "runner.h"
#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "case.h"
#include "productionwell.h"

using std::tr1::shared_ptr;
using std::endl;
using std::cout;

namespace ResOpt
{

MeEvaluator::MeEvaluator(const NOMAD::Parameters &p, MeOptimizer *o)
    : NOMAD::Evaluator(p),
      p_optimizer(o)
{
}

//-----------------------------------------------------------------------------------------------
// evaluates the model
//-----------------------------------------------------------------------------------------------
bool MeEvaluator::eval_x(NOMAD::Eval_Point &x, const NOMAD::Double &h_max, bool &count_eval) const
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
    delete c;

    return true;


}


//-----------------------------------------------------------------------------------------------
// generates a case from an Eval_Point
//-----------------------------------------------------------------------------------------------
Case* MeEvaluator::generateCase(const NOMAD::Eval_Point &x) const
{
    Case *c = new Case();

    Model *m = p_optimizer->runner()->model();

    // production wells
    for(int i = 0; i < numberOfProductionWells() ; ++i)
    {
        // time steps for each well
        for(int j = 0; j < m->masterSchedule().size(); ++j)
        {
            // adding variable value for time step j to well i
            c->addRealVariableValue(x[j].value());
        }
    }

    // fixed injection wells
    for(int i = 0; i < (m->numberOfWells() - numberOfProductionWells() - 1) ; ++i)
    {
        // time steps for each well
        for(int j = 0; j < m->masterSchedule().size(); ++j)
        {
            // adding variable value for time step j to well i
            c->addRealVariableValue(x[j + m->masterSchedule().size()].value());
        }
    }

   // subsea separator injection wells
            // time steps for each well
            for(int j = 0; j < m->masterSchedule().size(); ++j)
            {
                // adding variable value for time step j to well i
                c->addRealVariableValue(x[j + 2*m->masterSchedule().size()].value());
            }

            // Capacity and efficiency variables
            c->addRealVariableValue(x[3*m->masterSchedule().size()].value());
            c->addRealVariableValue(x[3*m->masterSchedule().size()].value()+1);


            // install time
            c->addIntegerVariableValue(p_optimizer->installTime());


    return c;

}

//-----------------------------------------------------------------------------------------------
// returns the number of production wells in the model
//-----------------------------------------------------------------------------------------------
int MeEvaluator::numberOfProductionWells()
{
    int num_prod = 0;

    Model *m = p_optimizer->runner()->model();

    for(int i = 0; i < m->numberOfWells(); ++i)
    {
        ProductionWell *w = dynamic_cast<ProductionWell*>(m->well(i));

        if(w != 0) num_prod++;
    }

    return num_prod;
}

} // namespace ResOpt
