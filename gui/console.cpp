#include "console.h"


namespace ResOptGui
{

Console::Console(QWidget *parent) :
    QDockWidget(parent),
    m_edit(this)
{

    setWidget(&m_edit);

    m_edit.setReadOnly(true);


}

//-----------------------------------------------------------------------------------------------
// prints a message to the console
//-----------------------------------------------------------------------------------------------
void Console::printMsg(QString msg)
{
    m_edit.append(msg);
}


} // namespace
