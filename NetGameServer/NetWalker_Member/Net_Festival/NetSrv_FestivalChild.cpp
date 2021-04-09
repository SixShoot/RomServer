#include "NetSrv_FestivalChild.h"
#include "../../MainProc/LuaPlot/LuaPlot.h"

map<int, StructFestivalDescriptor> CNetSrv_FestivalChild::m_festivalInfo;

bool CNetSrv_FestivalChild::Init()
{
	CNetSrv_Festival::_Init();

	if( This != NULL)
		return false;

	This = NEW( CNetSrv_FestivalChild );

	Global::Schedular()->Push( _OnTimeProc , FESTIVAL_CHECK_TIME_INTERVAL , This , NULL );

	return true;
}

bool CNetSrv_FestivalChild::Release()
{
	return true;
}

void CNetSrv_FestivalChild::LoadFestivalInfo( void* Data )
{
	map<int, StructFestivalDescriptor> tmpInfo;
	tmpInfo.clear();

	//-------------------------------------------
	//receive all festival info into tmp map
	PBYTE ptr				= (PBYTE)Data;

	// PacketID
	ptr						+= sizeof(DWORD);

	// Festival number
	int iFestivalNum         = *((int*)ptr);
	ptr						+= sizeof(DWORD);

	// Festival Data
	for ( int i = 0; i < iFestivalNum; ++i)
	{
		StructFestivalDescriptor tmpData;
		tmpData.vecTimes.clear();

		tmpData.iFestivalID      = *((int*)ptr);
		ptr						+= sizeof(DWORD);

		strcpy(tmpData.Name, (char*)ptr);
		ptr						+= _FESTIVAL_NAME_LENGTH;

		strcpy(tmpData.StartPlot, (char*)ptr);
		ptr						+= _MAX_LUA_FUNTIONNAMESIZE_;

		strcpy(tmpData.EndPlot, (char*)ptr);
		ptr						+= _MAX_LUA_FUNTIONNAMESIZE_;

		strcpy(tmpData.SceneControlString, (char*)ptr);
		ptr						+= _FESTIVAL_SCENE_CONTROL_STRING_LENGTH;

		int iTimeNum			= *((int*)ptr);
		ptr						+= sizeof(DWORD);

		Print(LV2, "", "Receive festival data : Name = %s, FestivalID = %d, StartPlot = %s, EndPlot = %s", tmpData.Name, tmpData.iFestivalID, tmpData.StartPlot, tmpData.EndPlot);

		//Time Data
		for (int j = 0; j < iTimeNum; ++j)
		{
			StructFestivalTime* pTime = (StructFestivalTime*)ptr;
			tmpData.vecTimes.push_back(*pTime);
			ptr		  		   += sizeof(StructFestivalTime);

			Print(LV2, "", "Festival time : StartTime = %d, EndTime = %d", pTime->dwStartTime, pTime->dwEndTime);
		}

		
		tmpInfo[tmpData.iFestivalID] = tmpData;
	}

	// we don't have festival info yet.
	if (m_festivalInfo.empty())
	{
		m_festivalInfo = tmpInfo;
	}
	// we have festival info, update if needed.
	else
	{
		//-------------------------------------------
		map<int, StructFestivalDescriptor>::iterator it;
		map<int, StructFestivalDescriptor>::iterator itTmp;
		int iFestivalID = 0;

		for (itTmp = tmpInfo.begin(); itTmp != tmpInfo.end(); itTmp++)
		{
			iFestivalID = itTmp->first;
			it = m_festivalInfo.find(iFestivalID);

			//add new festival data
			if (it == m_festivalInfo.end())
			{				
				m_festivalInfo[iFestivalID] = itTmp->second;
				Print(LV2, "", "Add new festival name = %s", itTmp->second.Name);
			}
			else
			{
				//update old data with new one
				it->second.vecTimes.clear();
				strcpy(it->second.Name, itTmp->second.Name);
				strcpy(it->second.StartPlot, itTmp->second.StartPlot);
				strcpy(it->second.EndPlot, itTmp->second.EndPlot);
				strcpy(it->second.SceneControlString, itTmp->second.SceneControlString);
				it->second.vecTimes       = itTmp->second.vecTimes;

				Print(LV2, "", "Update festival %s",  itTmp->second.Name);
			}
		}

		//remove unused festival
		for (it = m_festivalInfo.begin(); it != m_festivalInfo.end(); it++)
		{
			iFestivalID = it->first;
			itTmp = tmpInfo.find(iFestivalID);

			//target festival ID does not appear in new festival info
			if (itTmp == tmpInfo.end())
			{
				Print(LV2, "", "Remove festival %s",  itTmp->second.Name);

				//we remove it from festival info map
				it = m_festivalInfo.erase(it);

				if (it == m_festivalInfo.end())
				{
					break;
				}
			}
		}
	}
}


