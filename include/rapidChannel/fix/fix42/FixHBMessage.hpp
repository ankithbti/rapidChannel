/*
 * FixHBMessage.hpp
 *
 *  Created on: 20-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_FIX_FIX42_FIXHBMESSAGE_HPP_
#define INCLUDE_RAPIDCHANNEL_FIX_FIX42_FIXHBMESSAGE_HPP_

#include <rapidChannel/fix/FixBaseMessage.hpp>
#include <rapidChannel/Message.hpp>

// using Base<T>::x

namespace rapidChannel
{
namespace FIX
{
namespace FIX42
{
template<typename ProtocolAdaptor>
class HeartBeat : public Message<ProtocolAdaptor>, public FixBaseMessage
{
public:
	typedef boost::shared_ptr<HeartBeat> SharedPtr;

	HeartBeat() : Message<ProtocolAdaptor>("0"){
		getMutableHeader().setField(35, "0");
	}

	void apply(MessageVisitor<ProtocolAdaptor>& visitor, Buffer& buffer) {
		visitor.visit(*this, buffer);
	}

};
}
}
}




#endif /* INCLUDE_RAPIDCHANNEL_FIX_FIX42_FIXHBMESSAGE_HPP_ */
