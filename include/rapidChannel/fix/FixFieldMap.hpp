/*
 * FixFieldMap.hpp
 *
 *  Created on: 07-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_FIX_FIXFIELDMAP_HPP_
#define INCLUDE_RAPIDCHANNEL_FIX_FIXFIELDMAP_HPP_

#include <rapidChannel/Common.hpp>
#include <rapidChannel/fix/FixField.hpp>
#include <rapidChannel/fix/FixFieldSorter.hpp>

namespace rapidChannel
{
namespace FIX
{
class FieldMap : private boost::noncopyable
{
public:
	typedef boost::shared_ptr<FieldMap> SharedPtr;

	// To Do - may be multimap - not sure
	typedef std::map<int, FixField::SharedPtr, FixFieldSorter> Fields;
	typedef Fields::iterator FieldsItr;
	typedef Fields::const_iterator FieldsConstItr;
	typedef std::pair<Fields::iterator, bool> FieldsInsertRetType;

	typedef std::map<int, std::vector<FieldMap::SharedPtr>, normal_order > Groups;
	typedef Groups::iterator GroupsItr;
	typedef Groups::const_iterator GroupsConstItr;
	typedef std::pair<Groups::iterator, bool> GroupsInsertRetType;

private:

	Fields _fields;
	Groups _groups;

public:

	FieldMap(const FixFieldSorter& fieldSorter = FixFieldSorter(FixFieldSorter::normal)) : _fields(fieldSorter){

	}

	FieldMap(const int order[]) : _fields(FixFieldSorter(order)){

	}

	~FieldMap(){
		clear();
	}

	void setField(FixField::SharedPtr field, bool overwrite = true){
		FieldsInsertRetType retVal = _fields.insert(Fields::value_type(field->getTag(), field));
		if(overwrite && !retVal.second){
			retVal.first->second = field;
		}
	}

	void setField(int tag, const std::string& val){
		FixField::SharedPtr ff(new FixField(tag, val));
		setField(ff);
	}

	bool getFieldIfSet(FixField::SharedPtr fixField) const {
		FieldsConstItr it = _fields.find(fixField->getTag());
		if(it == _fields.end()){
			return false;
		}
		fixField = it->second;
		return true;
	}

	const std::string& getFieldValue(int tag) const {
		FieldsConstItr it = _fields.find(tag);
		if(it == _fields.end()){
			throw std::runtime_error(" - No Field present with required Tag: " + boost::lexical_cast<std::string>(tag));
		}
		return it->second->getValue();
	}

	bool isSetField(int tag) const {
		FieldsConstItr it = _fields.find(tag);
		if(it == _fields.end()){
			return false;
		}
		return true;
	}

	void removeField(int field){
		Fields::iterator it = _fields.find(field);
		if(it != _fields.end()){
			_fields.erase(it);
		}
	}

	void addGroup(int field, FieldMap::SharedPtr fieldMap, bool setCount = true){}
	FieldMap::SharedPtr getGroup(int num, int field) const{}
	void removeGroup(int num, int field){}
	bool hasGroup(int field) const{}
	bool hasGroup(int num, int field) const{}

	size_t getGroupCount(int field) const{
	}

	void clear(){
		_fields.clear();
		// Clear the vector inside map
		for(Groups::iterator it = _groups.begin() ; it != _groups.end() ; ++it){
			it->second.clear();
		}
	}

	bool isEmpty() const{
		return (_fields.size() == 0);
	}


	size_t totalFields() const {
		size_t retVal = _fields.size();
		for(GroupsConstItr it = _groups.begin() ; it != _groups.end() ; ++it){
			BOOST_FOREACH(FieldMap::SharedPtr fm, it->second){
				retVal += fm->totalFields();
			}
		}
		return retVal;
	}

	void getFixString(std::string& str) const{
		for(FieldsConstItr it = _fields.begin() ; it != _fields.end() ; ++it){
			str += it->second->getFixString();
			if(_groups.size() == 0){
				continue;
			}else{
				for(GroupsConstItr git = _groups.begin() ; git != _groups.end() ; ++git){
					BOOST_FOREACH(FieldMap::SharedPtr fm, git->second){
						fm->getFixString(str);
					}
				}
			}
		}
	}


	int getLength(const int beginString = 8, const int bodyLen = 9, const int checksum = 10) const {
		int retLengthVal = 0;
		for(FieldsConstItr it = _fields.begin() ; it != _fields.end() ; ++it){
			if(it->second->getTag() != beginString && it->second->getTag() != bodyLen && it->second->getTag() != checksum ){
				retLengthVal += it->second->getLength();
			}

			if(_groups.size() == 0){
				continue;
			}else{
				for(GroupsConstItr git = _groups.begin() ; git != _groups.end() ; ++git){
					BOOST_FOREACH(FieldMap::SharedPtr fm, git->second){
						retLengthVal += fm->getLength(beginString, bodyLen, checksum);
					}
				}
			}
		}
		return retLengthVal;
	}

	int getTotal(const int checksum = 10) const{
		int retVal = 0;
		for(FieldsConstItr it = _fields.begin() ; it != _fields.end() ; ++it){
			if(it->second->getTag() != checksum){
				retVal += it->second->getTotal();
			}

			if(_groups.size() == 0){
				continue;
			}else{
				for(GroupsConstItr git = _groups.begin() ; git != _groups.end() ; ++git){
					BOOST_FOREACH(FieldMap::SharedPtr fm, git->second){
						retVal += fm->getTotal(checksum);
					}
				}
			}
		}
		return retVal;
	}

	FieldsConstItr begin() const {
		return _fields.begin();
	}

	FieldsConstItr end() const {
		return _fields.end();
	}

	GroupsConstItr groupBegin() const {
		return _groups.begin();
	}

	GroupsConstItr groupEnd() const {
		return _groups.end();
	}

};
}
}



#endif /* INCLUDE_RAPIDCHANNEL_FIX_FIXFIELDMAP_HPP_ */
