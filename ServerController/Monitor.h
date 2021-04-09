#pragma once

#ifndef __MONITOR_DESCRIPTION_H__
#define __MONITOR_DESCRIPTION_H__

#pragma managed(push, off)

//**********************************************************************************************
class CMonitor
{
protected:
	unsigned long m_NetID;
	int			  m_SelectedIndex;

public:
	CMonitor(unsigned long netid);

public:
	unsigned long GetNetID()					{return m_NetID;}
	void		  SetNetID(unsigned long value)	{m_NetID = value;}
	int			  GetSelectedIndex()			{return m_SelectedIndex;}
	void		  GetSelectedIndex(int value)	{m_SelectedIndex = value;}

public:
	void Process(unsigned long nowtick, unsigned long elapsedtime){}

public:
	__declspec(property(get = GetNetID		  , put = SetNetID		  )) unsigned long NetID;
	__declspec(property(get = GetSelectedIndex, put = GetSelectedIndex)) int		   SelectedIndex;
};

//**********************************************************************************************
#pragma managed(pop)
#endif
