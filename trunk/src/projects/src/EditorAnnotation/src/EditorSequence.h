#pragma once

#include "EditorCluster.h"

#include <sequence/Cluster.h>
#include <sequence/Action.h>
#include <sequence/IVideo.h>
#include <QtCore/QRect>

#include <set>


enum EEditorSequenceType
{
	SINGLE,
	MULTI, //culti
};


class EditorSequence
{

	struct CompareClusterById {
		bool operator()(const EditorClusterPtr& a, const EditorClusterPtr& b) {
			return a->getUniqueId() < b->getUniqueId();
		}
	};

public:
	EditorSequence();
	virtual ~EditorSequence() { }

	/// <summary>
	/// Get type of sequence.
	/// </summary>
	/// <returns>succcess??</returns>
	virtual EEditorSequenceType getType() const = 0;

	/// <summary>
	/// Load sequence state.
	/// </summary>
	/// <returns>succcess??</returns>
	virtual bool load() = 0;

	/// <summary>
	/// Save sequence state.
	/// </summary>
	/// <returns>succcess??</returns>
	virtual bool save() = 0;

	/// <summary>
	/// Clears this instance.
	/// </summary>
	virtual void clear();


	/// <summary>
	/// Gets sequence videos paths.
	/// </summary>
	/// <returns>
	/// Current sequence videos paths.
	/// </returns>
	virtual std::vector<std::string> getVideosPaths() const = 0;

	/// <summary>
	/// Sets the sequece videos paths.
	/// </summary>
	/// <param name="paths">The paths.</param>
	virtual void setVideosPaths(const std::vector<std::string>& paths) = 0;

	/// <summary>
	/// Gets count of video frames.
	/// </summary>
	/// <returns>count of frames</returns>
	virtual size_t getNumFrames() const = 0;

	/// <summary>
	/// Gets image from sequence.
	/// </summary>
	/// <param name="i_FrameNum">The number of frame.</param>
	/// <returns>image</returns>
	virtual cv::Mat getFrameImage(size_t i_FrameNum) const = 0;

	/// <summary>
	/// Gets the clusters map.
	/// </summary>
	/// <returns>map of clusters</returns>
	const std::set<EditorClusterPtr, CompareClusterById>& getClusters() const  { return m_Clusters; }

	/// <summary>
	/// Adds the cluster to current video.
	/// </summary>
	/// <param name="i_Id">The cluster identifier.</param>
	/// <param name="i_ClusterPtr">The cluster object.</param>
	/// <param name="i_ActionPtr">The cluster action object.</param>
	virtual void addCluster(const EditorClusterPtr& i_ClusterPtr);

	/// <summary>
	/// Creates the new cluster.
	/// </summary>
	/// <returns>New cluster identifier</returns>
	size_t createCluster();

	/// <summary>
	/// Gets the cluster by its id.
	/// </summary>
	/// <param name="i_Id">The cluster id.</param>
	/// <returns>Cluster object</returns>
	EditorClusterPtr getCluster(int i_Id);

	/// <summary>
	/// Removes the cluster by id.
	/// </summary>
	/// <param name="i_Id">Cluster identifier</param>
	virtual bool removeCluster(int i_Id);

	/// <summary>
	/// Gets the cluster frame.
	/// </summary>
	/// <param name="i_ClusterId">The cluster identifier.</param>
	/// <param name="i_FrameNr">The frame number</param>
	/// <returns>Frame rectangle</returns>
	QRect getClusterFrame(int i_Id, size_t i_FrameNr);

	/// <summary>
	/// Adds the cluster frame.
	/// </summary>
	/// <param name="i_ClusterId">The cluster identifier.</param>
	/// <param name="i_Rect">The cluster rectangle.</param>
	/// <param name="i_FrameNr">The frame number</param>
	void addClusterFrame(int i_Id, const QRect& i_Rect, size_t i_FrameNr);

	/// <summary>
	/// Gets the frames of all cluster by frame number.
	/// </summary>
	/// <param name="i_FrameNr">The frame number</param>
	/// <returns>Vector of pair<cluster identifier, cluster rectangle></returns>
	std::vector<std::pair<size_t, cv::Rect>> getClusterFrames(size_t i_FrameNr);

	/// <summary>
	/// Sets the cluster start frame.
	/// </summary>
	/// <param name="i_ClusterId">The cluster identifier.</param>
	/// <param name="i_StartFrameNr">The start frame number</param>
	virtual bool setClusterStartFrame(size_t i_ClusterId, size_t i_StartFrameNr);

	/// <summary>
	/// Sets the cluster end frame.
	/// </summary>
	/// <param name="i_ClusterId">The cluster identifier.</param>
	/// <param name="i_EndFrameNr">The end frame number</param>
	virtual bool setClusterEndFrame(size_t i_ClusterId, size_t i_EndFrameNr);

	/// <summary>
	/// Gets the cluster files.
	/// </summary>
	/// <param name="i_ClusterId">The i_ cluster identifier.</param>
	/// <returns></returns>
	std::vector<std::string> getClusterFiles(size_t i_ClusterId);


	/// <summary>
	/// Gets the clusters by cluster identifier.
	/// </summary>
	/// <param name="i_ClusterId">The i_ cluster identifier.</param>
	/// <returns></returns>
	std::vector<EditorClusterPtr> getClustersByClusterId(int i_ClusterId) const;

	/// <summary>
	/// Gets sequence nr by frame number.
	/// </summary>
	/// <param name="i_FrameNr">Frame number.</param>
	/// <returns>sequence number</returns>
	virtual int getSequenceNr(size_t frameNr) const { return -1; }

	/// <summary>
	/// Was this sequence modified.
	/// </summary>
	bool wasModified() const { return m_WasModified; }

	/// <summary>
	/// Set this sequence as modified.
	/// </summary>
	void modified() { m_WasModified = true; }

	/// <summary>
	/// Clear modified flag.
	/// </summary>
	void resetModified() { m_WasModified = false; }

protected:
	void inner_addCluster(const EditorClusterPtr& i_ClusterPtr);
	void addClusterFile(size_t clusterId, std::string fileName);

	//object modified flag.
	bool m_WasModified;

	std::set<EditorClusterPtr, CompareClusterById> m_Clusters;
	std::map<size_t, std::vector<std::string>> m_ClustersFiles;
};

typedef std::shared_ptr<EditorSequence> EditorSequencePtr;