/*
 * ParseMessageVisitor.hpp
 *
 *  Created on: 06-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_PARSEMESSAGEVISITOR_HPP_
#define INCLUDE_RAPIDCHANNEL_PARSEMESSAGEVISITOR_HPP_

#include <rapidChannel/Common.hpp>
#include <rapidChannel/MessageVisitor.hpp>

namespace rapidChannel {
template<typename ProtocolAdaptor>
class ParseMessageVisitor: public MessageVisitor<ProtocolAdaptor> {
private:
	ProtocolAdaptor& _prototcolAdaptor;
public:
	typedef boost::shared_ptr<ParseMessageVisitor> SharedPtr;

	ParseMessageVisitor(ProtocolAdaptor& adaptor) :
			_prototcolAdaptor(adaptor) {

	}

	void visit(LogonMessage<ProtocolAdaptor>& message,
			Buffer& buffer) {
		_prototcolAdaptor.parseFromBuffer(message, buffer);
	}
};
}

#endif /* INCLUDE_RAPIDCHANNEL_PARSEMESSAGEVISITOR_HPP_ */