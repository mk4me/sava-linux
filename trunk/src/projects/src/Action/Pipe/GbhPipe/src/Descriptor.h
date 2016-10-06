#pragma once
#ifndef Descriptor_h__
#define Descriptor_h__

#include "FisherVector.h"

#include <utils/PipeProcess.h>
#include <utils/IAppModule.h>

class Descriptor : public utils::PipeProcess
{
public:
	~Descriptor();

	static utils::IAppModule* create();

	virtual void reserve() override;
	virtual void process() override;

private:
	static const std::string c_InFileExt;
	static const std::string c_OutFileExt;
	
	std::unique_ptr<utils::FileLock> m_FileLock;
	std::vector<int> m_CurrentIndex;

	FisherVector m_Fv;

	Descriptor();

	std::string getInFileName(const std::vector<int>& indexList) const;
	std::string getOutFileName(const std::vector<int>& indexList) const;
};

#endif // Descriptor_h__
