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
#include <rapidChannel/fix/fix42/FixHBMessage.hpp>
#include <Setting.hpp>
#include <rapidChannel/fix/SeqNoMemoryMap.hpp>

namespace rapidChannel
{

class FIXProtocolAdaptor: private boost::noncopyable
{
public:
	typedef boost::shared_ptr<FIXProtocolAdaptor> SharedPtr;
	typedef boost::function<void(Message<FIXProtocolAdaptor>::SharedPtr)> SendMessageCallback;
	typedef fitiedCoreCpp::appSetting::Setting::SmartPtr SettingSmartPtr;
	typedef std::pair<fitiedCoreCpp::appSetting::Setting::ChildrenListConstIt,
			fitiedCoreCpp::appSetting::Setting::ChildrenListConstIt> SettingChildernBeginEndItPair;
	typedef boost::shared_ptr<boost::asio::deadline_timer> HBTimer;
private:
	SettingSmartPtr _fixSetting;
	std::string _senderCompId;
	std::string _targetCompId;
	std::string _fixVersion;
	std::string _sessionId;
	std::string _fixStorePathPrefix;
	FIX::SeqNoMemoryMap::SharedPtr _senderSeqNo;
	FIX::SeqNoMemoryMap::SharedPtr _targetSeqNo;
	int _beginSeqNo;bool _resetSeqNo;
	std::string _hbInterval;
	std::string _encryptType;
	SendMessageCallback _sendMsgCallback;
	boost::asio::io_service _ioService;
	HBTimer _hbTimer;
	boost::shared_ptr<boost::thread> _ioServiceRunThread;
	boost::asio::io_service::work _ioWork;
	std::string _remBuffer;

	void runIoServiceThread()
	{
		std::cout << " Started FIXPA ioService Thread. " << std::endl;
		_ioService.run();
		std::cout << " Stopped FIXPA ioService Thread. " << std::endl;
	}

	void sendHB(const boost::system::error_code& error)
	{
		if (!error)
		{
			// Timer expired.
			std::cout << " Timer triggered. " << std::endl;
			// Send HB message
			Message<FIXProtocolAdaptor>::SharedPtr hbMsg(new FIX::FIX42::HeartBeat<FIXProtocolAdaptor>());
			if (_sendMsgCallback)
			{
				std::cout << " Calling send HB callback. " << std::endl;
				_sendMsgCallback(hbMsg);
			}
			_hbTimer->expires_from_now(boost::posix_time::seconds(boost::lexical_cast<int>(_hbInterval)));
			_hbTimer->async_wait(boost::bind(&FIXProtocolAdaptor::sendHB, this, _1));
		}
		else
		{
			std::cout << " Error in Timer: " << error.message() << std::endl;
		}
	}

public:

	~FIXProtocolAdaptor()
	{
		_ioService.stop();
		if (_ioServiceRunThread)
		{
			_ioServiceRunThread->interrupt();
		}
	}
	FIXProtocolAdaptor(SettingSmartPtr setting) :
			_beginSeqNo(0), _resetSeqNo(0), _ioService(), _hbTimer(new boost::asio::deadline_timer(_ioService)), _ioWork(
					_ioService)

