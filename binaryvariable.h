#ifndef BINARYVARIABLE_H
#define BINARYVARIABLE_H

#include "variable.h"

class BinaryVariable : public Variable
{
private:
    double m_value;
    bool m_is_variable;

public:
    BinaryVariable();

    virtual bool isVariable() {return m_is_variable;}

    // set functions
    void setValue(double v) {m_value = v;}
    void setIsVariable(bool b) {m_is_variable = b;}

    // get functions
    double max() {return 1.0;}
    double min() {return 0.0;}
    double value() {return m_value;}
};

#endif // BINARYVARIABLE_H
