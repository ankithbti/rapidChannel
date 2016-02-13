/*
 * FixBaseMessage.hpp
 *
 *  Created on: 07-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_FIX_FIXBASEMESSAGE_HPP_
#define INCLUDE_RAPIDCHANNEL_FIX_FIXBASEMESSAGE_HPP_

#include <rapidChannel/Common.hpp>
#include <rapidChannel/fix/FixFieldMap.hpp>

namespace rapidChannel
{
namespace FIX
{

static const int headerOrder[] = {
		8, 9, 35
};

static const std::string AdminMessageTypes = "0A12345";

class FixHeader : public FieldMap
{
public:
	FixHeader() : FieldMap(FixFieldSorter(FixFieldSorter::header)){
	}
};

class FixTrailer : public FieldMap
{
public:
	FixTrailer() : FieldMap(FixFieldSorter(FixFieldSorter::trailer)){
	}
};

class FixBaseMessage : public FieldMap
{
public:
	typedef boost::shared_ptr<FixBaseMessage> SharedPtr;

	enum fieldType { header, body, trailer };

protected:
	int _field;
	FixHeader _header;
	FixTrailer _footer;
	bool _isValidMessage;

	FixBaseMessage(const std::string& beginString, const std::string& msgType) : _isValidMessage(true){
		_header.setField(8, beginString);
		_header.setField(35, msgType);
	}

public:
	FixBaseMessage() : _isValidMessage(true){

	}

	FixBaseMessage(const std::string& str, bool validate = true){
		setString(str);
	}

	void toString(std::string& str, int beginString = 8, int bodylen = 9, int cs = 10){
		int length = bodyLength(beginString, bodylen, cs);
		_header.setField(9, boost::lexical_cast<std::string>(length));
		_footer.setField(10, boost::lexical_cast<std::string>(checksum(cs)));
		str.clear();
		str.reserve(length + 64);
		_header.getFixString(str);
		getFixString(str);
		_footer.getFixString(str);
	}

	void setString(const std::string& str, bool checkValidity = true){
		clear();
		std::string::size_type pos = 0;
		std::string msgType;
		int count = 0;
		fieldType type = header;
		while(pos < str.length()){
			FixField::SharedPtr ff(extractField(str, pos));
			if(count < 3 && headerOrder[count++] != ff->getTag()){
				throw std::runtime_error(" Not a valid fix message. Header sequence is wrong.");
			}
			if(isHeaderField(ff->getTag())){
				if(type != header){
					if(_field == 0){
						_field = ff->getTag();
					}
					_isValidMessage = false;
				}

				if(ff->getTag() == 35){
					msgType = ff->getValue();
				}

				_header.setField(ff->getTag(), ff->getValue());

			}else if(isTrailerField(ff->getTag())){
				type = trailer;
				_footer.setField(ff->getTag(), ff->getValue());
			}else{
				if(type == trailer){
					if(_field == 0){
						_field = ff->getTag();
					}
					_isValidMessage = false;
				}

				type = body;
				setField(ff->getTag(), ff->getValue());

			}
		}

		if(checkValidity){
			validate();
		}

	}

	void setStringHeader(const std::string& str, bool validate = true){

	}

	void reverseRoutr(FixHeader& header){

	}

	bool isAdminMessage() const{
		if(_header.isSetField(35)){
			return isAdminMessageType(_header.getFieldValue(35));
		}
		return false;
	}

	bool isAppMessage() const{
		if(_header.isSetField(35)){
			return !(isAdminMessageType(_header.getFieldValue(35)));
		}
		return false;
	}

	int bodyLength(int beginString = 8, int bodyLen = 9, int checksum = 10) const{
		return _header.getLength(beginString, bodyLen, checksum) +
				getLength(beginString, bodyLen, checksum) +
				_footer.getLength(beginString, bodyLen, checksum);
	}

	int checksum(int checksum = 10) const {
		return ((_header.getTotal(checksum) +
				getTotal(checksum) +
				_footer.getTotal(checksum)) % 256);
	}

	bool isHeaderField(int tag) const {
		switch(tag){
		case 8:
		case 9:
		case 35:
		case 49:
		case 56:
		case 34:
		case 52:
			return true;
			break;
		default:
			return false;
		}
	}

	bool isTrailerField(int tag) const {
		switch(tag){
		case 93:
		case 89:
		case 10:
			return true;
			break;
		default:
			return false;
		}
	}

	FixHeader& getMutableHeader() {
		return _header;
	}

	const FixHeader& getHeader() const{
		return _header;
	}

	FixTrailer& getMutableTrailer() {
		return _footer;
	}

	const FixTrailer& getTrailer() const {
		return _footer;
	}

	static bool isAdminMessageType(const std::string& msgType) {
		if(msgType.length() != 1){
			return false;
		}
		return (std::string::npos != AdminMessageTypes.find(msgType.at(0)));
	}

	void clear(){
		_field = 0;
		_header.clear();
		_footer.clear();
		_isValidMessage = false;
		FieldMap::clear();

	}

	bool isEmpty() const{
		return _header.isEmpty() && _footer.isEmpty() && FieldMap::isEmpty();
	}

private:
	void validate(){

	}

	FixField::SharedPtr extractField(const std::string& str, std::string::size_type& pos){

		// Not considering Data Field as of now - To Do
		std::string::const_iterator beginPos = str.begin() + pos;
		std::string::const_iterator equalSignPos = std::find(beginPos, str.end(), '=');
		if(equalSignPos == str.end()){
			throw std::runtime_error(" Wrong FIX format given in string. Equal sign not there.");
		}

		int tag = boost::lexical_cast<int>(std::string(beginPos, equalSignPos));

		std::string::const_iterator valStart = equalSignPos + 1;

		std::string::const_iterator soh = std::find(valStart, str.end(), '\001');
		if(soh == str.end()){
			throw std::runtime_error(" Wrong FIX format given in string. SOH not there");
		}

		std::string val(std::string(valStart, soh));
		pos = std::distance(str.begin(), soh + 1);
		return FixField::SharedPtr(new FixField(tag, val));

	}

};
}
}

#endif /* INCLUDE_RAPIDCHANNEL_FIX_FIXBASEMESSAGE_HPP_ */
