#pragma once

#include <sequence/IVideo.h>
#include <sequence/Cluster.h>
#include <sequence/Action.h>
#include <QtCore/QRect>

#include <map>

using namespace sequence;

typedef std::shared_ptr<Cluster> ClusterPtr;
typedef std::shared_ptr<Action> ActionPtr;

class EditorSequence
{

	struct CompareClusterByBeginTime {
		bool operator()(const ClusterPtr& a, const ClusterPtr& b) {
			return a->getStartFrame() < b->getStartFrame();
		}
	};

public:
	EditorSequence();
	~EditorSequence(){};

	/// <summary>
	/// Gets the current video.
	/// </summary>
	/// <returns>std.shared_ptr&lt;_Ty&gt;.</returns>
	std::shared_ptr<IVideo> getVideo() { return m_Video; }

	/// <summary>
	/// Loads the video.
	/// </summary>
	/// <param name="i_VideoPath">The video full path.</param>
	void loadVideo(const std::string& i_VideoPath);

	/// <summary>
	/// Gets the video full path.
	/// </summary>
	/// <returns>std.string.</returns>
	std::string getVideoPath() const { return m_VidePath; }
	 

	/// <summary>
	/// Adds the cluster to current video.
	/// </summary>
	/// <param name="i_Id">The cluster identifier.</param>
	/// <param name="i_ClusterPtr">The cluster object.</param>
	/// <param name="i_ActionPtr">The cluster action object.</param>

	void addCluster(size_t i_Id, ClusterPtr i_ClusterPtr, ActionPtr i_ActionPtr = nullptr);

	/// <summary>
	/// Creates the new cluster.
	/// </summary>
	/// <returns>New cluster identifier</returns>
	size_t createCluster();

	/// <summary>
	/// Gets the clusters map.
	/// </summary>
	/// <returns>map of clusters</returns>
	const std::map<size_t, ClusterPtr>& getClusters() const { return m_ClusterMap; }

	/// <summary>
	/// Gets the cluster by its id.
	/// </summary>
	/// <param name="i_Id">The cluster id.</param>
	/// <returns>Cluster object</returns>
	ClusterPtr getCluster(size_t i_Id);

	/// <summary>
	/// Removes the cluster by id.
	/// </summary>
	/// <param name="i_Id">Cluster identifier</param>
	void removeCluster(size_t i_Id);

	/// <summary>
	/// Gets the cluster frame.
	/// </summary>
	/// <param name="i_ClusterId">The cluster identifier.</param>
	/// <param name="i_FrameNr">The frame number</param>
	/// <returns>Frame rectangle</returns>
	QRect getClusterFrame(size_t i_ClusterId, size_t i_FrameNr);

	/// <summary>
	/// Adds the cluster frame.
	/// </summary>
	/// <param name="i_ClusterId">The cluster identifier.</param>
	/// <param name="i_Rect">The cluster rectangle.</param>
	/// <param name="i_FrameNr">The frame number</param>
	void addClusterFrame(size_t i_ClusterId, QRect& i_Rect, size_t i_FrameNr);

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
	void setClusterStartFrame(size_t i_ClusterId, size_t i_StartFrameNr);

	/// <summary>
	/// Sets the cluster end frame.
	/// </summary>
	/// <param name="i_ClusterId">The cluster identifier.</param>
	/// <param name="i_EndFrameNr">The end frame number</param>
	void setClusterEndFrame(size_t i_ClusterId, size_t i_EndFrameNr);

	/// <summary>
	/// Sets the action to cluster.
	/// </summary>
	/// <param name="i_Id">The cluster identifier.</param>
	/// <param name="i_ActionPtr">The action object</param>
	void setAction(size_t i_Id, ActionPtr i_ActionPtr);

	/// <summary>
	/// Gets all actions.
	/// </summary>
	/// <returns>Map of actions: map<cluster id, action></returns>
	const std::map<size_t, ActionPtr>& getActions() const { return m_ActionMap; }

	/// <summary>
	/// Gets the action by cluster id.
	/// </summary>
	/// <param name="i_Id">The cluste identifier.</param>
	/// <returns>ActionPtr.</returns>
	ActionPtr getAction(size_t i_Id);

	/// <summary>
	/// Clears this instance.
	/// </summary>
	void clear();


private:
	//vidoe file path
	std::string m_VidePath;

	//current video 
	std::shared_ptr<IVideo> m_Video;

	//map of current video clusters (key is file id)
	std::map<size_t, ClusterPtr> m_ClusterMap;

	//map of current video action (key is file id)
	std::map<size_t, ActionPtr> m_ActionMap;
};


typedef std::shared_ptr<EditorSequence> EditorSequencePtr;
