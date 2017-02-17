#include "EditorCluster.h"


EditorCluster::EditorCluster(const ClusterPtr& cluster)
	: m_Source(cluster)
	, m_HasPrevContinuation(false)
	, m_HasNextContinuation(false)
	, m_InnerId(-1)
{
	//create first action
	EditorActionPtr action = std::make_shared<EditorAction>();
	action->setStartFrame(m_Source->getStartFrame());
	action->setEndFrame(m_Source->getEndFrame());
	addAction(action);
}

EditorCluster::EditorCluster(int clusterId /*= Cluster::INVALID_CLUSTER*/)
	:EditorCluster(std::make_shared<Cluster>(clusterId))
{ 

}

EditorCluster::EditorCluster(const std::string& filepath)
	: EditorCluster(std::make_shared<Cluster>(filepath))
{

}

EditorCluster::EditorCluster(const EditorCluster& cluster)
{
	m_Source = std::make_shared<Cluster>(cluster.getSource()->getClusterId());
	m_Source->setFrameSize(cluster.getSource()->getFrameSize());
	m_Source->setFramesPositions(cluster.getSource()->getFramesPositions());

	m_InnerId = cluster.getUniqueId();
	m_HasPrevContinuation = cluster.hasPrevContinuation();
	m_HasNextContinuation = cluster.hasNextContinuation();

	for (auto action : cluster.getActions())
		addAction(std::make_shared<EditorAction>(*action));
}

size_t EditorCluster::getLength() const
{
	if (m_Source)
		return m_Source->getEndFrame() - m_Source->getStartFrame() + 1;

	return 0;
}

EditorActionPtr EditorCluster::getAction(size_t index) const
{
	if (index < m_Actions.size())
		return m_Actions[index];

	return nullptr;
}

void EditorCluster::addAction(const EditorActionPtr& action)
{
	auto it = std::find_if(m_Actions.begin(), m_Actions.end(), [action](const EditorActionPtr& p) { return p->getSource() == action->getSource(); });
	if (it == m_Actions.end())
		m_Actions.push_back(action);
}

void EditorCluster::removeAction(const EditorActionPtr& action)
{
	//przynajmniej jedna akcja musi pozostac
	if (m_Actions.size() < 2)
		return;

	auto it = std::find_if(m_Actions.begin(), m_Actions.end(), [action](const EditorActionPtr& p) { return p->getSource() == action->getSource(); });
	if (it != m_Actions.end())
	{
		m_Actions.erase(it);
		update();
	}
}


bool EditorCluster::setActionBeginTime(const EditorActionPtr& action, size_t frame_nr)
{
	if (m_Actions.size() == 1)
		return false;

	auto it = std::find(m_Actions.begin(), m_Actions.end(), action);
	if (it == m_Actions.begin() || it == m_Actions.end() || action->getEndFrame() <= frame_nr)
		return false;

	(*std::prev(it))->setEndFrame(frame_nr);
	(*it)->setStartFrame(frame_nr);

	return true;
}

bool EditorCluster::setActionEndTime(const EditorActionPtr& action, size_t frame_nr)
{
	if (m_Actions.size() == 1)
		return false;

	auto it = std::find(m_Actions.begin(), m_Actions.end(), action);
	if (it == (m_Actions.end() - 1) || it == m_Actions.end() || action->getStartFrame() >= frame_nr)
		return false;

	(*std::next(it))->setStartFrame(frame_nr);
	(*it)->setEndFrame(frame_nr);

	return true;
}

bool EditorCluster::splitActions(size_t frame_nr)
{
	//check if frame_nr is in actions
	if (frame_nr <= m_Actions.front()->getStartFrame() || frame_nr >= m_Actions.back()->getEndFrame())
		return false;

	for (auto it = m_Actions.begin(); it != m_Actions.end(); ++it)
	{
		EditorActionPtr action = (*it);
		if (frame_nr > action->getStartFrame() && frame_nr < action->getEndFrame())
		{
			EditorActionPtr newAction = std::make_shared<EditorAction>(*action);
			newAction->setStartFrame(frame_nr);
			action->setEndFrame(frame_nr);
			
			m_Actions.insert(it + 1, newAction);
			return true;
		}
	}

	return false;
}

bool EditorCluster::isEditable() const
{
	return (!m_HasNextContinuation && !m_HasPrevContinuation);
}

EditorActionPtr EditorCluster::getActionByTime(size_t time)
{
	auto it = find_if(m_Actions.begin(), m_Actions.end(), [time](const EditorActionPtr& action){
		return time >= action->getStartFrame() && time <= action->getEndFrame();
	});

	return (it != m_Actions.end()) ? *it : nullptr;	
}

void EditorCluster::update()
{
	size_t start_frame = m_Source->getStartFrame();
	size_t end_frame = m_Source->getEndFrame();

	//zobacz czy akcje mieszcz¹ siê w przedziale klastra
	auto it = m_Actions.begin();
	while(it != m_Actions.end())
	{
		EditorActionPtr action = (*it);

		if (m_Actions.size() == 1)
		{
			action->setStartFrame(start_frame);
			action->setEndFrame(end_frame);
			break;
		}

		if (start_frame >= action->getEndFrame() || end_frame <= action->getStartFrame())
		{
			it = m_Actions.erase(it);
			continue;
		}
		
		if (start_frame > action->getStartFrame() && start_frame < action->getEndFrame())
			action->setStartFrame(start_frame);

		if (end_frame > action->getStartFrame() && end_frame < action->getEndFrame())
			action->setEndFrame(end_frame);

		++it;
	}


	//usuñ dziury, akcje musz¹ byæ ci¹g³e

	m_Actions.front()->setStartFrame(start_frame);
	m_Actions.back()->setEndFrame(end_frame);

	it = m_Actions.begin();
	while (it != m_Actions.end())
	{
		auto nextIt = std::next(it);
		if (nextIt != m_Actions.end())
		{
			if ((*it)->getEndFrame() < (*nextIt)->getStartFrame())
				(*it)->setEndFrame((*nextIt)->getStartFrame());
		}

		++it;
	}
}

