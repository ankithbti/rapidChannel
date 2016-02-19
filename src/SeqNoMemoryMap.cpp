/*
 * SeqNoMemoryMap.cpp
 *
 *  Created on: 15-Feb-2016
 *      Author: ankithbti
 */
#include <rapidChannel/fix/SeqNoMemoryMap.hpp>
#include <unistd.h>
#include <sys/mman.h>

namespace rapidChannel
{
namespace FIX
{

SeqNoMemoryMap::SeqNoMemoryMap(const std::string& file, int beginSeqNo) :
		_mmap(0), _fd(0), _seqNo(0), _beginSeqNo(beginSeqNo)
{
	{

		_fd = open(file.c_str(), O_RDWR | O_CREAT, mode_t(0644));
		if (_fd == -1)
		{
			throw std::runtime_error(" Problem while opening file handle for: " + file);
		}

		int result = lseek(_fd, sizeof(int), 0);
		if (result == -1)
		{
			throw std::runtime_error(" Failed to get the end of file: " + file);
		}
		result = write(_fd, "", 1);
		if (result == -1)
		{
			throw std::runtime_error(" Failed to write at end of file: " + file);
		}

		// Again back to beginning
		result = lseek(_fd, 0, 0);
		if (result == -1)
		{
			throw std::runtime_error(" Failed to get the end of file: " + file);
		}

		_mmap = (int*) mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
		if (_mmap == MAP_FAILED)
		{
			throw std::runtime_error(" Failed to create mmap for file: " + file);
		}

		std::cout << "Created mem map successfully. " << std::endl;
		if (*_mmap > 0)
		{
			_seqNo = *_mmap;
		}
		else
		{
			setSeqNo(_beginSeqNo);
		}
		std::cout << "SeqNo: " << _seqNo << std::endl;
	}

}

SeqNoMemoryMap::~SeqNoMemoryMap()
{
	if (_mmap)
	{
		if (munmap(_mmap, sizeof(int)) == -1)
		{
			std::cout << " Failed to unmap. " << std::endl;
		}
	}
	close(_fd);

}

int SeqNoMemoryMap::getCurrentSeqNo()
{
	return _seqNo;
}
int SeqNoMemoryMap::incrementAndGetSeqNo()
{
	_seqNo.fetch_add(1);
	setSeqNo(_seqNo);
	return _seqNo;
}
void SeqNoMemoryMap::setSeqNo(int val)
{
	_seqNo = val;
	_mmap[0] = _seqNo;
}
void SeqNoMemoryMap::resetSeqNo()
{
	setSeqNo(_beginSeqNo);
}

}
}

