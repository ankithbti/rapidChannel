/*
 * FixField.hpp
 *
 *  Created on: 07-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_FIX_FIXFIELD_HPP_
#define INCLUDE_RAPIDCHANNEL_FIX_FIXFIELD_HPP_

#include <rapidChannel/Common.hpp>

namespace rapidChannel
{
namespace FIX
{
class FixField : private boost::noncopyable
{
private:
	int _tag;
	std::string _value;
	std::string _fixString;
	size_t _length;
	int _total;
public:

	typedef boost::shared_ptr<FixField> SharedPtr;

	FixField(int tag, const std::string& val) :
		_tag(tag),
		_value(val),
		_fixString(boost::lexical_cast<std::string>(_tag) + "=" + _value + '\001'),
		_total(0){
		_length = std::distance(_fixString.begin(), _fixString.end());
		for(std::string::const_iterator it = _fixString.begin() ; it != _fixString.end() ; ++it){
			_total += static_cast<unsigned char>(*it);
		}
		//std::cout << " Total for tag: " << _fixString << " is " << _total << std::endl;
	}

	~FixField()
	{
		//std::cout << " Destructor Called for: " << _tag << "  " << _value << std::endl;
	}

	const std::string& getValue() const {
		return _value;
	}

	int getTag() const{
		return _tag;
	}

	const std::string& getFixString() const{
		return _fixString;
	}

	size_t getLength() const {
		return _length;
	}

	int getTotal() const {
		return _total;
	}


};
}
}



#endif /* INCLUDE_RAPIDCHANNEL_FIX_FIXFIELD_HPP_ */
