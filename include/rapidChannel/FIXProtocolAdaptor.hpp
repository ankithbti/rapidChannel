/*
 * FIXProtocolAdaptor.hpp
 *
 *  Created on: 07-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_FIXPROTOCOLADAPTOR_HPP_
#define INCLUDE_RAPIDCHANNEL_FIXPROTOCOLADAPTOR_HPP_

#include <rapidChannel/Common.hpp>
#include <rapidChannel/Logon.hpp>

namespace rapidChannel {

class FIXProtocolAdaptor : private boost::noncopyable {

public:

	void convertToBuffer(LogonMessage<FIXProtocolAdaptor>& message, Buffer& buffer);
	void parseFromBuffer(LogonMessage<FIXProtocolAdaptor>& message, Buffer& buffer);

};

}




#endif /* INCLUDE_RAPIDCHANNEL_FIXPROTOCOLADAPTOR_HPP_ */
