#pragma once
#ifndef RAPORT_ERROR_COMMAND_H_
#define RAPORT_ERROR_COMMAND_H_

#include "StartCommand.h"

#include <boost/serialization/base_object.hpp>
#include <QtCore/QProcess>

namespace Network
{
	class RaportErrorCommand : public StartCommand
	{
	public:
		//! Constructor.
		RaportErrorCommand() {}
		//! Constructor.
		/*!
		\param _receiverIp from which ip command was sent.
		\param _tag for special command description.
		*/
		RaportErrorCommand(const std::string& _receiverIp, const std::string& _tag,
			const std::string& _exePathWithArgs, const QProcess::ProcessError& _error)			
			: StartCommand(_receiverIp, _tag, _exePathWithArgs), error_(_error) {}

		//! Serialize command.
		/*!
		\return serialized command.
		*/
		virtual std::string encode();
		//! Get type of command.
		/*!
		\return RaportError type of command.
		*/
		virtual Type getType() const { return RaportError; }
		
		//! Get error of process.
		/*!
		\return error of process when finish.
		*/
		QProcess::ProcessError getError() const { return error_; }

	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<StartCommand>(*this);
			ar & error_;
		}

		//! Error of process.
		QProcess::ProcessError error_;
	};
}

#endif // RAPORT_ERROR_COMMAND_H_