#include "Performance.h"

#include "toolkit/SimpleTimer.h"

using namespace std;

#ifdef _MSC_VER
	#ifdef _DEBUG
		#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
	#endif
#endif

struct	PFMCOUNTER
{
	char	chName[512];
	int		nCount;
	double	fTotalTime;
};

map<int,double>		g_mapPfmTicks;
map<int,int>		g_mapPfmCounter;
map<int,double>		g_mapPfmTimer;

CPeformanceManager::CPeformanceManager()
{
	m_fLast = CSimpleTimer::Singleton()->GetElapsed();
};

CPeformanceManager::~CPeformanceManager()
{
	for (int i=0;i<_values.size();i++)
		delete (PFMCOUNTER*)_values[i].data;
}

void	CPeformanceManager::Print()
{
	double	fNew 		=  CSimpleTimer::Singleton()->GetElapsed();
	double	fElapsed 	= ( fNew - m_fLast );

	if ( fElapsed > 1.0 )
	{
		int i=0;
		for (i=0;i<size();i++)
		{
			PFMCOUNTER * pfmCounter = (PFMCOUNTER*)get_data_ptr(i);
			fprintf(stderr,"Performance : %s[%d] , %.2f%% , %.2f FPS\n"
				,pfmCounter->chName
				,_values[i].key
				,pfmCounter->fTotalTime * 100.0f
				,float(pfmCounter->nCount/fElapsed));
			pfmCounter->fTotalTime = 0;
			pfmCounter->nCount = 0;
		}
		m_fLast = fNew;
	}
}

CPeformanceManager	*	CPeformanceManager::Singleton()
{
	static	CPeformanceManager	s_pfman;
	return	&s_pfman;
}

CPerformance::CPerformance(int nPfmID, const char *sName)
{
	CPeformanceManager	*	pPfm = CPeformanceManager::Singleton();

	int nIndex = pPfm->find(m_nPfmID = nPfmID);
	PFMCOUNTER * pfmCounter = NULL;
	if ( nIndex < 0 )
	{
		pfmCounter = new PFMCOUNTER;
		pfmCounter->nCount = 0;
		pfmCounter->fTotalTime = 0;
		pPfm->set_data_ptr(pPfm->insert(m_nPfmID),pfmCounter);
	}
	else
	{
		pfmCounter = (PFMCOUNTER*)pPfm->get_data_ptr(nIndex);
	}
	sprintf(pfmCounter->chName,sName);
	m_fCreat = CSimpleTimer::Singleton()->GetElapsed();
	pfmCounter->nCount ++;
};

CPerformance::~CPerformance()
{
	CPeformanceManager	*	pPfm = CPeformanceManager::Singleton();

	int nIndex = pPfm->find(m_nPfmID);
	if ( nIndex < 0 )
		return;

	PFMCOUNTER * pfmCounter = (PFMCOUNTER*)pPfm->get_data_ptr(nIndex);
	pfmCounter->fTotalTime += ( CSimpleTimer::Singleton()->GetElapsed() - m_fCreat );
}