void CNetSrv_FestivalChild::CliConnect( int CliID , string Account)
{
	map<int, StructFestivalDescriptor>::iterator it;

	for (it = m_festivalInfo.begin(); it != m_festivalInfo.end(); it++)
	{
		SendToClient_FestivalStatus(CliID, it->second.iFestivalID, it->second.iStatus, it->second.SceneControlString);
	}
}

void CNetSrv_FestivalChild::RequestFestivalInfo()
{
	PG_Festival_LtoD_RequestFestivalInfo Send;
	Send.iZoneID = Global::Ini()->ZoneID;

	SendToDBCenter( sizeof( Send ) , &Send );
}

void CNetSrv_FestivalChild::SendToClient_FestivalStatus(int iCliID, int iFestivalID, int iStatus, const char* pszControlString)
{
	PG_Festival_LtoC_FestivalStatus Send;
	Send.iFestivalID = iFestivalID;
	Send.iStatus	 = iStatus;
	strcpy(Send.strSceneControl, pszControlString);

	SendToCli(iCliID, sizeof( Send ) , &Send);
}

void CNetSrv_FestivalChild::SendToAllClient_FestivalStatus(int iFestivalID, int iStatus, const char* pszControlString)
{
	PG_Festival_LtoC_FestivalStatus Send;
	Send.iFestivalID = iFestivalID;
	Send.iStatus	 = iStatus;
	strcpy(Send.strSceneControl, pszControlString);

	set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
	set< BaseItemObject*>::iterator   PlayerObjIter;
	for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
	{
		BaseItemObject* Obj = *PlayerObjIter;

		RoleDataEx* Role = Obj->Role();

		SendToCli_ByProxyID( Role->TempData.Sys.SockID , Role->TempData.Sys.ProxyID , sizeof( Send ) , &Send );
	}
}

int CNetSrv_FestivalChild::CheckFestival(StructFestivalDescriptor& fesDesc)
{
	DWORD dwNow	= RoleDataEx::G_Now + RoleDataEx::G_TimeZone * 60 *60;

	int Result = EM_FESTIVAL_STATUS_END;

	for (int i = 0; i< fesDesc.vecTimes.size(); ++i)
	{
		DWORD dwStartTime = fesDesc.vecTimes[i].dwStartTime;
		DWORD dwEndTime   = fesDesc.vecTimes[i].dwEndTime;

		if ((dwNow >= dwStartTime ) && (dwNow <= dwEndTime))
		{
			Result = EM_FESTIVAL_STATUS_BEGIN;
		}
	}

	return Result;
}

