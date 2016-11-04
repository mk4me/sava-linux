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
		enum Type
		{
			Start,
			Stop,
			Kill,
			RaportError,
			RaportFinished
		};

		AbstractCommand() {}
		AbstractCommand(const std::string& _receiverIp, const std::string& _tag)
			: reciverIp_(_receiverIp), tag_(_tag) {}

		static std::shared_ptr<AbstractCommand> decode(const std::string& _message);
		virtual std::string encode() = 0;

		virtual Type getType() const = 0;
		std::string getTag() const { return tag_; }

		bool isThisForMe(const std::string& _ip) const { return reciverIp_ == _ip; }		

	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & reciverIp_;
			ar & tag_;
		}

		std::string reciverIp_;
		std::string tag_;		
	};
}

#endif // ABSTRACT_COMMAND_H_