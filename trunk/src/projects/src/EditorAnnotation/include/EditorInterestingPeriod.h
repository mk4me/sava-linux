#pragma once

#include <vector>

/**
* \class EditorInterestingPeriod
*
* \brief Base class for adding interesting period times from sequences video
*
*/
class EditorInterestingPeriod
{

public:
	/** \brief represents one period time*/
	struct SPeriod
	{
		SPeriod(int begin, int end) : beginFrame(begin), endFrame(end), duration(end-begin+1) {}
		/** \brief when the event starts*/
		int beginFrame;
		/** \brief when the event ends*/
		int endFrame;
		/** \brief how was its duration */
		int duration;
	};

	EditorInterestingPeriod();
	~EditorInterestingPeriod();
	
	/** \brief update periods */
	void update();
	/** \brief get all periods */
	std::vector<SPeriod>& getPeriods() { return m_Periods; }
	/** \brief get next period depends on time */
	SPeriod getNextPeriod(int frameNr);
	/** \brief get prev period depends on time */
	SPeriod getPrevPeriod(int frameNr);
	/** \brief get periond in std::string format*/
	std::string toString();

protected:
	/** \brief add new period */
	void addPeriod(int beginFrame, int endFrame);
	/** \brief must be override to collect all interesting periods */
	virtual void addPeriods() = 0;

private:
	struct PeriodComparator
	{
		bool operator()(const SPeriod& p1, const SPeriod& p2) { 
			return p1.beginFrame < p2.beginFrame; }
	};

	void reduce();

private:
	std::vector<SPeriod> m_Periods;
};