#ifndef _PERFORMANCE_
#define _PERFORMANCE_

#include "dichotomy.h"

class	CPeformanceManager : public dichotomy <int>
{
public:
	CPeformanceManager();
	virtual ~CPeformanceManager();

	static	CPeformanceManager*	Singleton();

	void	Print();

public:
	double	m_fLast;
};

class CPerformance
{
public:
	CPerformance(int nPfmID, const char *pName);
	virtual ~CPerformance();

public:
	int		m_nPfmID;
	double 	m_fCreat;
};

#define	_PRO_PERFORMANCE_
#ifdef	_PRO_PERFORMANCE_
#define	PERFORMANCE(id,name)	CPerformance performance(id,name);
#else
#define	PERFORMANCE(id,name)
#endif

#endif


