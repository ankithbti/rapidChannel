/*
 * CallbackHelper.hpp
 *
 *  Created on: 28-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_CALLBACKHELPER_HPP_
#define INCLUDE_RAPIDCHANNEL_CALLBACKHELPER_HPP_

#include <rapidChannel/Common.hpp>


namespace rapidChannel
{
template<typename ProtocolAdaptor>
class CallbackHelper : private boost::noncopyable{
public:

	typedef boost::shared_ptr<CallbackHelper> SharedPtr;

	typedef boost::variant<boost::function<void(const std::string&, const std::string&)>,
			boost::function<void(const std::string&)>,
			boost::function<void(typename Message<ProtocolAdaptor>::SharedPtr)>,
			boost::function<void(typename Message<ProtocolAdaptor>::SharedPtr, const std::string&)> > ChannelCallback;

	enum CallbackType
	{
		OnConnect = 0, OnDisconnect = 1, OnError = 2, OnMessage = 4
	};

	typedef std::map<CallbackType, ChannelCallback> ChannelCallbackContainer;
	typedef typename ChannelCallbackContainer::iterator ChannelCallbackContainerIt;
	typedef typename ChannelCallbackContainer::const_iterator ChannelCallbackContainerConstIt;
	typedef std::pair<ChannelCallbackContainerIt, bool> ChannelCallbackContainerInsertRetVal;

private:

	ChannelCallbackContainer _callbacks;

public:


	void set(CallbackType type, ChannelCallback cb, bool overwrite = false){
		ChannelCallbackContainerInsertRetVal retVal = _callbacks.insert(std::pair<CallbackType, ChannelCallback>(type, cb));
		if(!retVal.second && overwrite){
			retVal.first->second = cb;
		}
	}

	ChannelCallback get(CallbackType type) const {
		ChannelCallbackContainerConstIt it = _callbacks.find(type);
		if(it == _callbacks.end()){
			throw std::runtime_error(" CallbackHelper::get() - No callback present for type: " + boost::lexical_cast<std::string>(type));
		}
		return it->second;
	}

	void clear(){
		_callbacks.clear();
	}

	void clear(CallbackType type){
		ChannelCallbackContainerConstIt it = _callbacks.find(type);
		if(it != _callbacks.end()){
			_callbacks.erase(it);
		}
	}

};
}


#endif /* INCLUDE_RAPIDCHANNEL_CALLBACKHELPER_HPP_ */
