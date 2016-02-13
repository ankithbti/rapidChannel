/*
 * ConvertMessageVisitor.hpp
 *
 *  Created on: 06-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_CONVERTMESSAGEVISITOR_HPP_
#define INCLUDE_RAPIDCHANNEL_CONVERTMESSAGEVISITOR_HPP_

#include <rapidChannel/Common.hpp>
#include <rapidChannel/MessageVisitor.hpp>

namespace rapidChannel {

template< typename ProtocolAdaptor>
class ConvertMessageVisitor: public MessageVisitor<ProtocolAdaptor> {
private:
	ProtocolAdaptor& _prototcolAdaptor;
public:

	typedef boost::shared_ptr<ConvertMessageVisitor> SharedPtr;

	ConvertMessageVisitor(ProtocolAdaptor& adaptor) : _prototcolAdaptor(adaptor) {
		std::cout << " Created ConvertMessageVisitor. " << std::endl;
	}

	void visit(FIX::FIX42::Logon<ProtocolAdaptor>& message, Buffer& buffer) {
		_prototcolAdaptor.convertToBuffer(message, buffer);
	}

};
}

#endif /* INCLUDE_RAPIDCHANNEL_CONVERTMESSAGEVISITOR_HPP_ */
