/*
 * Message.hpp
 *
 *  Created on: 06-Feb-2016
 *      Author: ankithbti
 */

#ifndef SRC_MESSAGE_HPP_
#define SRC_MESSAGE_HPP_

#include <rapidChannel/Common.hpp>


namespace rapidChannel
{

template<typename ProtocolAdaptor>
class MessageVisitor;

template<typename ProtocolAdaptor>
class Message : private boost::noncopyable
{
public:
	typedef boost::shared_ptr<Message<ProtocolAdaptor> > SharedPtr;

	virtual void apply(MessageVisitor<ProtocolAdaptor>& visitor, Buffer& buffer) = 0;

	virtual ~Message(){

	}
};

}


#endif /* SRC_MESSAGE_HPP_ */
