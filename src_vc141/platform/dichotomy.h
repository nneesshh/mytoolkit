#ifndef _DICHOTOMY_SORT_
#define _DICHOTOMY_SORT_

#include <vector>
#include <map>

template < class _T > class tiny_dichotomy
{
public:
	tiny_dichotomy();
	virtual ~tiny_dichotomy();

public:
	void			clear()
	{
		_values.clear();
	}

	_T				get(int index)
	{
		return	_values[index];
	}

	int				size()
	{
		return	_values.size();
	}
	
	void			remove_by_index(int index);
	
	int				insert(const _T & key);
	void			remove(const _T & key);
	int				find(const _T & key);
	
public:
	std::vector <_T>		_values;
	
private:
	int				insert_at_next(int index,const _T & key);
	int				insert_at_prev(int index,const _T & key);
};

template < class _T > class dichotomy
{
public:
	dichotomy();
	virtual ~dichotomy();

public:
	struct	value
	{
		_T		key;
		void*	data;
	};

public:
	void			clear()
	{
		_values.clear();
	}

	_T				get(int index)
	{
		return	_values[index].key;
	}

	int				size()
	{
		return	_values.size();
	}
	
	void			remove_by_index(int index);
	
	int				insert(const _T & key);
	void			remove(const _T & key);
	int				find(const _T & key);
	
public:
	void			set_data_ptr(int index,void* pPtr);
	void*			get_data_ptr(int index);

public:
	std::vector <value>	_values;
	
private:
	int				insert_at_next(int index,const _T & key);
	int				insert_at_prev(int index,const _T & key);
};

#include "dichotomy.inl"

#endif

