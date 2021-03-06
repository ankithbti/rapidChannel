/*
 * TcpClientTransport.hpp
 *
 *  Created on: 07-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_TCPCLIENTTRANSPORT_HPP_
#define INCLUDE_RAPIDCHANNEL_TCPCLIENTTRANSPORT_HPP_

#include <rapidChannel/Common.hpp>

namespace rapidChannel
{
class TcpClientTransport: private boost::noncopyable
{
public:
	typedef boost::shared_ptr<TcpClientTransport> SharedPtr;

	typedef boost::function<void(const std::string&, const std::string&)> OnErrorCallback;
	typedef boost::function<void(const std::string&)> OnConnectCallback;
	typedef boost::function<void(const std::string&)> OnDisconnectCallback;
	typedef boost::function<void(const std::string&)> OnDataRecCallback;

private:

	std::string _ipAddress;
	size_t _port;bool _isHBRequired;
	size_t _hbInterval;
	boost::asio::io_service _ioService;
	boost::asio::io_service::work _ioWork;
	boost::asio::ip::tcp::resolver _resolver;
	SocketSharedPtr _socket;
	boost::shared_ptr<boost::asio::deadline_timer> _hbTimer;
	boost::mutex _startStopMutex;
	boost::shared_ptr<boost::thread> _ioServiceThread;
	boost::array<unsigned char, 256> _buffer;
	bool _isAlive;

	OnDataRecCallback _recDataCallback;
	OnConnectCallback _onConnectCallback;
	OnDisconnectCallback _onDisconnectCallback;
	OnErrorCallback _onErrorCallback;

	void resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
	void connect(const boost::system::error_code& err);
	void read();
	void asyncRead(const boost::system::error_code& ec, std::size_t bytes_transferred);
	void asyncWrite(const boost::system::error_code& ec, std::size_t bytes_transferred);
	void sendHeartBeat(const boost::system::error_code& ec);
	void runIoService();

public:
	TcpClientTransport(const std::string& ip, size_t port, bool isHBReq = false, size_t hbInterval = 30);
	void start();
	void stop();
	inline bool isRunning()
	{
		boost::unique_lock<boost::mutex> lock(_startStopMutex);
		return _isAlive;
	}

	void setDataRecCallback(OnDataRecCallback cb)
	{
		_recDataCallback = cb;
	}

	void setOnConnectCallback(OnConnectCallback cb)
	{
		_onConnectCallback = cb;
	}

	void setOnDisconnectCallback(OnDisconnectCallback cb)
	{
		_onDisconnectCallback = cb;
	}

	void onErrorCallback(OnErrorCallback cb)
	{
		_onErrorCallback = cb;
	}

	void send(Buffer& buf);
};
}

#endif /* INCLUDE_RAPIDCHANNEL_TCPCLIENTTRANSPORT_HPP_ */
