//
//
//  @ Project : Milestone
//  @ File Name : CFPSCounter.h
//  @ Date : 2016-02-05
//  @ Author : Kamil Lebek
//
//


#ifndef _CFPSCOUNTER_H
#define _CFPSCOUNTER_H

#include <chrono>

//! Global utils namespace
namespace utils
{

	//! Class providing simple FPS meter
	class CFPSCounter
	{
	public:

		//! Simple constructor
		CFPSCounter() : m_frameNumber(0), m_timeAccum(0.0f), m_fpsRate(0.0f), m_Updated(false)
		{
			// Setup time of the last call
			m_fLast = std::chrono::high_resolution_clock::now();
		}

		//! Simple destructor
		virtual ~CFPSCounter()
		{
		}

		//! Returns current FPS rate (with UPDATE)
		float operator() ()
		{
			// Frame end
			TimePoint fNow = std::chrono::high_resolution_clock::now();

			// FPS measures
			++m_frameNumber;
			using ms = std::chrono::duration<float, std::milli>;
			m_timeAccum += (std::chrono::duration_cast<ms>(fNow - m_fLast).count() / 1000.0f);
			
			// After 1 second we update
			if (m_timeAccum > 1.0f)
			{
				// Save last FPS rate
				m_Updated = true;

				// Calculate current FPS count
				m_fpsRate = m_frameNumber / m_timeAccum;
				m_timeAccum -= 1.0f;
				m_frameNumber = 0;
			}
			// Otherwise we don't
			else
			{
				m_Updated = false;
			}

			// Set last time marker after counting
			m_fLast = fNow;

			// Return current FPS rate
			return m_fpsRate;
		}

		//! Returns true if FPS rate was updated
		bool IsUpdated()
		{
			return m_Updated;
		}


	private:
		//! Time point definition
		typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> TimePoint;

		//! Actual frame number
		size_t m_frameNumber;

		//! Time accumulated from last recalculation
		float m_timeAccum;

		//! Time point from the last call
		TimePoint m_fLast;

		//! Currently calculated FPS rate
		float m_fpsRate;

		//! Updated flag
		bool m_Updated;

	};
}

#endif // _CFPSCOUNTER_H