#pragma once
#ifndef ABSTRACT_COMMAND_H_
#define ABSTRACT_COMMAND_H_

#include <string>
#include <memory>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>

namespace Network
{
	class AbstractCommand
	{
	public:
		//! Command type recognized and parsed in system.
		enum Type
		{
			Start,
			Stop,
			Kill,
			RaportError,
			RaportFinished
		};

		//! Constructor.
		AbstractCommand() {}
		//! Constructor.
		/*!
		\param _receiverIp from which ip command was sent.
		\param _tag for special command description.
		*/
		AbstractCommand(const std::string& _receiverIp, const std::string& _tag)
			: receiverIp_(_receiverIp), tag_(_tag) {}

		//! Decodes serialized command.
		/*!
		\param _message to decode.
		\return decoded command.
		*/
		static std::shared_ptr<AbstractCommand> decode(const std::string& _message);
		//! Serialize command.
		/*!
		\return serialized command.
		*/
		virtual std::string encode() = 0;

		//! Get type of command.
		/*!
		\return type of command.
		*/
		virtual Type getType() const = 0;
		//! Get command's tag.
		/*!
		\return type of command.
		*/
		std::string getTag() const { return tag_; }
		//! Check if received command is for correct receiver.
		/*!
		\return true if command is for computer that received it.
		*/
		bool isThisForMe(const std::string& _ip) const { return receiverIp_ == _ip; }		

	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & receiverIp_;
			ar & tag_;
		}

		//! Receiver Ip.
		std::string receiverIp_;
		//! Tag for command.
		std::string tag_;		
	};
}

#endif // ABSTRACT_COMMAND_H_