#pragma once
#ifndef	__AURORA_AGENT_CLI_H__
#define __AURORA_AGENT_CLI_H__

#include "netbridge/NetBridge.h"
#include "functionschedular/functionschedular.h"
#include "PG/PG_AuroraAgent.h"

class AuroraAgentCli;

//********************************************************************************************************
typedef void  (AuroraAgentCli::*PFUNC_NETPG) (unsigned long dwNetID , unsigned long dwSize, void* PG);
//********************************************************************************************************

class AuroraAgentCli : public CEventObj
{
protected:
	CNetBridge*		m_netBridge;		//For Aurora Agent
	unsigned long	m_NetID;
	int				m_NetStatus;
	std::string		m_AuroraAgentIP;
	int				m_AuroraAgentPort;
	unsigned long	m_LastConnetTick;
	void*			m_MemoryBuffer;
	unsigned long	m_MemoryBufferSize;

	std::map<int, PFUNC_NETPG>	m_PGCallBack;

public:
	AuroraAgentCli();
	virtual ~AuroraAgentCli();

public:
	static AuroraAgentCli* This;

public:
	static void Init();
	static void Release();

public:
	static int  _OnTimeProc( SchedularInfo* Obj , void* InputClass );

protected:
	void Connect();
	void* GetPacketMemory(unsigned long dwSize);

public:
	void Process();

public:
	void SAA_PG_AURORA_AGENT_DCtoAA_LOG_DATA(int iLogType, void* pLogData, unsigned long dwDataSize);
	void SAA_PG_AURORA_AGENT_DCtoAA_ROLE_LEAVE_WORLD( const char* pszAccountName, const char* pszRoleName, int iRoleDBID, int iWorldID, int iZoneID, int iRace,
		int iVoc, int iVoc_Sub, int iSex, int iLV, int iLV_Sub );
	void SAA_PG_AURORA_AGENT_DCtoAA_QUERY_PC_BENEFIT(const char* pszAccount, int iDBID, int iZoneID);

protected:
	void On_PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT(const char* pszLuaPlot, int iPCBenefit ,int iDBID, int iZoneID);

protected:
	void _PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT(unsigned long dwNetID, unsigned long dwPackSize, void* PG);

public:
	// CEventObj implementation
	virtual bool		OnRecv(unsigned long dwNetID, unsigned long dwPackSize, void* PG);
	virtual void		OnConnectFailed(unsigned long dwNetID, unsigned long dwFailedCode);
	virtual void		OnConnect(unsigned long dwNetID);
	virtual void		OnDisconnect(unsigned long dwNetID);
	virtual CEventObj*	OnAccept(unsigned long dwNetID);
};

#endif