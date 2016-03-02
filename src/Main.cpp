#include<iostream>
#include <rapidChannel/Channel.hpp>
#include <rapidChannel/TcpClientTransport.hpp>
#include <rapidChannel/fix/fix42/FixLogonMessage.hpp>
#include <rapidChannel/FIXProtocolAdaptor.hpp>
#include <XmlSettingParser.hpp>
#include <rapidChannel/fix/SeqNoMemoryMap.hpp>
#include <rapidChannel/CallbackHelper.hpp>

const char SOH = '\001';

using namespace rapidChannel;
using namespace fitiedCoreCpp::appSetting;

typedef Channel<TcpClientTransport, FIXProtocolAdaptor> ChannelType;
typedef CallbackHelper<FIXProtocolAdaptor> CBHelper;


boost::mutex g_mutex;
boost::condition_variable g_cv;
int flag = 0;

void evenThread(){
	int x = 0;
	while(x < 10){
		boost::unique_lock<boost::mutex> lock(g_mutex);
		if(flag == 1){
			g_cv.wait(lock);
		}
		std::cout << x << std::endl;
		flag = 1;
		g_cv.notify_one();
		x += 2;
	}
}

void oddThread(){
	int y = 1;
	while(y < 10){
		boost::unique_lock<boost::mutex> lock(g_mutex);
		if(flag == 0){
			g_cv.wait(lock);
		}
		std::cout << y << std::endl;
		flag = 0;
		g_cv.notify_one();
		y += 2;
	}
}

void onMessage(Message<FIXProtocolAdaptor>::SharedPtr msg, const std::string& type){
	std::cout << " Got Message from Channel of Type: " << type << std::endl;
}

void onConnect(const std::string& id)
{
	std::cout << " Got onConnect via ID: " << id << std::endl;
}


void onDisconnect(const std::string& id)
{
	std::cout << " Got onDisconnect via ID: " << id << std::endl;
}

void onError(const std::string& id, const std::string& err)
{
	std::cout << " Got onError via ID: " << id << " Err:" << err << std::endl;
}

int main()
{

	// Interveiw Q 1
	//	boost::thread t1(&evenThread);
	//	boost::thread t2(&oddThread);
	//
	//	t1.join();
	//	t2.join();

	//return 0;

	size_t port = 5001;

	try
	{

		const std::string configFile("config/appSetting.xml");
		XmlSettingParser xmlSettingParser(configFile);
		Setting::SmartPtr rootSetting(xmlSettingParser.getRoot());

		CBHelper::SharedPtr cbHelper(new CBHelper());


		boost::function<void(typename Message<FIXProtocolAdaptor>::SharedPtr, const std::string&)> onMsgRec
				= boost::bind(&onMessage, _1, _2);
		boost::function<void(const std::string&)> onConnectCB
				= boost::bind(&onConnect, _1);
		boost::function<void(const std::string&)> onDisconnectCB
				= boost::bind(&onDisconnect, _1);
		boost::function<void(const std::string&, const std::string&)> onErrorCB
				= boost::bind(&onError, _1, _2);

		cbHelper->set(CBHelper::OnMessage, onMsgRec);
		cbHelper->set(CBHelper::OnConnect, onConnectCB);
		cbHelper->set(CBHelper::OnDisconnect, onDisconnectCB);
		cbHelper->set(CBHelper::OnError, onErrorCB);


		Channel<TcpClientTransport, FIXProtocolAdaptor> c1(rootSetting, cbHelper);
		c1.start();

		boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

		Message<FIXProtocolAdaptor>::SharedPtr fixLogonMsg(new FIX::FIX42::Logon<FIXProtocolAdaptor>());
		c1.send(fixLogonMsg);

		while (true)
		{
			std::cout << __FILE__ << "::" << __FUNCTION__ << " - Running - Going to sleep. " << std::endl;
			boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
		}

	} catch (const std::runtime_error& err)
	{
		std::cout << " Error: " << err.what() << std::endl;
		return 1;
	}

	return 0;

	TcpClientTransport client("127.0.0.1", port);
	client.start();

	//Buffer logonMessage;
	std::stringstream logonHeaderPart1;
	std::stringstream logonHeaderPart2;
	//std::stringstream logonBody;
	std::stringstream logonFooter;

	int messageLength = 0;

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

	logonHeaderPart2 << "35=A" << SOH << "49=CLIENT1" << SOH << "56=EXECUTOR" << SOH << "34=1" << SOH << "52="
			<< reqFormatDateTime << SOH << "98=0" << SOH << "108=30" << SOH;
	// the message following the BodyLength field up to, and including, the delimiter immediately preceding the
	// CheckSum tag ("10=").
	messageLength = logonHeaderPart2.str().length();

	logonHeaderPart1 << "8=FIX.4.2" << SOH << "9=" << messageLength << SOH << logonHeaderPart2.str();

	///////////////////////////
	// To get Checksum
	///////////////////////////
	static char tmpBuf[4];
	long idx;
	unsigned int cks;
	for (idx = 0L, cks = 0; idx < logonHeaderPart1.str().length(); cks +=
			(unsigned int) logonHeaderPart1.str().c_str()[idx++])
		;
	sprintf(tmpBuf, "%03d", (unsigned int) (cks % 256));
	///////////////////////////

	// The checksum algorithm of FIX consists of summing up the decimal value of the ASCII representation
	// all the bytes up to the checksum field (which is last) and return the value modulo 256.
	logonFooter << "10=" << tmpBuf << SOH;

	logonHeaderPart1 << logonFooter.str();
	std::cout << __FILE__ << "::" << __FUNCTION__ << " Fix Message: " << logonHeaderPart1.str() << std::endl;

	std::string fixMsg = logonHeaderPart1.str();
	Buffer b;
	b = fixMsg;
	//std::copy(fixMsg.begin(), fixMsg.end(), b.data());

	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	std::cout << " --> Data: " << b.data() << " <-- " << std::endl;

	client.send(b);

	while (true)
	{
		std::cout << __FILE__ << "::" << __FUNCTION__ << " - Running - Going to sleep. " << std::endl;
		boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	}
	return 0;
}
