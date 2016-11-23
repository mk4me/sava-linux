#pragma once

#include "EditorSequence.h"

class EditorMultiSequence: public EditorSequence
{

public:
	EditorMultiSequence();
	virtual ~EditorMultiSequence() { };

	EEditorSequenceType getType() const override { return EEditorSequenceType::MULTI; }

	bool load() override;
	bool save() override;
	void clear() override;

	size_t getNumFrames() const override;
	cv::Mat getFrameImage(size_t i_FrameNum) const override;

	virtual std::vector<std::string> getVideosPaths() const override { return m_VideoPaths; }
	virtual void setVideosPaths(const std::vector<std::string>& paths) override { m_VideoPaths = paths; }

	virtual int getSequenceNr(size_t frameNr) const override;

	virtual bool setClusterStartFrame(size_t i_ClusterId, size_t i_StartFrameNr) override;
	virtual bool setClusterEndFrame(size_t i_ClusterId, size_t i_EndFrameNr) override;

private:
	std::vector<EditorClusterPtr> split(const EditorClusterPtr& cluster, std::vector<size_t>& o_VideoIds) const;
	EditorClusterPtr merge(std::vector<EditorClusterPtr>& clusters, std::vector<size_t>& i_VideoIds) const;

	void fixCluster(EditorClusterPtr i_Cluster);

	void printCluster(const EditorCluster& cluster, const std::string& info = std::string()) const;

private:
	//current video 
	std::vector<std::shared_ptr<IVideo>> m_Videos;

	//video paths
	std::vector<std::string> m_VideoPaths;

	//help vector with frames sizes
	std::vector<size_t> m_VideosFramesSizes;

	//vector with sequences timestamps
	//std::vector<sequence::IVideo::Timestamp> m_FramesTimes;
};

typedef std::shared_ptr<EditorMultiSequence> EditorMultiSequencePtr;
