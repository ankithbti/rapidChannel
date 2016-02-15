/*
 * FixLogonMessage.hpp
 *
 *  Created on: 07-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_FIX_FIX42_FIXLOGONMESSAGE_HPP_
#define INCLUDE_RAPIDCHANNEL_FIX_FIX42_FIXLOGONMESSAGE_HPP_

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
class Logon : public Message<ProtocolAdaptor>, public FixBaseMessage
{
public:
	Logon(){
		getMutableHeader().setField(35, "A");
	}

	void apply(MessageVisitor<ProtocolAdaptor>& visitor, Buffer& buffer) {
		visitor.visit(*this, buffer);
	}
};
}
}
}


#endif /* INCLUDE_RAPIDCHANNEL_FIX_FIX42_FIXLOGONMESSAGE_HPP_ */
