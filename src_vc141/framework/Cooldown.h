#pragma once
//------------------------------------------------------------------------------
/**
    @class CCooldown
    
    (C) 2016 n.lee
*/
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <float.h>

#define CD_WAITING_QUEUE_SIZE	8

//------------------------------------------------------------------------------
/** 
	@brief CCooldown
 */
class CCooldown
{
public:
	CCooldown( float fCDTimeout )
	{
		m_fCDTimeout = fCDTimeout;

		memset(m_arrWaitingQueue, 0, sizeof(m_arrWaitingQueue));

		//
		ResetCD();
	}

	CCooldown()
	{
		m_fCDTimeout = FLT_MAX;

		memset(m_arrWaitingQueue, 0, sizeof(m_arrWaitingQueue));

		//
		ResetCD();
	}

	~CCooldown();

	/// tick until timeout
	bool						TickAndTimeout( float fElapsed )
	{
		if (IsInCD())
		{
			m_fCDElapsed += fElapsed;
			if (m_fCDElapsed>=m_fCDTimeout)
			{
				ResetCD(); // cool down over
			}
		}
		return !IsInCD();
	}

	void						SetCDTimeout( float fCDTimeout )
	{
		m_fCDTimeout = fCDTimeout;
	}

	bool						IsInCD()
	{
		return m_bInCD;
	}

	void						StartCD()
	{
		if (!m_bInCD)
		{
			ResetCD();

			m_bInCD = true;
		}
	}

	void						ResetCD()
	{
		m_bInCD = false;
		m_fCDElapsed = 0.0f;
	}

	void						SetCountdown( float fCountdown )
	{
		StartCD();
		m_fCDElapsed = m_fCDTimeout-fCountdown;
	}

	float						GetCountdown()
	{
		return (IsInCD() ? (m_fCDTimeout-m_fCDElapsed) : 0.0f);
	}

	void						BeginWaiting( int nIndex )
	{
		m_arrWaitingQueue[nIndex] = true;
	}

	void						EndWaiting( int nIndex )
	{
		m_arrWaitingQueue[nIndex] = false;
	}

	bool						IsWaiting( int nIndex )
	{
		return m_arrWaitingQueue[nIndex];
	}

private:
	bool			m_bInCD;
	float			m_fCDElapsed;
	float			m_fCDTimeout;

	bool			m_arrWaitingQueue[CD_WAITING_QUEUE_SIZE];
};

/*EOF*/