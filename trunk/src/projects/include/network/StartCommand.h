#pragma once
#ifndef START_COMMAND_H_
#define START_COMMAND_H_

#include "AbstractCommand.h"

#include <boost/serialization/base_object.hpp>

namespace Network
{
	class StartCommand : public AbstractCommand
	{
	public:
		StartCommand() {}
		StartCommand(const std::string& _receiverIp, const std::string& _tag, const std::string& _exePathWithArgs)		
			: AbstractCommand(_receiverIp, _tag), exePathWithArgs_(_exePathWithArgs) {}

		virtual std::string encode();
		virtual Type getType() const { return Start; }		

		std::string exePathWithArgs() const { return exePathWithArgs_; }

	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<AbstractCommand>(*this);
			ar & exePathWithArgs_;
		}

		std::string exePathWithArgs_;
	};
}

#endif // START_COMMAND_H_