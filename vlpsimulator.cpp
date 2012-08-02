#include "vlpsimulator.h"

#include <QFile>
#include <iostream>

#include "vlptable.h"
#include "model.h"
#include "reservoir.h"

using std::cout;
using std::endl;

namespace ResOpt
{

VlpSimulator::VlpSimulator()
{
}

VlpSimulator::~VlpSimulator()
{
    for(int i = 0; i < m_vlp_tables.size(); ++i) delete m_vlp_tables.at(i);
}


//-----------------------------------------------------------------------------------------------
// Reads a single vlp table from a file
//-----------------------------------------------------------------------------------------------
VlpTable* VlpSimulator::readVlpTable(const QString &file, const QString &well_name)
{
    // opening the input file
    QFile input(file);

    // checking if file opened ok...
    if(!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning("Could not open vlp table file: %s", file.toAscii().data());
        exit(1);
    }


    // starting to read the file

    cout << "Reading vlp table for WELL: " << well_name.toAscii().data() << "..." << endl;


    QStringList list;
    VlpTable *table = new VlpTable();

    bool ok = true;

    while(!input.atEnd() && ok)
    {
        list = processLine(input.readLine());   // reading a line from the output file

        // format: glift pbh qq go gw

        if(list.size() == 5)    // the correct number of elements
        {
            // trying to read the numbers
            bool ok_line = true;
            QList<double> nums;

            for(int i = 0; i < 6; i++)
            {
                nums.push_back(list.at(i).toDouble(&ok_line));
                if(!ok_line) break;
            }

            if(ok_line) // all numbers read ok
            {
                // adding a new row to the vlp table
                table->addRow(nums.at(1), nums.at(0), nums.at(2), nums.at(3), nums.at(4));

            }
            else
            {
                cout << endl << "### Error detected in VLP table input file! ###" << endl
                     << "File: " << file.toAscii().data() << endl
                     << "Could not convert line to numbers..." << endl
                     << "Last line: " << list.join(" ").toAscii().data() << endl;

                exit(1);


            }

        }
        else if(!isEmpty(list))
        {
            cout << endl << "### Error detected in VLP table input file! ###" << endl
                 << "File: " << file.toAscii().data() << endl
                 << "Line does not have the correct number of entries..." << endl
                 << "Last line: " << list.join(" ").toAscii().data() << endl;

            exit(1);

        }


    }

    return table;

}

//-----------------------------------------------------------------------------------------------
// Reads the vlp tables if they have not been read yet
//-----------------------------------------------------------------------------------------------
bool VlpSimulator::generateInputFiles(Model *m)
{
    // reading the reservoir file, generating vlp tables
    QFile input(m->reservoir()->file());

    // checking if file opened ok...
    if(!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning("Could not open reservoir definition file: %s", input.fileName().toAscii().data());
        exit(1);
    }


    // starting to read the file

    QStringList list;

    bool ok = true;

    while(!input.atEnd() && ok)
    {
        // format: wellname filename

        list = processLine(input.readLine());   // reading a line from the input file

        if(list.size() == 2)
        {
            m_vlp_tables.push_back(readVlpTable(list.at(1), list.at(0)));
        }
        else if(!isEmpty(list))
        {
            cout << endl << "### Error detected in reservoir input file! ###" << endl
                 << "Line does not have the correct number of entries..." << endl
                 << "Correct format: wellname filename" << endl
                 << "Last line: " << list.join(" ").toAscii().data() << endl;

            exit(1);

        }

    }

    return true;


}


//-----------------------------------------------------------------------------------------------
// Splits a line read from the driver file into a list of arguments
//-----------------------------------------------------------------------------------------------
QStringList VlpSimulator::processLine(const QString& line)
{

    QString temp = line.trimmed();   // removing trailing and leading whitespaces


    QStringList list = temp.split(QRegExp("\\s+")); // splitting the line at whitespace


    return list;
}


//-----------------------------------------------------------------------------------------------
// Checks if the line is empty or not (white spaces)
//-----------------------------------------------------------------------------------------------
bool VlpSimulator::isEmpty(const QStringList &list)
{
    bool ok = true;

    for(int i = 0; i < list.size(); i++)
    {
        QString temp = list.at(i);

        temp.remove(QRegExp("\\s+"));

        if(temp.size() != 0)
        {
            ok = false;
            break;
        }

    }

    return ok;
}


} // namespace ResOpt

