#ifndef _DICHOTOMY_INLINE_
#define _DICHOTOMY_INLINE_

#include "assert.h"

template <class _T>
dichotomy<_T>::dichotomy()
{
	_values.reserve(64);
}

template <class _T>
dichotomy<_T>::~dichotomy()
{

}

template <class _T>
void dichotomy<_T>::remove_by_index(int index)
{
	_values.erase(_values.begin() + index);
}

template <class _T>
int dichotomy<_T>::insert(const _T & key)
{
	int top = int(_values.size())-1,middle = 0,bottom = 0,id=0;

	if (find(key) > -1)
		return -2;

	if (top < 0 || key > _values[top].key)
	{
		value v;
		v.key	= key;
		v.data	= NULL;
		_values.push_back(v);
		return int(_values.size()) - 1;
	}
	else
		if (_values.size() > 0 && key < _values[0].key)
		{
			value v;
			v.key = key;
			v.data = NULL;
			_values.insert(_values.begin(),v);
			return 0;
		}


	_T	tmp;
	for (;bottom<=top;)
	{
		middle = (top + bottom)/2;
		if (middle >= int(_values.size()))
			break;
		tmp = _values[middle].key;
		if (key > tmp)
		{
			if ( (id = insert_at_next(middle+1,key)) != -1)
				return id;
		}
		else
		{
			if ( (id = insert_at_prev(middle-1,key)) != -1)
				return id;
		}
		if (tmp > key)
			top = middle - 1;
		else
			bottom = middle + 1 ;
	}
	assert(0);
	return -1;
}

template <class _T>
int dichotomy<_T>::insert_at_next(int index,const _T & key)
{
	if (!(index < _values.size()-1))
		return -1;
	//int nNextValue = _values[index].key;
	if (key >= _values[index].key)
		return -1;
	value v;
	v.key = key;
	v.data = NULL;
	_values.insert(_values.begin()+index,v);
	return index;
}

template <class _T>
int dichotomy<_T>::insert_at_prev(int index,const _T & key)
{
	if (index < 0)
		return -1;
	//int nPrvValue = _values[index].key;
	if (key <= _values[index].key)
		return -1;
	value v;
	v.key = key;
	v.data = NULL;
	_values.insert(_values.begin()+index+1,v);
	return index+1;
}

template <class _T>
int dichotomy<_T>::find(const _T & key)
{
	int top = int(_values.size())-1,middle = 0,bottom = 0;

	for (;bottom<=top;)
	{
		middle = (top+bottom)/2;
		if (middle >= int(_values.size()))
			break;
		_T & tmp = _values[middle].key;
		if (tmp == key)
			return middle;
		else
			if (tmp > key)
				top = middle - 1;
			else
				bottom = middle + 1 ;
	}
	return -1;
}

template <class _T>
void dichotomy<_T>::remove(const _T & key)
{
	int id = find(key);
	if (id < 0)
		return;
	remove_by_index(id);
}

template <class _T>
void* dichotomy<_T>::get_data_ptr(int index)
{
	return _values[index].data;
}

template <class _T>
void dichotomy<_T>::set_data_ptr(int index,void* data)
{
	_values[index].data = data;
}

/***/

template <class _T>
tiny_dichotomy<_T>::tiny_dichotomy()
{
	_values.reserve(64);
}

template <class _T>
tiny_dichotomy<_T>::~tiny_dichotomy()
{

}

template <class _T>
void tiny_dichotomy<_T>::remove_by_index(int index)
{
	_values.erase(_values.begin() + index);
}

template <class _T>
int tiny_dichotomy<_T>::insert(const _T & key)
{
	int top = int(_values.size())-1,middle = 0,bottom = 0,id=0;

	if (find(key) > -1)
		return -2;

	if (top < 0 || key > _values[top])
	{
		_values.push_back(key);
		return int(_values.size()) - 1;
	}
	else
		if (_values.size() > 0 && key < _values[0])
		{
			_values.insert(_values.begin(),key);
			return 0;
		}


	_T	tmp;
	for (;bottom<=top;)
	{
		middle = (top + bottom)/2;
		if (middle >= _values.size())
			break;
		tmp = _values[middle];
		if (key > tmp)
		{
			if ( (id = insert_at_next(middle+1,key)) != -1)
				return id;
		}
		else
		{
			if ( (id = insert_at_prev(middle-1,key)) != -1)
				return id;
		}
		if (tmp > key)
			top = middle - 1;
		else
			bottom = middle + 1 ;
	}
	assert(0);
	return -1;
}

template <class _T>
int tiny_dichotomy<_T>::insert_at_next(int index,const _T & key)
{
	if (!(index < _values.size()-1))
		return -1;
	if (key >= _values[index])
		return -1;
	_values.insert(_values.begin()+index,key);
	return index;
}

template <class _T>
int tiny_dichotomy<_T>::insert_at_prev(int index,const _T & key)
{
	if (index < 0)
		return -1;
	if (key <= _values[index])
		return -1;
	_values.insert(_values.begin()+index+1,key);
	return index+1;
}

template <class _T>
int tiny_dichotomy<_T>::find(const _T & key)
{
	int top = _values.size()-1,middle = 0,bottom = 0;

	for (;bottom<=top;)
	{
		middle = (top+bottom)/2;
		if (middle >= _values.size())
			break;
		_T & tmp = _values[middle];
		if (tmp == key)
			return middle;
		else
			if (tmp > key)
				top = middle - 1;
			else
				bottom = middle + 1 ;
	}
	return -1;
}

template <class _T>
void tiny_dichotomy<_T>::remove(const _T & key)
{
	int id = find(key);
	if (id < 0)
		return;
	remove_by_index(id);
}

#endif
