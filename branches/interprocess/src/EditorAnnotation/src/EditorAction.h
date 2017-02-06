#pragma once

#include <sequence/Cluster.h>
#include <sequence/Action.h>
#include <sequence/IVideo.h>


using namespace sequence;

class EditorAction;

typedef std::shared_ptr<Action> ActionPtr;
typedef std::shared_ptr<EditorAction> EditorActionPtr;


/// <summary>
/// Implementacja pojedynczej akcji przypisanej do klastra.
/// </summary>
class EditorAction
{
	public:
		EditorAction();
		EditorAction(const ActionPtr& action);
		EditorAction(const std::string& filepath);

		EditorAction(const EditorAction& action);

		ActionPtr getSource() const { return m_Source; }

		size_t getStartFrame() const { return m_StartFrame; }
		void setStartFrame(size_t val) { m_StartFrame = val; }

		size_t getEndFrame() const { return m_EndFrame; }
		void setEndFrame(size_t val) { m_EndFrame = val; }

		void clear();

	private:
		ActionPtr m_Source;

		size_t m_StartFrame;
		size_t m_EndFrame;
};

