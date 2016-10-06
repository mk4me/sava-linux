#include "EditorInterestingPeriod.h"
#include <sstream>
#include <QtCore/QDebug>

/*--------------------------------------------------------------------*/
EditorInterestingPeriod::EditorInterestingPeriod()
{

}

/*--------------------------------------------------------------------*/
EditorInterestingPeriod::~EditorInterestingPeriod()
{

}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriod::update()
{
	m_Periods.clear();
	addPeriods();
	reduce();
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriod::reduce()
{
	//here I just sort the periods, but it should only reduce duplicats etc.
	std::sort(m_Periods.begin(), m_Periods.end(), PeriodComparator());
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriod::addPeriod(int beginFrame, int endFrame)
{
	SPeriod period(beginFrame, endFrame);
	m_Periods.push_back(period);
}

/*--------------------------------------------------------------------*/
EditorInterestingPeriod::SPeriod EditorInterestingPeriod::getNextPeriod(int frameNr)
{
	SPeriod ret(-1, -1);
	auto upperIt = std::upper_bound(m_Periods.begin(), m_Periods.end(), SPeriod(frameNr,0), PeriodComparator());
	if (upperIt != m_Periods.end())
		ret = *upperIt;

	return ret;	
}

/*--------------------------------------------------------------------*/
EditorInterestingPeriod::SPeriod EditorInterestingPeriod::getPrevPeriod(int frameNr)
{
	SPeriod ret(-1, -1);
	auto lowIt = std::lower_bound(m_Periods.begin(), m_Periods.end(), SPeriod(frameNr, 0), PeriodComparator());
	if (lowIt != m_Periods.end() && lowIt != m_Periods.begin())
		ret = *(lowIt-1);

	return ret;
}


/*--------------------------------------------------------------------*/
std::string EditorInterestingPeriod::toString()
{
	std::stringstream s;
	for (int i = 0; i < m_Periods.size(); i++)
	{
		SPeriod period = m_Periods[i];
		s << (i+1) << ". " << " Start: " << period.beginFrame << " End: " << period.endFrame << " Duration: " <<  period.duration << std::endl;
	}

	return s.str();
}




