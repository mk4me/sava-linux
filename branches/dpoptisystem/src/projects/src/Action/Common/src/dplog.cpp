/*****************************************
*  DP Optimization framework             *
*  author: Artur Bak, 2016               *
******************************************/

#include <iostream>
#include <ios>
#include  "dplog.h"
#include "dputils.h"


namespace dp
{

dpLog::dpLog(std::string name)
{
    m_name = name;
    m_logFile = NULL;
    m_resultFile = NULL;

}

/**
*  Initialize log file basing on OPP parameters
*/
void dpLog::initLogFile(std::string params)
{
    std::string logFileName  = dp::oppGetValueForKey("logFile", params);
    if (logFileName != "")
    {
        std::cout << "dpLog::initLogFile " << logFileName << std::endl;
        m_logFile = new std::ofstream();
        m_logFile->open(logFileName.c_str(), std::ios_base::app);
    }
    else
    {
        std::cout << "dpLog::initLogFile ERROR: no logFile specified " << std::endl;
    }
}

/**
*  Initialize result file basing on OPP parameters
*/
void dpLog::initResultFile(std::string params)
{
    std::string resultFileName  = dp::oppGetValueForKey("resultFile", params);
    if (resultFileName != "")
    {
        std::cout << "dpLog::initResultFile " << resultFileName << std::endl;
        m_resultFile = new std::ofstream();
        m_resultFile->open(resultFileName.c_str(), std::ios_base::app);
    }
    else
    {
        std::cout << "dpLog::initResultFile ERROR: no resultFile specified " << std::endl;
    }
}

/**
*  Closes log file
*/
void dpLog::closeLogFile()
{
    if (m_logFile != NULL)
    {
        m_logFile->close();
        delete m_logFile;
    }
}

/**
*  closes result file
*/
void dpLog::closeResultFile()
{
    if (m_resultFile != NULL)
    {
        m_resultFile->close();
        delete m_resultFile;
    }
}
    
/**
* Dump a log without new line to file and on the screen
*/
void dpLog::dbg(std::string text, bool flush)
{
    std::cout << text;
    if (m_logFile != NULL)
    {
        *m_logFile << text;
    }
    
    if (flush)
    {
        m_logFile->flush();
    }
}

/**
* Dump a log with new line to file and on the screen
*/
void dpLog::dbgl(std::string text, bool flush)
{
    dbg(text, false);
    std::cout << std::endl;
    if (m_logFile != NULL)
    {
        *m_logFile << std::endl;
    }
    
        if (flush)
    {
        m_logFile->flush();
    }
}

/**
* Adds opp string to result file
*/
void dpLog::addResult(std::string params, bool flush)
{
    std::cout << params << std::endl;
    if (m_resultFile != NULL)
    {
        *m_resultFile << params << std::endl;
    }
    
    if (flush)
    {
        m_resultFile->flush();
    }
}
    
}
