#pragma once
#include <string>
#include <boost/lexical_cast.hpp>
#include <utils/ProgramOptions.h>

class Console
{
    public:
    int id;

    ProgramOptions options;

    Console( int id ) : id( id ), options( "CalibrationConsole.exe" )
    {
        options.addPositional<int>(
            "consoleNo",
            "console number (equals to " + boost::lexical_cast<std::string, int>( id ) + ")" );
    }

    virtual const std::string description()
    {
        return std::string();
    }

    virtual const std::string example()
    {
        return std::string();
    }

    virtual void parse( int argc, const char* argv[] ) {}

    virtual void operator()() {};
};