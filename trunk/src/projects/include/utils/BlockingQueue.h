#pragma once
#ifndef BlockingQueue_h__
#define BlockingQueue_h__

#include <queue>
#include <mutex>
#include <condition_variable>
#include <assert.h>

namespace utils
{
	template<typename T>
	class BlockingQueue
	{
	public:
		BlockingQueue() : m_Released(false) { }
		~BlockingQueue() 
		{
			
		}

		void push(const T& object)
		{
			T tmpObj(object);
			{
				std::lock_guard<std::mutex> lock(m_Mutex);
				m_Queue.push(std::move(tmpObj));
			}
			m_Condition.notify_one();
		}

		void push(T&& object)
		{
			{
				std::lock_guard<std::mutex> lock(m_Mutex);
				if (m_Released)
					return;
				m_Queue.push(std::move(object));
			}
			m_Condition.notify_one();
		}

		bool pop(T& outObject)
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			if (!m_Released)
				m_Condition.wait(lock, [this]{ return !m_Queue.empty() || m_Released; });
			if (m_Released)
				return false;
			outObject = std::move(m_Queue.front());
			m_Queue.pop();
			return true;
		}

		void release()
		{
			{
				std::unique_lock<std::mutex> lock(m_Mutex);
				m_Released = true;
			}
			m_Condition.notify_all();
		}

		typename std::queue<T>::size_type size() const { return m_Queue.size(); }

	private:
		std::queue<T> m_Queue;
		std::mutex m_Mutex;
		std::condition_variable m_Condition;
		bool m_Released;
	};
}

#endif // BlockingQueue_h__