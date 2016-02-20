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

template<typename Transport, typename ProtocolAdaptor>
class Channel: private boost::noncopyable
{
public:
	typedef boost::shared_ptr<Channel> SharedPtr;
	typedef boost::variant<boost::function<void(const std::string&, const std::string&)>,
			boost::function<void(const std::string&)>,
			boost::function<void(typename Message<ProtocolAdaptor>::SharedPtr)> > ChannelCallback;

	enum CallbackType
	{
		OnConnect = 0, OnDisconnect = 1, OnError = 2, OnMessage = 4
	};

	typedef std::map<CallbackType, ChannelCallback> ChannelCallbackContainer;
	typedef typename ChannelCallbackContainer::iterator ChannelCallbackContainerIt;
	typedef typename ChannelCallbackContainer::const_iterator ChannelCallbackContainerConstIt;

private:

	typedef fitiedCoreCpp::appSetting::Setting::SmartPtr SettingSmartPtr;

	SettingSmartPtr _channelSetting;
	typename Transport::SharedPtr _transport;
	typename ProtocolAdaptor::SharedPtr _protocolAdaptor;
	typename MessageVisitor<ProtocolAdaptor>::SharedPtr _messageVisitor;
	ChannelCallbackContainer _callbacks;

public:

	void onDataRec(const std::string& buf)
	{
		_protocolAdaptor->deserialise(buf);
	}

	/**
	 * @throw: Might throw std::runtime_error
	 */
	Channel(SettingSmartPtr channelSetting, ChannelCallbackContainer callabcks = ChannelCallbackContainer()) :
			_channelSetting(channelSetting), _transport(new Transport("127.0.0.1", 5001)), _protocolAdaptor(
					new ProtocolAdaptor(_channelSetting)), _messageVisitor(
					new ConvertMessageVisitor<ProtocolAdaptor>(*_protocolAdaptor.get())), _callbacks(callabcks)
	{
		// Set some common Header Properties for FIX via config in Channel and then it will send those to Adaptor
		// Channel can pass these fields to Adaptor - SenderCompID, Target, sendingTime, seqNo, lastRecSeqNo etc.
		// Adaptor can decide how to use these fields
		_protocolAdaptor->setSendMessageCallback(boost::bind(&Channel::send, this, _1));
		_transport->setDataRecCallback(boost::bind(&Channel::onDataRec, this, _1));
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

	void send(typename Message<ProtocolAdaptor>::SharedPtr msg)
	{
		Buffer buf;
		msg->apply(*(_messageVisitor.get()), buf);
		std::cout << " Data: " << buf.data() << std::endl;
		// Send this buf on transport
		_transport->send(buf);
	}

};

}

#endif /* SRC_CHANNEL_HPP_ */
