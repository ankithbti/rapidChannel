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

namespace rapidChannel
{

template <typename Transport, typename ProtocolAdaptor>
class Channel : private boost::noncopyable
{
private:
	typedef boost::shared_ptr<Channel> SharedPtr;

	typename Transport::SharedPtr _transport;
	typename ProtocolAdaptor::SharedPtr _protocolAdaptor;
	typename MessageVisitor<ProtocolAdaptor>::SharedPtr _messageVisitor;
	// To Do - Also few Settings will be member variables

public:
	/**
	 * @throw: Might throw std::runtime_error
	 */
	Channel(){

	}

	/**
	 * @throw : Might throw std::runtime_error
	 */
	void start()
	{

	}

	void stop()
	{

	}

	inline bool isAlive()
	{
		return false; // To Do
	}

	void send(typename Message<ProtocolAdaptor>::SharedPtr msg){
		Buffer buf;
		msg->apply(*(_messageVisitor.get()), buf);
		// Send this buf on transport
	}

};

}



#endif /* SRC_CHANNEL_HPP_ */