void CNetSrv_FestivalChild::OnFestivalCheck()
{
	BaseItemObject* pObj;
	map<int, int> mapStatusChange;
	map<int, StructFestivalDescriptor>::iterator it;
	map<int, int>::iterator itStatusChange;

	//--------------------------------------------------------------------------
	//Check festival time
	mapStatusChange.clear();

	for (it = m_festivalInfo.begin(); it != m_festivalInfo.end(); it++)
	{
		int oldStatus = it->second.iStatus;
		int newStatus = CheckFestival(it->second);

		//Status Change
		if (newStatus != oldStatus)
		{			
			mapStatusChange[it->first] = newStatus;
		}
	}

	//There is no festival status change
	if(mapStatusChange.empty())
		return;
	//--------------------------------------------------------------------------

	//we have festival begin or stop
	set<BaseItemObject* >&	NpcObjSet = *(BaseItemObject::NPCObjSet());
	set< BaseItemObject*>::iterator   NpcObjIter;

	for( NpcObjIter = NpcObjSet.begin() ; NpcObjIter != NpcObjSet.end() ; NpcObjIter++ )
	{		
		pObj = *NpcObjIter;

		if (pObj != NULL)
		{
			//find the obj template
			int	iObjGUID				= pObj->Role()->BaseData.ItemInfo.OrgObjID;

			GameObjDbStruct* pNPCObj	= g_ObjectData->GetObj( iObjGUID );

			if (pNPCObj != NULL)
			{
				//determine that is npc's tagID(FestivalID) in set of changed festival
				itStatusChange = mapStatusChange.find(pNPCObj->NPC.iTagID);

				//npc is in the set
				if (itStatusChange != mapStatusChange.end())
				{
					RoleDataEx* pRole    = pObj->Role();
					string strRoleName   = Global::GetRoleName_ASCII( pRole );

					switch (itStatusChange->second)
					{
					case EM_FESTIVAL_STATUS_BEGIN:
						{
							LuaPlotClass::Show(pObj->GUID(), pRole->RoomID());

							//Resume all lua plot associated with the obj
							pObj->PlotVM()->StartAllLuaFunc();

							Print(LV2, "Festival", "Show NPC : Name = %s, GUID = %d, ZoneID = %d, RoomID = %d", strRoleName.c_str(), pObj->GUID(), pRole->BaseData.ZoneID, pRole->RoomID());
							break;
						}
					case EM_FESTIVAL_STATUS_END:
						{
							//Pause all lua plot associated with the obj
							pObj->PlotVM()->PauseAllLuaFunc();

							//DelFromPartition
							LuaPlotClass::Hide( pObj->GUID() );

							Print(LV2, "Festival", "Hide NPC : Name = %s, GUID = %d, ZoneID = %d, RoomID = %d", strRoleName.c_str(), pObj->GUID(), pRole->BaseData.ZoneID, pRole->RoomID());
							break;
						}
					}
				}
			}
		}
	}

	//Run start or end script of festivals
	for (itStatusChange = mapStatusChange.begin(); itStatusChange != mapStatusChange.end(); itStatusChange++)
	{
		int iFestivalID = itStatusChange->first;		
		int iStatus		= itStatusChange->second;

		string _StartPlot = m_festivalInfo[iFestivalID].StartPlot;
		string _EndPlot   = m_festivalInfo[iFestivalID].EndPlot;

		switch (iStatus)
		{
		case EM_FESTIVAL_STATUS_BEGIN:
			{
				if (!_StartPlot.empty())
				{
					Print(LV2, "Festival", "Run initial plot = %s", _StartPlot.c_str());
					//festival initial plot 
					LUA_VMClass::PCall(_StartPlot.c_str(), -1, -1);
				}

				m_festivalInfo[iFestivalID].iStatus = iStatus;

				Print(LV2, "", "Festival %s begin", m_festivalInfo[iFestivalID].Name);

				SendToAllClient_FestivalStatus(iFestivalID, iStatus, m_festivalInfo[iFestivalID].SceneControlString);
				break;
			}
		case EM_FESTIVAL_STATUS_END:
			{
				if (!_EndPlot.empty())
				{
					Print(LV2, "Festival", "Run end plot = %s", _EndPlot.c_str());

					//festival end plot
					LUA_VMClass::PCall(_EndPlot.c_str(), -1, -1);
				}

				m_festivalInfo[iFestivalID].iStatus = iStatus;

				Print(LV2, "", "Festival %s end", m_festivalInfo[iFestivalID].Name);
				SendToAllClient_FestivalStatus(iFestivalID, iStatus, m_festivalInfo[iFestivalID].SceneControlString);
				break;
			}
		}
	}
}

int  CNetSrv_FestivalChild::_OnTimeProc( SchedularInfo* Obj , void* InputClass )
{
	//check if all npc loaded
	if (Global::St()->IsLoadAllNpcComplete)
	{
		//There is no festival got from datacenter
		if (m_festivalInfo.empty())
		{
			Print(LV2, "Festival", "Send request to datacenter");
			//send request to datacenter
			RequestFestivalInfo();
		}

		//check festival status
		OnFestivalCheck();
	}

	//check every min
	Global::Schedular()->Push( _OnTimeProc , FESTIVAL_CHECK_TIME_INTERVAL , This , NULL );
	return 0;
}