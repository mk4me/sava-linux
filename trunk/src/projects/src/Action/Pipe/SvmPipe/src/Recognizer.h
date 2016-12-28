#pragma once
#ifndef Recognizer_h__
#define Recognizer_h__

#include "utils/PipeProcess.h"

#include <memory>

struct svm_model;

class Recognizer : public utils::PipeProcess
{
public:
	~Recognizer();

	static utils::IAppModule* create();

	virtual void reserve() override;
	virtual void process() override;
	virtual void finalize() override;

	virtual bool loadParameters(const ProgramOptions& options) override;

private:
	std::unique_ptr<utils::FileLock> m_FileLock;
	std::vector<std::unique_ptr<utils::FileLock>> m_InFileLocks;
	std::vector<int> m_CurrentIndex;
	int m_LastVideoIndex;

	std::vector<std::string> m_Extensions;

	std::vector<svm_model*> m_Model;

	Recognizer();

	void reserveRealtime();
	void reserveAll();

	void finalizeRealTime();
	void finalizeAll();

	void processEmptyActions() const;

	void load();
	int predict(const std::vector<float>& fv) const;
	std::vector<float> loadFV(const std::string& file) const;

	std::string getInFilePattern(const std::vector<int>& indexList) const;
	std::string getOutFileName(const std::vector<int>& indexList) const;
};

#endif // Recognizer_h__
