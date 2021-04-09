#pragma once

#include ".\NetSrv_PublicEncounter.h"

class CNetSrv_PublicEncounterChild : public CNetSrv_PublicEncounter
{
public:
	CNetSrv_PublicEncounterChild(void){};
	~CNetSrv_PublicEncounterChild(void){};

public:
	static	void Init();
	static  void Release();

public:
	virtual void CliConnect( BaseItemObject* pObj, int iSockID );
	virtual void CliDisconnect( int iSockID );
	virtual void OnRZ_CliRegionChanged(BaseItemObject* pObj, int iRegionID);
};

