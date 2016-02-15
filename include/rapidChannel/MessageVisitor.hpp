/*
 * MessageVisitor.hpp
 *
 *  Created on: 06-Feb-2016
 *      Author: ankithbti
 */

#ifndef SRC_MESSAGEVISITOR_HPP_
#define SRC_MESSAGEVISITOR_HPP_

#include <rapidChannel/Common.hpp>
#include <rapidChannel/fix/fix42/FixLogonMessage.hpp>

namespace rapidChannel
{
template <typename ProtocolAdaptor>
class MessageVisitor : private boost::noncopyable
{

public:
	typedef boost::shared_ptr<MessageVisitor<ProtocolAdaptor> > SharedPtr;

	virtual void visit(FIX::FIX42::Logon<ProtocolAdaptor>& message, Buffer& buffer) = 0;

	virtual ~MessageVisitor(){

	}
};

}



#endif /* SRC_MESSAGEVISITOR_HPP_ */