	{
		std::cout << " Created FixProtocolAdaptor. " << std::endl;
		// Take out FixSetting out of incoming setting - might be come as Root Setting
		if (boost::iequals(setting->getName(), "fixSetting"))
		{
			_fixSetting = setting;
		}
		else if (setting->isChildPresent("fixSetting"))
		{
			// Find in children nodes
			_fixSetting = setting->getChild("fixSetting");
		}
		else
		{
			throw std::runtime_error(" No FixSetting present in incoming settings. ");
		}

		try
		{

			SettingChildernBeginEndItPair childernIrPair = _fixSetting->getChildrenIterators();
			for (fitiedCoreCpp::appSetting::Setting::ChildrenListConstIt it = childernIrPair.first;
					it != childernIrPair.second; ++it)
			{
				std::cout << " Child: " << (*it)->getName() << " Child: " << (*it)->getChildCount() << std::endl;
				if (boost::iequals((*it)->getName(), "fixHeader"))
				{
					// Add static Header fix fields to the Logon message

					SettingChildernBeginEndItPair fixHeaderFields =
							_fixSetting->getChild("fixHeader")->getChildrenIterators();
					for (fitiedCoreCpp::appSetting::Setting::ChildrenListConstIt fixHeaderField = fixHeaderFields.first;
							fixHeaderField != fixHeaderFields.second; ++fixHeaderField)
					{
						if ((*fixHeaderField)->isAttributePresent("tag")
								&& (*fixHeaderField)->isAttributePresent("value"))
						{
							switch (boost::lexical_cast<int>((*fixHeaderField)->getAttributeValue("tag")))
							{
							case 8:
								_fixVersion = (*fixHeaderField)->getAttributeValue("value");
								break;
							case 49:
								_senderCompId = (*fixHeaderField)->getAttributeValue("value");
								break;
							case 56:
								_targetCompId = (*fixHeaderField)->getAttributeValue("value");
								break;
							default:
								break;
							}
						}
					}

				}
				else if (boost::iequals((*it)->getName(), "fixDefaultSetting"))
				{
					if ((*it)->isAttributePresent("path-prefix"))
					{
						_fixStorePathPrefix = (*it)->getAttributeValue("path-prefix");
					}
					if ((*it)->isAttributePresent("beginSeqNo"))
					{
						_beginSeqNo = boost::lexical_cast<int>((*it)->getAttributeValue("beginSeqNo"));
					}
					if ((*it)->isAttributePresent("resetSeqNo"))
					{
						_resetSeqNo = boost::iequals((*it)->getAttributeValue("resetSeqNo"), "Y");
					}
					if ((*it)->isAttributePresent("heartBtInt"))
					{
						_hbInterval = (*it)->getAttributeValue("heartBtInt");
					}
					if ((*it)->isAttributePresent("encryptType"))
					{
						_encryptType = (*it)->getAttributeValue("encryptType");
					}
				}
			}

			if (_hbInterval.empty())
			{
				_hbInterval = "30";
			}

		} catch (const boost::bad_lexical_cast& err)
		{
			throw std::runtime_error(
					std::string(
							" Failed to read mandatory header fields from setting. Caught bad_lexical_cast exception: ")
							+ err.what());
		} catch (const std::runtime_error& err)
		{
			throw std::runtime_error(
					std::string(" Failed to read mandatory header fields from setting. Caught generic exception: ")
							+ err.what());
		}

		if (_fixVersion.empty() || _senderCompId.empty() || _targetCompId.empty())
		{
			throw std::runtime_error(
					" Missing mandatory header parameter [ FixVersion / SenderCompId / TargetCompId ] in setting. ");
		}
		_sessionId = _fixVersion + "-" + _senderCompId + "-" + _targetCompId;

		_senderSeqNo = FIX::SeqNoMemoryMap::SharedPtr(
				new FIX::SeqNoMemoryMap(_fixStorePathPrefix + "/" + _sessionId + ".seqno.sender", _beginSeqNo));
		_targetSeqNo = FIX::SeqNoMemoryMap::SharedPtr(
				new FIX::SeqNoMemoryMap(_fixStorePathPrefix + "/" + _sessionId + ".seqno.target", _beginSeqNo));

		if (_resetSeqNo)
		{
			_senderSeqNo->resetSeqNo();
			_targetSeqNo->resetSeqNo();
		}

		if (_encryptType.empty())
		{
			_encryptType = "0";
		}

		_ioServiceRunThread = boost::shared_ptr<boost::thread>(
				new boost::thread(boost::bind(&FIXProtocolAdaptor::runIoServiceThread, this)));

		int hbInt = boost::lexical_cast<int>(_hbInterval);
		if (hbInt > 0)
		{
			std::cout << " Setting timer for HB...." << std::endl;
			_hbTimer->expires_from_now(boost::posix_time::seconds(hbInt));
			_hbTimer->async_wait(boost::bind(&FIXProtocolAdaptor::sendHB, this, _1));
		}

	}

	void setSendMessageCallback(SendMessageCallback cb)
	{
		_sendMsgCallback = cb;
	}

