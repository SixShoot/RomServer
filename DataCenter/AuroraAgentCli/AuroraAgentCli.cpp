#include "AuroraAgentCli.h"
#include "../MainProc/Global.h"
#include "../NetWalker_Member/Net_Other/NetDC_OtherChild.h"

enum NetStatus
{
	EM_NET_DISCONNECTED	,
	EM_NET_CONNECTING	,
	EM_NET_CONNECTED
};

AuroraAgentCli* AuroraAgentCli::This = NULL;

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
AuroraAgentCli::AuroraAgentCli()
{
	m_netBridge			= CreateNetBridge();
	m_NetID				= 0xFFFFFFFF;
	m_NetStatus			= EM_NET_DISCONNECTED;
	m_AuroraAgentIP		= "";
	m_AuroraAgentPort	= 0;
	m_LastConnetTick	= GetTickCount();
	m_MemoryBuffer		= NULL;
	m_MemoryBufferSize	= 0;

	m_PGCallBack[EM_PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT]	= &AuroraAgentCli::_PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT;

	IniFileClass* pIni = Global::Ini();

	if (pIni)
	{
		m_AuroraAgentIP		= pIni->Str( "AuroraAgentIP" );	
		m_AuroraAgentPort	= pIni->Int( "AuroraAgentPort" );
	}
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
AuroraAgentCli::~AuroraAgentCli()
{
	DeleteNetBridge(m_netBridge);

	if (m_MemoryBuffer != NULL)
	{
		delete m_MemoryBuffer;
	}
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::Init()
{
	if (This == NULL)
	{
		IniFileClass* pIni = Global::Ini();

		if (pIni)
		{
			std::string AuroraAgentIP = pIni->Str( "AuroraAgentIP" );

			if (!AuroraAgentIP.empty())
			{
				This = new AuroraAgentCli();
				Global::Schedular()->Push( _OnTimeProc , 1000 , NULL , NULL );
			}
		}
	}	
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::Release()
{
	if (This != NULL)
	{
		delete This;
		This = NULL;
	}	
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
int AuroraAgentCli::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{
	if (This != NULL)
	{
		This->Process();
		Global::Schedular()->Push( _OnTimeProc , 1000 , NULL , NULL );
	}	

	return 0;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::Connect()
{
	if (!m_AuroraAgentIP.empty())
	{
		unsigned long dwIP = ConvertIP(m_AuroraAgentIP.c_str());

		m_netBridge->Connect(dwIP, m_AuroraAgentPort, this);
		m_NetStatus = EM_NET_CONNECTING;
		m_LastConnetTick = GetTickCount();
	}
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void* AuroraAgentCli::GetPacketMemory(unsigned long dwSize)
{
	if (dwSize > m_MemoryBufferSize)
	{
		if (m_MemoryBuffer != NULL)
		{
			delete m_MemoryBuffer;
		}

		m_MemoryBufferSize = ((dwSize / 4) + 1) * 4;

		m_MemoryBuffer = NEW BYTE[m_MemoryBufferSize];
	}

	return m_MemoryBuffer;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::Process()
{
	//------------------------------------------------
	//Aurora agent connect
	unsigned long dwNowTick = GetTickCount();

	switch (m_NetStatus)
	{
	case EM_NET_DISCONNECTED:
		{
			if (dwNowTick - m_LastConnetTick >= 2000)
			{
				Connect();
			}
			break;
		}
	case EM_NET_CONNECTING:
		{
			if (dwNowTick - m_LastConnetTick >= 2000)
			{
				m_NetStatus = EM_NET_DISCONNECTED;
			}
			break;
		}		
	case EM_NET_CONNECTED:
		break;
	}
	//------------------------------------------------
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::SAA_PG_AURORA_AGENT_DCtoAA_LOG_DATA(int iLogType, void* pLogData, unsigned long dwDataSize)
{
	unsigned long dwPacketSize = sizeof(PG_AURORA_AGENT_DCtoAA_LOG_DATA) + dwDataSize;

	dwPacketSize = ((dwPacketSize / 4) + 1) * 4;

	void* pData = GetPacketMemory(dwPacketSize);
	memset(pData, 0, dwPacketSize);
	PBYTE pTraveler = (PBYTE)pData;

	PG_AURORA_AGENT_DCtoAA_LOG_DATA* pHeader = (PG_AURORA_AGENT_DCtoAA_LOG_DATA*)pTraveler;
	pHeader->iType = EM_PG_AURORA_AGENT_DCtoAA_LOG_DATA;
	pHeader->iLogType = iLogType;
	pHeader->dwDataSize = dwDataSize;

	pTraveler += sizeof(PG_AURORA_AGENT_DCtoAA_LOG_DATA);

	memcpy(pTraveler, pLogData, dwDataSize);

	m_netBridge->Send(m_NetID, dwPacketSize, pData);
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::SAA_PG_AURORA_AGENT_DCtoAA_ROLE_LEAVE_WORLD( const char* pszAccountName, const char* pszRoleName, int iRoleDBID, int iWorldID, int iZoneID, int iRace, int iVoc, int iVoc_Sub, int iSex, int iLV, int iLV_Sub )
{
	PG_AURORA_AGENT_DCtoAA_ROLE_LEAVE_WORLD Packet;
	strncpy(Packet.szAccountName, pszAccountName, sizeof(Packet.szAccountName));
	Packet.szAccountName[sizeof(Packet.szAccountName) - 1] = 0;
	strncpy(Packet.szRoleName, pszRoleName, sizeof(Packet.szRoleName));
	Packet.szRoleName[sizeof(Packet.szRoleName) - 1] = 0;

	Packet.iRoleDBID = iRoleDBID;
	Packet.iWorldID = iWorldID;
	Packet.iZoneID	= iZoneID;
	Packet.iRace	= iRace;
	Packet.iVoc		= iVoc;
	Packet.iVoc_Sub = iVoc_Sub;
	Packet.iSex		= iSex;
	Packet.iLV		= iLV;
	Packet.iLV_Sub	= iLV_Sub;

	m_netBridge->Send(m_NetID, sizeof(Packet), &Packet);
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::SAA_PG_AURORA_AGENT_DCtoAA_QUERY_PC_BENEFIT(const char* pszAccount, int iDBID, int iZoneID)
{
	PG_AURORA_AGENT_DCtoAA_QUERY_PC_BENEFIT Packet;
	strncpy(Packet.szAccount, pszAccount, sizeof(Packet.szAccount));
	Packet.szAccount[sizeof(Packet.szAccount) - 1] = 0;
	Packet.iDBID = iDBID;
	Packet.iZoneID = iZoneID;

	m_netBridge->Send(m_NetID, sizeof(Packet), &Packet);
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::On_PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT(const char* pszLuaPlot, int iPCBenefit ,int iDBID, int iZoneID)
{
	switch (iPCBenefit)
	{
	case 1:
		{
			CNetDC_OtherChild::SL_RoleRunPlot(iZoneID, pszLuaPlot, iDBID);
		}
		break;
	default:
		break;
	}
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::_PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT(unsigned long dwNetID, unsigned long dwPackSize, void* PG)
{
	PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT* pPkt = (PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT*)(PG);
	On_PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT(pPkt->szLuaPlot, pPkt->iPCBenefit, pPkt->iDBID, pPkt->iZoneID);
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
bool AuroraAgentCli::OnRecv(unsigned long dwNetID, unsigned long dwPackSize, void* PG)
{
	bool Result = false;

	if (PG)
	{
		PG_AURORA_AGENT_BASE *pPkt = reinterpret_cast<PG_AURORA_AGENT_BASE *>(PG);

		PFUNC_NETPG pFunc = m_PGCallBack[pPkt->iType];

		if (pFunc)
		{
			(this->*pFunc)(dwNetID, dwPackSize, PG);
			Result = true;
		}
		else
		{
			Print(LV5, "AuroraAgentCli::OnRecv", "pFunc NULL, iType = %d", pPkt->iType);
		}
	}
	else
	{
		Print(LV5, "AuroraAgentCli::OnRecv", "PG NULL");
	}

	return Result;
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::OnConnectFailed(unsigned long dwNetID, unsigned long dwFailedCode)
{
	m_NetID = dwNetID;
	m_NetStatus = EM_NET_DISCONNECTED;
	Print(LV5, "", "AuroraAgent ConnectFailed, code = %d", dwFailedCode);
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::OnConnect(unsigned long dwNetID)
{
	m_NetID = dwNetID;
	m_NetStatus = EM_NET_CONNECTED;
	Print(LV3, "", "AuroraAgent Connected");
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
void AuroraAgentCli::OnDisconnect(unsigned long dwNetID)
{
	m_NetID = 0xFFFFFFFF;
	m_NetStatus = EM_NET_DISCONNECTED;
	Print(LV3, "", "AuroraAgent Disconnected");
}

//----------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
CEventObj*	AuroraAgentCli::OnAccept(unsigned long dwNetID)
{
	return this;
}