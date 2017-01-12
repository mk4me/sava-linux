#pragma once
#ifndef IDetector_h__
#define IDetector_h__

#include <utils/ProgramOptions.h>

#define SAVE_OUTPUT_ENABLED 1

namespace clustering
{
	/// <summary>
	/// interfejs pozwalaj¹ce na tworzenie ró¿nych implementacji algorytmu wykrywania cie¿ek w sekwencji.
	/// </summary>
	class ISequenceDetector
	{
	public:
		virtual ~ISequenceDetector() { }
		virtual bool loadParameters(const ProgramOptions& options) = 0;
		virtual void process(const std::string& inputSequence, const std::string& outputPaths) = 0;

		virtual void show() { }
	};
}

#endif // IDetector_h__