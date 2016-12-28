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

	static void setOutFileExtension(const std::string& extension) { c_OutFileExt = extension; }

	virtual void reserve() override;
	virtual void process() override;

	virtual bool loadParameters(const ProgramOptions& options) override;

private:
	static const std::string c_InFileExt;
	static std::string c_OutFileExt;
	
	std::unique_ptr<utils::FileLock> m_OutFileLock;
	std::unique_ptr<utils::FileLock> m_InFileLock;
	std::vector<int> m_CurrentIndex;

	FisherVector m_Fv;

	Descriptor();

	std::string getInFileName(const std::vector<int>& indexList) const;
	std::string getOutFileName(const std::vector<int>& indexList) const;

	void reserveRealtime();
	void reserveAll();
	bool reserveFile(const std::vector<int>& fileIndex);

	bool checkVideo(const std::string& videoName) const;
};

#endif // Descriptor_h__
