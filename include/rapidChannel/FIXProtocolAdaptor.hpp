/*
 * FIXProtocolAdaptor.hpp
 *
 *  Created on: 07-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_FIXPROTOCOLADAPTOR_HPP_
#define INCLUDE_RAPIDCHANNEL_FIXPROTOCOLADAPTOR_HPP_

#include <rapidChannel/Common.hpp>
#include <rapidChannel/fix/fix42/FixLogonMessage.hpp>
#include <Setting.hpp>

namespace rapidChannel {

class FIXProtocolAdaptor : private boost::noncopyable {
public:
	typedef boost::shared_ptr<FIXProtocolAdaptor> SharedPtr;
	typedef fitiedCoreCpp::appSetting::Setting::SmartPtr SettingSmartPtr;

private:
	SettingSmartPtr _fixSetting;
public:
	FIXProtocolAdaptor(SettingSmartPtr setting){
		std::cout << " Created FixProtocolAdaptor. " << std::endl;
		// Take out FixSetting out of incoming setting - might be come as Root Setting


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