	void convertToBuffer(FIX::FIX42::Logon<FIXProtocolAdaptor>& message, Buffer& buffer)
	{

		try
		{

			SettingChildernBeginEndItPair childernIrPair = _fixSetting->getChildrenIterators();
			for (fitiedCoreCpp::appSetting::Setting::ChildrenListConstIt it = childernIrPair.first;
					it != childernIrPair.second; ++it)
			{
				std::cout << " Child: " << (*it)->getName() << " Child: " << (*it)->getChildCount() << std::endl;
				if (boost::iequals((*it)->getName(), "fixHeader"))
				{
					// Add static Header fix fields to the Logon message

					SettingChildernBeginEndItPair fixHeaderFields =
							_fixSetting->getChild("fixHeader")->getChildrenIterators();
					for (fitiedCoreCpp::appSetting::Setting::ChildrenListConstIt fixHeaderField = fixHeaderFields.first;
							fixHeaderField != fixHeaderFields.second; ++fixHeaderField)
					{
						if ((*fixHeaderField)->isAttributePresent("tag")
								&& (*fixHeaderField)->isAttributePresent("value"))
						{
							message.getMutableHeader().setField(
									boost::lexical_cast<int>((*fixHeaderField)->getAttributeValue("tag")),
									(*fixHeaderField)->getAttributeValue("value"));
						}
					}

				}
				else if (boost::iequals((*it)->getName(), "fixMessage"))
				{
					if ((*it)->isAttributePresent("type")
							&& boost::iequals((*it)->getAttributeValue("type"), message.getHeader().getFieldValue(35)))
					{
						SettingChildernBeginEndItPair logonFields = (*it)->getChildrenIterators();
						for (fitiedCoreCpp::appSetting::Setting::ChildrenListConstIt logonField = logonFields.first;
								logonField != logonFields.second; ++logonField)
						{
							if ((*logonField)->isAttributePresent("tag") && (*logonField)->isAttributePresent("value"))
							{
								message.setField(boost::lexical_cast<int>((*logonField)->getAttributeValue("tag")),
										(*logonField)->getAttributeValue("value"));
							}
						}
					}
				}
			}

		} catch (const std::runtime_error& err)
		{
			std::cout << " Exception caught: " << err.what() << std::endl;
		}

		if (boost::iequals(message.getHeader().getFieldValue(35), "A"))
		{
			message.setField(98, _encryptType);
			message.setField(108, _hbInterval);
		}

		// Attach the sending Time as well
		///////////////////////////
		// YYYYMMDD-HH:MM:SS.sss
		///////////////////////////
		boost::posix_time::ptime currTime(boost::posix_time::microsec_clock::universal_time());
		std::string isoDateTime = boost::posix_time::to_iso_string(currTime);
		std::cout << " Curr Time: " << isoDateTime << std::endl;
		std::string yyyymmdd = isoDateTime.substr(0, 8);
		std::string hh = isoDateTime.substr(9, 2);
		std::string mm = isoDateTime.substr(11, 2);
		std::string ss = isoDateTime.substr(13, 2);
		std::string millisec = isoDateTime.substr(16, 3);
		std::string reqFormatDateTime = yyyymmdd + "-" + hh + ":" + mm + ":" + ss + "." + millisec;
		///////////////////////////
		message.getMutableHeader().setField(52, reqFormatDateTime);

		// Attch the seqNo
		message.getMutableHeader().setField(34, boost::lexical_cast<std::string>(_senderSeqNo->getCurrentSeqNo()));

		std::string str;
		message.toString(str);

		// Fill the Buffer
		std::copy(str.begin(), str.end(), buffer.data());

		_senderSeqNo->incrementAndGetSeqNo();

	}

	void parseFromBuffer(FIX::FIX42::Logon<FIXProtocolAdaptor>& message, const Buffer& buffer)
	{
		std::cout << " Converting to Logon from Buffer " << std::endl;

		// If any message received then reset and start the HBTimer Thread
	}

