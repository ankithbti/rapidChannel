/*
 * Logon.hpp
 *
 *  Created on: 06-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_LOGON_HPP_
#define INCLUDE_RAPIDCHANNEL_LOGON_HPP_

#include <rapidChannel/Common.hpp>
#include <rapidChannel/Message.hpp>

namespace rapidChannel
{
template <typename ProtocolAdaptor>
struct LogonMessage : public Message<ProtocolAdaptor>
{
	typedef boost::shared_ptr<LogonMessage<ProtocolAdaptor> > SharedPtr;

	void apply(MessageVisitor<ProtocolAdaptor>& visitor, Buffer& buffer)
	{
		visitor->visit(*this, buffer);
	}

	int _encryptMethod;
	int _hbInterval;
	std::string _userName;
	std::string _password;

};
}

//BOOST_FUSION_ADAPT_STRUCT(
//		rapidChannel::LogonMessage,
//		(int, _encryptMethod)
//		(int, _hbInterval)
//		(std::string, _userName)
//		(std::string, _password));
//


#endif /* INCLUDE_RAPIDCHANNEL_LOGON_HPP_ */
