/*
 * SeqNoMemoryMap.hpp
 *
 *  Created on: 15-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_FIX_SEQNOMEMORYMAP_HPP_
#define INCLUDE_RAPIDCHANNEL_FIX_SEQNOMEMORYMAP_HPP_

#include <rapidChannel/Common.hpp>

namespace rapidChannel
{
namespace FIX
{
class SeqNoMemoryMap : private boost::noncopyable
{
public:
	typedef boost::shared_ptr<SeqNoMemoryMap> SharedPtr;
private:
	int * _mmap;
	int _fd;
	boost::atomic_int _seqNo;
	int _beginSeqNo;

	void setSeqNo(int val);

public:
	SeqNoMemoryMap(const std::string& file, int beginSeqNo);
	~SeqNoMemoryMap();
	int getCurrentSeqNo();
	int incrementAndGetSeqNo();
	void resetSeqNo();
};
}
}




#endif /* INCLUDE_RAPIDCHANNEL_FIX_SEQNOMEMORYMAP_HPP_ */
