#ifndef USERCONSTRAINT_H
#define USERCONSTRAINT_H

#include <QString>
#include <QStringList>
#include <QList>
#include <tr1/memory>

using std::tr1::shared_ptr;

namespace ResOpt
{

class Model;
class Constraint;

/**
 * @brief Class for user defined constraints
 * @details This class makes it possible for the user to specify a constaint that is not explicitly included by the model through an equation in the driver file.
 *
 */
class UserConstraint
{
public:
    enum OPER {PLUSS, MINUS};


private:
    QString m_expression;                       // the expression for the constraint
    QStringList m_arguments;
    QList<UserConstraint::OPER> m_operators;


    Model *p_model;

    shared_ptr<Constraint> p_constraint;


    double upperBound();
    double lowerBound();

    void generateArguments();

    double resolveArgumentValue(QString arg, bool *ok);

    void error(QString msg);


public:
    UserConstraint(Model *m);
    UserConstraint(const UserConstraint &uc, Model *new_model);

    // misc functions
    bool update();

    void initialize();



    // set functions
    void setExpression(const QString &e) {m_expression = e;}

    // get functions
    const QString& expression() const {return m_expression;}

    shared_ptr<Constraint> constraint() {return p_constraint;}

    Model* model() {return p_model;}


};

} // namespace ResOpt

#endif // USERCONSTRAINT_H