	void convertToBuffer(FIX::FIX42::HeartBeat<FIXProtocolAdaptor>& message, Buffer& buffer)
	{
		try
		{

			SettingChildernBeginEndItPair childernIrPair = _fixSetting->getChildrenIterators();
			for (fitiedCoreCpp::appSetting::Setting::ChildrenListConstIt it = childernIrPair.first;
					it != childernIrPair.second; ++it)
			{
				std::cout << " Child: " << (*it)->getName() << " Child: " << (*it)->getChildCount() << std::endl;
				if (boost::iequals((*it)->getName(), "fixHeader"))
				{
					// Add static Header fix fields to the Logon message

					SettingChildernBeginEndItPair fixHeaderFields =
							_fixSetting->getChild("fixHeader")->getChildrenIterators();
					for (fitiedCoreCpp::appSetting::Setting::ChildrenListConstIt fixHeaderField = fixHeaderFields.first;
							fixHeaderField != fixHeaderFields.second; ++fixHeaderField)
					{
						if ((*fixHeaderField)->isAttributePresent("tag")
								&& (*fixHeaderField)->isAttributePresent("value"))
						{
							message.getMutableHeader().setField(
									boost::lexical_cast<int>((*fixHeaderField)->getAttributeValue("tag")),
									(*fixHeaderField)->getAttributeValue("value"));
						}
					}

				}
				else if (boost::iequals((*it)->getName(), "fixMessage"))
				{
					if ((*it)->isAttributePresent("type")
							&& boost::iequals((*it)->getAttributeValue("type"), message.getHeader().getFieldValue(35)))
					{
						SettingChildernBeginEndItPair logonFields = (*it)->getChildrenIterators();
						for (fitiedCoreCpp::appSetting::Setting::ChildrenListConstIt logonField = logonFields.first;
								logonField != logonFields.second; ++logonField)
						{
							if ((*logonField)->isAttributePresent("tag") && (*logonField)->isAttributePresent("value"))
							{
								message.setField(boost::lexical_cast<int>((*logonField)->getAttributeValue("tag")),
										(*logonField)->getAttributeValue("value"));
							}
						}
					}
				}
			}

		} catch (const std::runtime_error& err)
		{
			std::cout << " Exception caught: " << err.what() << std::endl;
		}

		if (boost::iequals(message.getHeader().getFieldValue(35), "A"))
		{
			message.setField(98, _encryptType);
			message.setField(108, _hbInterval);
		}

		// Attach the sending Time as well
		///////////////////////////
		// YYYYMMDD-HH:MM:SS.sss
		///////////////////////////
		boost::posix_time::ptime currTime(boost::posix_time::microsec_clock::universal_time());
		std::string isoDateTime = boost::posix_time::to_iso_string(currTime);
		std::cout << " Curr Time: " << isoDateTime << std::endl;
		std::string yyyymmdd = isoDateTime.substr(0, 8);
		std::string hh = isoDateTime.substr(9, 2);
		std::string mm = isoDateTime.substr(11, 2);
		std::string ss = isoDateTime.substr(13, 2);
		std::string millisec = isoDateTime.substr(16, 3);
		std::string reqFormatDateTime = yyyymmdd + "-" + hh + ":" + mm + ":" + ss + "." + millisec;
		///////////////////////////
		message.getMutableHeader().setField(52, reqFormatDateTime);

		// Attch the seqNo
		message.getMutableHeader().setField(34, boost::lexical_cast<std::string>(_senderSeqNo->getCurrentSeqNo()));

		std::string str;
		message.toString(str);

		// Fill the Buffer
		std::copy(str.begin(), str.end(), buffer.data());

		_senderSeqNo->incrementAndGetSeqNo();

	}

	void parseFromBuffer(FIX::FIX42::HeartBeat<FIXProtocolAdaptor>& message, const Buffer& buffer)
	{
		std::cout << " Converting to Logon from Buffer " << std::endl;

		// If any message received then reset and start the HBTimer Thread
	}

	void deserialise(const std::string& buf){
		// Deserialise as per FIX protocol
//		8=<NeginString>[\001]9=<Length>[].....10=<CS>[]

		//Check if complete or not.....
		std::cout << " FIXPA - Data: " << buf << std::endl;

	}

private:
	void init()
	{

	}

};

}

#endif /* INCLUDE_RAPIDCHANNEL_FIXPROTOCOLADAPTOR_HPP_ */
