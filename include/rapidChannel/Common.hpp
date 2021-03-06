/*
 * Common.hpp
 *
 *  Created on: 06-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_COMMON_HPP_
#define INCLUDE_RAPIDCHANNEL_COMMON_HPP_

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/variant.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time.hpp>
#include <boost/atomic.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/function.hpp>

#include <boost/fusion/mpl.hpp>
#include <boost/fusion/container.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/functional.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/define_struct_inline.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/is_sequence.hpp>

#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

namespace rapidChannel
{
//	typedef boost::array<unsigned char, 256> Buffer;
	typedef std::string Buffer;
	typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketSharedPtr;
	typedef std::pair<int, std::string> FixField;
}



#endif /* INCLUDE_RAPIDCHANNEL_COMMON_HPP_ */
