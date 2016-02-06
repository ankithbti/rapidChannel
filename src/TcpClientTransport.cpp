/*
 * TcpClientTransport.cpp
 *
 *  Created on: 07-Feb-2016
 *      Author: ankithbti
 */

#include <rapidChannel/TcpClientTransport.hpp>

namespace rapidChannel
{
  TcpClientTransport::TcpClientTransport (const std::string& ip, size_t port,
  bool isHBReq,
					  size_t hbInterval) :
      _ipAddress (ip), _port (port), _isHBRequired (isHBReq), _hbInterval (hbInterval), _ioService (), _ioWork (
	  _ioService), _resolver (_ioService), _socket (new boost::asio::ip::tcp::socket (_ioService))
  {
    std::cout << __FILE__ << "::" << __FUNCTION__ << " - Constructed Transport for " << _ipAddress << ":" << _port
	<< std::endl;

  }
  void
  TcpClientTransport::start ()
  {
    boost::unique_lock<boost::mutex> lock (_startStopMutex);
    if (!_ioServiceThread)
      {
	_ioServiceThread = boost::shared_ptr<boost::thread> (
	    new boost::thread (&TcpClientTransport::runIoService, this));
	boost::asio::ip::tcp::resolver::query query (_ipAddress, boost::lexical_cast<std::string> (_port));
	_resolver.async_resolve (query, boost::bind (&TcpClientTransport::resolve, this, _1, _2));
	if (_isHBRequired)
	  {
	    _hbTimer = boost::shared_ptr<boost::asio::deadline_timer> (
		new boost::asio::deadline_timer (_ioService, boost::posix_time::seconds (_hbInterval)));
	    _hbTimer->async_wait (boost::bind (&TcpClientTransport::sendHeartBeat, this, _1));
	  }
      }
    else
      {
	std::cout << __FILE__ << "::" << __FUNCTION__ << " - Transport is already Running. " << std::endl;
      }

  }
  void
  TcpClientTransport::stop ()
  {
    boost::unique_lock<boost::mutex> lock (_startStopMutex);
    if (_ioServiceThread)
      {
	_ioServiceThread->interrupt ();
	_socket->close ();
	_hbTimer->cancel ();
      }
  }

  void
  TcpClientTransport::send (Buffer& buf)
  {
//	_socket->async_write_some(boost::asio::buffer(buf),
//			boost::bind(&TcpClientTransport::asyncWrite, this, _1, _2));

    boost::asio::async_write (*(_socket.get ()), boost::asio::buffer (buf),
			      boost::bind (&TcpClientTransport::asyncWrite, this, _1, _2));
  }

  void
  TcpClientTransport::resolve (const boost::system::error_code& err,
			       boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
  {
    if (!err)
      {
	boost::asio::ip::tcp::endpoint endPoint = *endpoint_iterator;
	std::cout << __FILE__ << "::" << __FUNCTION__ << " - Resolved to " << endPoint.address ().to_string () << ":"
	    << endPoint.port () << std::endl;

	_socket->async_connect (endPoint, boost::bind (&TcpClientTransport::connect, this, _1));
      }
    else
      {
	std::cout << __FILE__ << "::" << __FUNCTION__ << " - Error: " << err.message () << std::endl;
      }

  }
  void
  TcpClientTransport::connect (const boost::system::error_code& err)
  {
    if (!err)
      {
	// Connection successful
	std::cout << __FILE__ << "::" << __FUNCTION__ << " - Connection Successful to " << std::endl;

	// To Do - Call the Channel Callback that Connection is Successful

	// Start Reading Part
	read ();
      }
    else
      {
	std::cout << __FILE__ << "::" << __FUNCTION__ << " - Error: " << err.message () << std::endl;
      }
  }

  void
  TcpClientTransport::read ()
  {
    _socket->async_read_some (boost::asio::buffer (_buffer),
			      boost::bind (&TcpClientTransport::asyncRead, this, _1, _2));
  }

  void
  TcpClientTransport::asyncRead (const boost::system::error_code& err, std::size_t bytes_transferred)
  {
    if (!err)
      {
	std::cout << __FILE__ << "::" << __FUNCTION__ << " - Read " << bytes_transferred << " bytes sent from client - "
	    << _buffer.data () << std::endl;
	read ();
      }
    else
      {
	if(boost::iequals("End of file", err.message())){
	    read();
	}else{
	    std::cout << __FILE__ << "::" << __FUNCTION__ << " - Error: " << err.message () << std::endl;
	}
      }
  }
  void
  TcpClientTransport::asyncWrite (const boost::system::error_code& err, std::size_t bytes_transferred)
  {
    if (!err)
      {
	std::cout << __FILE__ << "::" << __FUNCTION__ << " - Write " << bytes_transferred << " bytes sent to server. "
	    << std::endl;
      }
    else
      {
	std::cout << __FILE__ << "::" << __FUNCTION__ << " - Error: " << err.message () << std::endl;
      }
  }
  void
  TcpClientTransport::sendHeartBeat (const boost::system::error_code& err)
  {
    std::cout << __FILE__ << "::" << __FUNCTION__ << " - Sending HB... " << std::endl;
  }

  void
  TcpClientTransport::runIoService ()
  {
    std::cout << __FILE__ << "::" << __FUNCTION__ << " - Starting IOService Thread. " << std::endl;
    _ioService.run ();
    std::cout << __FILE__ << "::" << __FUNCTION__ << " - Ending IOService Thread. " << std::endl;
  }

}

