#ifndef CASE_H
#define CASE_H

#include <QVector>

namespace ResOpt
{


class Case
{
private:
    QVector<double> m_real_var_values;
    QVector<double> m_binary_var_values;

    QVector<double> m_constraint_values;
    double m_objective_value;

public:
    Case();

    // add functions
    void addRealVariableValue(double v) {m_real_var_values.push_back(v);}
    void addBinaryVariableValue(double v) {m_binary_var_values.push_back(v);}

    void addConstraintValue(double v) {m_constraint_values.push_back(v);}

    // set functions
    void setObjectiveValue(double v) {m_objective_value = v;}

    // get functions
    int numberOfRealVariables() const {return m_real_var_values.size();}
    int numberOfBinaryVariables() const {return m_binary_var_values.size();}
    int numberOfConstraints() const {return m_constraint_values.size();}

    double realVariableValue(int i) const {return m_real_var_values.at(i);}
    double binaryVariableValue(int i) const {return m_binary_var_values.at(i);}
    double constraintValue(int i) const {return m_constraint_values.at(i);}
    double objectiveValue() const {return m_objective_value;}


};

} // namespace ResOpt
#endif // CASE_H
