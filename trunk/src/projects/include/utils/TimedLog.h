#pragma once
#ifndef DebugLog_h__
#define DebugLog_h__

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QThread>

#include <iostream>
#include <string>

namespace utils
{
	/// <summary>
	/// klasa implementuje funkcjonalnoæ okresowego zapisywania informacji do logu
	/// </summary>
	/// <seealso cref="QObject" />
	class TimedLog : public QObject
	{
		Q_OBJECT
	public:
		TimedLog(QObject* parent = nullptr);

	protected:
		virtual void save(std::ostream& os) = 0;

		/// <summary>
		/// Starts logging.
		/// </summary>
		/// <param name="name">The name. Logs will be saved in directory with this name.</param>
		/// <param name="interval">The interval in seconds.</param>
		void startTimer(const std::string& name, float interval);

	private slots:
		void timeout();

	private:
		QTimer* m_Timer;
		QThread* m_Thread;
		std::string m_Filename;
	};
}

#endif // DebugLog_h__