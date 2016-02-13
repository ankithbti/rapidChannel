/*
 * FixFieldSorter.hpp
 *
 *  Created on: 07-Feb-2016
 *      Author: ankithbti
 */

#ifndef INCLUDE_RAPIDCHANNEL_FIX_FIXFIELDSORTER_HPP_
#define INCLUDE_RAPIDCHANNEL_FIX_FIXFIELDSORTER_HPP_

namespace rapidChannel
{
namespace FIX
{

struct header_order
{
	// To Make sure first 3 tags should be 8,9,35
	static bool compare(const int x, const int y){
		int orderX = getOrderedPosition(x);
		int orderY = getOrderedPosition(y);
		if(orderX && orderY){
			return orderX < orderY;
		}else if(orderX){
			return true;
		}else if(orderY){
			return false;
		}else{
			return x < y;
		}
	}

	static int getOrderedPosition(const int tag){
		switch(tag){
		case 8:
			return 1;
			break;
		case 9:
			return 2;
			break;
		case 35:
			return 3;
			break;
		default:
			return 0;

		};
	}
};


struct trailer_order {
	static bool compare(int x, int y){
		if(x == 10){
			return false;
		}else if(y == 10){
			return false;
		}else{
			return x < y;
		}
	}
};

// Sort fields in given group order
struct group_order{
	static bool compare(int x, int y, int* order, int largest){
		if(x <= largest && y <= largest){
			int orderX = order[x];
			int orderY = order[y];
			if(orderX == 0 && orderY == 0){
				return x < y;
			}else if(orderX == 0){
				return false;
			}else if(orderY  == 0){
				return true;
			}else{
				return orderX < orderY;
			}
		}else if(x <= largest){
			return true;
		}else if(y <= largest){
			return false;
		}else{
			return x < y;
		}
	}
};

typedef std::less < int > normal_order;

struct FixFieldSorter
{
	enum cmp_mode { header, normal, group, trailer };

private:
	cmp_mode _mode;
	int _delim;
	int _largest;
	int* _groupOrder;

public:

	FixFieldSorter(cmp_mode cmpMode) : _mode(cmpMode), _delim(0), _largest(0), _groupOrder(NULL){

	}

	~FixFieldSorter(){
		if(_groupOrder){
			delete [] _groupOrder;
		}
	}

	FixFieldSorter(const FixFieldSorter& rhs) {
		this->_mode = rhs._mode;
		this->_delim = rhs._delim;
		this->_largest = rhs._largest;
		this->_groupOrder = rhs._groupOrder;
	}

	FixFieldSorter& operator=(const FixFieldSorter& rhs){
		if(this == &rhs){
			return *this;
		}
		this->_mode = rhs._mode;
		this->_delim = rhs._delim;
		this->_largest = rhs._largest;
		this->_groupOrder = rhs._groupOrder;
		return *this;
	}

	FixFieldSorter(int first, ...) : _mode(group),_delim(0), _largest(0){
		int field = first;
		int size = 0;
		_largest = _delim = field;
		va_list arguments;
		va_start(arguments, first);
		while( field != 0 ){
			_largest = _largest > field ? _largest : field;
			++size;
			field = va_arg(arguments, int);
		}

		if(size){
			_groupOrder = new int[_largest];
			va_start(arguments, first);
			int field = first;
			int i = 0;
			while(field != 0){
				_groupOrder[field] = ++i;
				field = va_arg(arguments, int);
			}

		}else{
			_largest = _delim = 0;
		}
		va_end(arguments);
	}


	FixFieldSorter(const int order[]) : _mode(group), _delim(0), _largest(0){
		int size = 0;
		while(order[size] != 0){
			++size;
		}

		if(size){
			_largest = _delim = order[0];

			for(int i = 1 ; i < size ; ++i){
				int field = order[i];
				_largest = _largest > field ? _largest : field;
			}

			_groupOrder = new int[_largest];
			for(int i = 0 ; i < size ; ++i){
				_groupOrder[order[i]] = i + 1;
			}
		}
	}

	bool operator()(int x, int y) const {
		switch(_mode){
		case header:
			return header_order::compare(x,y);
			break;
		case normal:
			return x < y;
			break;
		case group:
			return group_order::compare(x,y, _groupOrder, _largest);
			break;
		case trailer:
			return trailer_order::compare(x,y);
			break;
		default:
			return x < y;
		};
	}

};
}
}



#endif /* INCLUDE_RAPIDCHANNEL_FIX_FIXFIELDSORTER_HPP_ */
