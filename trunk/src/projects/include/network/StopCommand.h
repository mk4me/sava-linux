#pragma once
#ifndef STOP_COMMAND_H_
#define STOP_COMMAND_H_

#include "AbstractCommand.h"

#include <boost/serialization/base_object.hpp>

namespace Network
{
	class StopCommand : public AbstractCommand
	{
	public:
		StopCommand() {}
		StopCommand(const std::string& _receiverIp, const std::string& _tag)
			: AbstractCommand(_receiverIp, _tag) {}

		virtual std::string encode();
		virtual Type getType() const { return Stop; }

	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<AbstractCommand>(*this);
		}
	};
}

#endif // STOP_COMMAND_H_