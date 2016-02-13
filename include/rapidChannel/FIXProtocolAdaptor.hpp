/*
 * FIXProtocolAdaptor.hpp
 *
 *  Created on: 07-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_FIXPROTOCOLADAPTOR_HPP_
#define INCLUDE_RAPIDCHANNEL_FIXPROTOCOLADAPTOR_HPP_

#include <rapidChannel/Common.hpp>
#include <rapidChannel/FIXLogonMessage.hpp>
#include <rapidChannel/FIXHeartBeatMessage.hpp>
#include <rapidChannel/FIXProtocol.hpp>
#include <rapidChannel/fix/fix42/FixLogonMessage.hpp>

namespace rapidChannel {

class FIXProtocolAdaptor : private boost::noncopyable {
public:
	typedef boost::shared_ptr<FIXProtocolAdaptor> SharedPtr;
	FIXProtocolAdaptor(){
		// To Do - Configs will come here as arguments for FIX Header
		// like SenderCompId, targetCompId, HeartBeat Interval etc.
		// Those will be passed to FIXProtocol
		std::cout << " Created FixProtocolAdaptor. " << std::endl;
	}

	void convertToBuffer(FIX::FIX42::Logon<FIXProtocolAdaptor>& message, Buffer& buffer){
		std::string str;
		message.toString(str);
		std::cout << " Converting the Logon to Buffer : " << str << std::endl;
	}

	void parseFromBuffer(FIX::FIX42::Logon<FIXProtocolAdaptor>& message, const Buffer& buffer){
		std::cout << " Converting to Logon from Buffer " << std::endl;
	}

};

}




#endif /* INCLUDE_RAPIDCHANNEL_FIXPROTOCOLADAPTOR_HPP_ */
