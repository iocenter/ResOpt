#ifndef PIPECONNECTION_H
#define PIPECONNECTION_H

#include <tr1/memory>

class Pipe;
class BinaryVariable;

using std::tr1::shared_ptr;

class PipeConnection
{
private:
    int m_pipe_number;
    Pipe *p_pipe;
    shared_ptr<BinaryVariable> p_routing_variable;


public:
    PipeConnection();
    ~PipeConnection();

    // set functions
    void setPipeNumber(int i) {m_pipe_number = i;}
    void setPipe(Pipe *p) {p_pipe = p;}
    void setVariable(shared_ptr<BinaryVariable> v) {p_routing_variable = v;}

    // get functions
    int pipeNumber() {return m_pipe_number;}
    Pipe* pipe() {return p_pipe;}
    shared_ptr<BinaryVariable> variable() {return p_routing_variable;}

};

#endif // PIPECONNECTION_H
