#pragma once

#include <sequence/Cluster.h>
#include <sequence/Action.h>
#include <sequence/IVideo.h>

#include "EditorAction.h"


using namespace sequence;

class EditorCluster;
typedef std::shared_ptr<Cluster> ClusterPtr;
typedef std::shared_ptr<EditorCluster> EditorClusterPtr;


/// <summary>
/// Implementacja pojedynczego klastra.
/// </summary>
class EditorCluster
{
	public:
		EditorCluster(const ClusterPtr& cluster);
		EditorCluster(int clusterId = Cluster::INVALID_CLUSTER);
		EditorCluster(const std::string& filepath);
		EditorCluster(const EditorCluster& cluster);

		void setUniqueId(int id) { m_InnerId = id; }
		int getUniqueId() const { return m_InnerId; }

		ClusterPtr getSource() const { return m_Source; }
		const std::vector<EditorActionPtr>& getActions() const { return m_Actions; };

		size_t getLength() const;

		EditorActionPtr getAction(size_t index = 0) const;
		size_t getActionsCount() const { return m_Actions.size(); }
		void addAction(const EditorActionPtr& action);
		void removeAction(const EditorActionPtr& action);

		bool setActionBeginTime(const EditorActionPtr& action, size_t frame_nr);
		bool setActionEndTime(const EditorActionPtr& action, size_t frame_nr);
		bool splitActions(size_t frame_nr);

		void setHasPrevContinuation(bool val)  { m_HasPrevContinuation = val; }
		void setHasNextContinuation(bool val)  { m_HasNextContinuation = val; }
		bool hasPrevContinuation() const { return m_HasPrevContinuation; }
		bool hasNextContinuation() const { return m_HasNextContinuation; }

		bool isEditable() const;

		EditorActionPtr getActionByTime(size_t time);

		void update();

	private:
		int m_InnerId;
		ClusterPtr m_Source;
		std::vector<EditorActionPtr> m_Actions;

		bool m_HasPrevContinuation;
		bool m_HasNextContinuation;
};

