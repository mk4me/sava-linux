#pragma once
#ifndef RAPORT_FINISHED_COMMAND_H_
#define RAPORT_FINISHED_COMMAND_H_

#include "StartCommand.h"

#include <boost/serialization/base_object.hpp>
#include <QtCore/QProcess>

namespace Network
{
	class RaportFinishedCommand : public StartCommand
	{
	public:
		//! Constructor.
		RaportFinishedCommand() {}
		//! Constructor.
		/*!
		\param _receiverIp from which ip command was sent.
		\param _tag for special command description.
		*/
		RaportFinishedCommand(const std::string& _receiverIp, const std::string& _tag,
			const std::string& _exePathWithArgs, const int& _exitCode, const QProcess::ExitStatus& _exitStatus)
			: StartCommand(_receiverIp, _tag, _exePathWithArgs), exitStatus_(_exitStatus) {}

		//! Serialize command.
		/*!
		\return serialized command.
		*/
		virtual std::string encode();
		//! Get type of command.
		/*!
		\return RaportFinished type of command.
		*/
		virtual Type getType() const { return RaportFinished; }

		//! Get exit code of process.
		/*!
		\return exit code of process when finish.
		*/
		int exitCode() const { return exitCode_; }
		//! Get exit status of process.
		/*!
		\return exit status of process when finish.
		*/
		QProcess::ExitStatus exitStatus() const { return exitStatus_; }

	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<StartCommand>(*this);
			ar & exitCode_;
			ar & exitStatus_;
		}

		//! Exit code of process.
		int exitCode_;
		//! Exit status of process.
		QProcess::ExitStatus exitStatus_;
	};
}

#endif // RAPORT_FINISHED_COMMAND_H_