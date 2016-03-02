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
#include <rapidChannel/CallbackHelper.hpp>
#include <Setting.hpp>

namespace rapidChannel
{

template<typename Transport, typename ProtocolAdaptor>
class Channel: private boost::noncopyable
{
public:
	typedef boost::shared_ptr<Channel> SharedPtr;

	friend Transport;
	friend ProtocolAdaptor;
private:

	typedef fitiedCoreCpp::appSetting::Setting::SmartPtr SettingSmartPtr;

	SettingSmartPtr _channelSetting;
	typename Transport::SharedPtr _transport;
	typename ProtocolAdaptor::SharedPtr _protocolAdaptor;
	typename MessageVisitor<ProtocolAdaptor>::SharedPtr _messageVisitor;
	typename
	CallbackHelper<ProtocolAdaptor>::SharedPtr _cbHelper;

private:

	void onDataRec(const std::string& buf)
	{
		_protocolAdaptor->deserialise(buf);
	}

	void onMessageReceived(typename Message<ProtocolAdaptor>::SharedPtr msg, const std::string& type){
		std::cout << " Channel onMsg Rec: " << std::endl;
		try{
			boost::function<void(typename Message<ProtocolAdaptor>::SharedPtr, const std::string&)> onMessageCb =
					boost::get<boost::function<void(typename Message<ProtocolAdaptor>::SharedPtr, const std::string&)> >(_cbHelper->get(CallbackHelper<ProtocolAdaptor>::OnMessage));

			onMessageCb(msg, type);

		}catch(const std::runtime_error& err){

		}
	}

	void onConnect( const std::string& id){
		std::cout << " Channel onConnect Rec: " << std::endl;
		try{
			boost::function<void(const std::string&)> onConnectCb =
					boost::get<boost::function<void(const std::string&)> >(_cbHelper->get(CallbackHelper<ProtocolAdaptor>::OnConnect));

			onConnectCb(id);

		}catch(const std::runtime_error& err){

		}
	}

	void onDisconnect(const std::string& id){
		std::cout << " Channel onDisconnect Rec: " << std::endl;
		try{
			boost::function<void(const std::string&)> onDisconnectCb =
					boost::get<boost::function<void(const std::string&)> >(_cbHelper->get(CallbackHelper<ProtocolAdaptor>::OnDisconnect));

			onDisconnectCb(id);

		}catch(const std::runtime_error& err){

		}
	}

	void onError(const std::string& id, const std::string& errMsg){
			std::cout << " Channel onError Rec: " << std::endl;
			try{
				boost::function<void(const std::string&, const std::string&)> onErrorCb =
						boost::get<boost::function<void(const std::string&, const std::string&)> >(_cbHelper->get(CallbackHelper<ProtocolAdaptor>::OnDisconnect));

				onErrorCb(id, errMsg);

			}catch(const std::runtime_error& err){

			}
		}

public:

	/**
	 * @throw: Might throw std::runtime_error
	 */
	Channel(SettingSmartPtr channelSetting, typename CallbackHelper<ProtocolAdaptor>::SharedPtr cbHelper = CallbackHelper<ProtocolAdaptor>::SharedPtr(new CallbackHelper<ProtocolAdaptor>())) :
		_channelSetting(channelSetting), _transport(new Transport("127.0.0.1", 5001)), _protocolAdaptor(
				new ProtocolAdaptor(_channelSetting)), _messageVisitor(
						new ConvertMessageVisitor<ProtocolAdaptor>(*_protocolAdaptor.get())), _cbHelper(cbHelper)
{
		// Set some common Header Properties for FIX via config in Channel and then it will send those to Adaptor
		// Channel can pass these fields to Adaptor - SenderCompID, Target, sendingTime, seqNo, lastRecSeqNo etc.
		// Adaptor can decide how to use these fields
		_protocolAdaptor->setSendMessageCallback(boost::bind(&Channel::send, this, _1));
		_protocolAdaptor->setReceivedMessageCallback(boost::bind(&Channel::onMessageReceived, this, _1, _2));
		_protocolAdaptor->setOnErrorCallback(boost::bind(&Channel::onError, this, _1, _2));
		_transport->setDataRecCallback(boost::bind(&Channel::onDataRec, this, _1));
		_transport->setOnConnectCallback(boost::bind(&Channel::onConnect, this, _1));
		_transport->setOnDisconnectCallback(boost::bind(&Channel::onDisconnect, this, _1));
		_transport->onErrorCallback(boost::bind(&Channel::onError, this, _1, _2));
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
