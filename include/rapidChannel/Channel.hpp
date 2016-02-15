/*
 * Channel.hpp
 *
 *  Created on: 06-Feb-2016
 *      Author: ankithbti
 */

#ifndef SRC_CHANNEL_HPP_
#define SRC_CHANNEL_HPP_

#include <rapidChannel/Common.hpp>
#include <rapidChannel/Message.hpp>
#include <rapidChannel/ConvertMessageVisitor.hpp>
#include <rapidChannel/fix/FixField.hpp>
#include <Setting.hpp>

namespace rapidChannel
{

template <typename Transport, typename ProtocolAdaptor>
class Channel : private boost::noncopyable
{
private:
	typedef boost::shared_ptr<Channel> SharedPtr;
	typedef fitiedCoreCpp::appSetting::Setting::SmartPtr SettingSmartPtr;

	SettingSmartPtr _channelSetting;
	typename Transport::SharedPtr _transport;
	typename ProtocolAdaptor::SharedPtr _protocolAdaptor;
	typename MessageVisitor<ProtocolAdaptor>::SharedPtr _messageVisitor;

public:
	/**
	 * @throw: Might throw std::runtime_error
	 */
	Channel(SettingSmartPtr channelSetting) :
	_channelSetting(channelSetting),
	_transport(new Transport("127.0.0.1", 5001)),
	_protocolAdaptor(new ProtocolAdaptor(_channelSetting)),
	_messageVisitor(new ConvertMessageVisitor<ProtocolAdaptor>(*_protocolAdaptor.get())){
		// Set some common Header Properties for FIX via config in Channel and then it will send those to Adaptor
		// Channel can pass these fields to Adaptor - SenderCompID, Target, sendingTime, seqNo, lastRecSeqNo etc.
		// Adaptor can decide how to use these fields
	}

	/**
	 * @throw : Might throw std::runtime_error
	 */
	void start()
	{
	  _transport->start();
	}

	void stop()
	{
	  _transport->stop();
	}

	inline bool isAlive()
	{
		return _transport->isRunning();
	}

	void send(typename Message<ProtocolAdaptor>::SharedPtr msg){
		Buffer buf;
		msg->apply(*(_messageVisitor.get()), buf);
		// Send this buf on transport
		//_transport->send(buf);
	}

};

}



#endif /* SRC_CHANNEL_HPP_ */
