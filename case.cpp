#include "case.h"
#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"

namespace ResOpt
{

Case::Case()
{
}

Case::Case(Model *m)
{
    // adding real variables
    for(int i = 0; i < m->realVariables().size(); ++i)
    {
        addRealVariableValue(m->realVariables().at(i)->value());
    }

    // adding binary variables
    for(int i = 0; i < m->binaryVariables().size(); ++i)
    {
        addBinaryVariableValue(m->binaryVariables().at(i)->value());
    }


}

Case::Case(const Case &c, bool cpy_output)
{
    for(int i = 0; i < c.numberOfRealVariables(); ++i)
    {
        m_real_var_values.push_back(c.m_real_var_values.at(i));
    }

    for(int i = 0; i < c.numberOfBinaryVariables(); ++i)
    {
        m_binary_var_values.push_back(c.m_binary_var_values.at(i));
    }

    if(cpy_output)  // copy the constraints and objective too
    {
        for(int i = 0; i < c.numberOfConstraints(); ++i)
        {
            m_constraint_values.push_back(c.m_constraint_values.at(i));
        }

        m_objective_value = c.m_objective_value;


    }

}

} // namespace ResOpt
