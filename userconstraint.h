#ifndef USERCONSTRAINT_H
#define USERCONSTRAINT_H

#include <QString>

namespace ResOpt
{

class Model;

/**
 * @brief Class for user defined constraints
 * @details This class makes it possible for the user to specify a constaint that is not explicitly included by the model through an equation in the driver file.
 *
 */
class UserConstraint
{
private:
    QString m_expression;   // the expression for the constraint

    Model *p_model;


public:
    UserConstraint(Model *m);

    // misc functions
    bool parseExpression();

    // set functions
    void setExpression(const QString &e) {m_expression = e;}

    // get functions
    const QString& expression() const {return m_expression;}


};

} // namespace ResOpt

#endif // USERCONSTRAINT_H
