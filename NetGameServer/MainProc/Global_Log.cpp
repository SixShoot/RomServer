
#include "Global.h"

#include "../NetWalker_Member/NetWakerGSrvInc.h" // Net_DCBase

#include "FileVersion/FileVersion.h"
#include "cpu_usage/CpuUsage.h"

#if USE_NEW_LOG_CENTER_SERVER
	#include "../NetWalker_Member/Net_LCBase/PG_LCBase.h"
	#include "../NetWalker_Member/Net_LCBase/PG_LCBase_DynamicBuffer.h"
#endif

//////////////////////////////////////////////////////////////////////////

void Global::_ProxyPingLog(int ProxyID, int Time)
{
#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0010); // 16 bytes
    pkBuf->SetCommandType(LC_CMD_ProxyPingLog);
    pkBuf->PushInt32(ProxyID);
    pkBuf->PushInt32(Time);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[512];
    sprintf_s(Buf, sizeof(Buf), "Insert ProxyPingLog(Ping,ProxyID) Values (%d,%d)"
        , Time
        , ProxyID);
    Net_DCBase::LogSqlCommand(Buf);
#endif
}

//////////////////////////////////////////////////////////////////////////

void Global::Log_LocalServerProcTime(int MinTime, int MaxTime, int DelayTime, int ZoneID, int WorldID)
{
#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0040); // 64 bytes
    pkBuf->SetCommandType(LC_CMD_LocalServerProcTimeLog);
    pkBuf->PushInt32(MinTime);
    pkBuf->PushInt32(MaxTime);
    pkBuf->PushInt32(DelayTime);
    pkBuf->PushInt32(ZoneID);
    pkBuf->PushInt32(WorldID);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[512];
    sprintf_s(Buf, sizeof(Buf), "Insert LocalServerProcTimeLog(MinNetProcTime,MaxNetProcTime,LastProcDelayTime,ZoneID,WorldID) Values(%d,%d,%d,%d,%d)"
        , MinTime
        , MaxTime
        , DelayTime
        , ZoneID
        , WorldID);
    Net_DCBase::LogSqlCommand(Buf);
#endif
}

void	Global::Log_GMToolsGMCommand( const char* Account , int ManageLv , int ToRoleDBID , const char* Command )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String( Command );

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0120); // 256 + 32 bytes
    pkBuf->SetCommandType(LC_CMD_GMToolsCommandLog);
    pkBuf->PushVarChar(const_cast<char*>(Account));
    pkBuf->PushInt32(RoleDataEx::G_ZoneID);
    pkBuf->PushInt32(ManageLv);
    pkBuf->PushInt32(ToRoleDBID);
    pkBuf->PushNVarChar(const_cast<wchar_t*>(charTransfer.GetWCString()));
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    string CmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
    char Buf[2048];
	
	string AccountX =  StringToSqlX(Account, _MAX_ACCOUNT_SIZE_);
	
    sprintf_s( Buf, sizeof(Buf), "INSERT INTO GMToolsCommandLog(Account,ZoneID,ManageLv,ToRoleDBID,Command) VALUES (CAST( %s AS varchar(65) ),%d,%d,%d,CAST(%s AS nvarchar(4000)))"
        , AccountX.c_str()
        , RoleDataEx::G_ZoneID
        , ManageLv
        , ToRoleDBID
        , CmdBinStr.c_str() );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_GMCommand( RoleDataEx* Owner , const char* GMCommand , const char* RoleName )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Owner->IsPlayer() == false )
		return;

	//§âUnicode Utf-8 Âà¬° char*
	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String( GMCommand );

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0820); // 2048 + 32 bytes
    pkBuf->SetCommandType(LC_CMD_GMCommandLog);
    pkBuf->PushInt32(Owner->DBID());
    pkBuf->PushInt32(Owner->G_ZoneID);
    pkBuf->PushInt32(static_cast<int>(Owner->Pos()->X));
    pkBuf->PushInt32(static_cast<int>(Owner->Pos()->Z));
    pkBuf->PushNVarChar(const_cast<wchar_t*>(charTransfer.GetWCString()));
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    string CmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
    char Buf[2048];
    sprintf_s( Buf, sizeof(Buf), "INSERT INTO GMCommandLog(PlayerDBID,ZoneID,PosX,PosZ,Command) VALUES (%d,%d,%d,%d,CAST(%s AS nvarchar(4000)))"
        , Owner->DBID()
        , Owner->G_ZoneID
        , int( Owner->Pos()->X )
        , int( Owner->Pos()->Z )
        , CmdBinStr.c_str() );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_ItemTrade( int FromPlayerDBID , int FromNpcDBID , int ToPlayerDBID , int X , int Z , ActionTypeENUM Type , ItemFieldStruct& Item , const char* Note)
{
	GameObjDbStructEx* ItemObj = Global::GetObjDB( Item.OrgObjID );
	if( ItemObj == NULL  )
		return;

	if(		Type == EM_ActionType_Tran 
		||	Type == EM_ActionType_RefineLvUp 
		||	Type == EM_ActionType_RefineLvDn 
		||	Type == EM_ActionType_RefineNone )
	{

	}
	else
	{
		ServerItemLogStruct& Info  = _St->SrvItemManage[ ItemObj->GUID ];
		Info.ItemID = ItemObj->GUID;
		Info.CreateCount += Item.Count;
	}
	
	if( ItemObj->MaxHeap > 1 && Global::Ini()->IsItemTradeLog_Detail == false )
		return;

	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsItemTradeLog == false )
		return;

	GameObjDbStructEx* ItemDB = Global::GetObjDB( Item.OrgObjID );
	if( ItemDB == NULL )
		return;

	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String( Note );

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x1100); // 4096 + 256 bytes
    pkBuf->SetCommandType(LC_CMD_ItemTradeLog);
    pkBuf->PushInt32(FromPlayerDBID);
    pkBuf->PushInt32(FromNpcDBID);
    pkBuf->PushInt32(ToPlayerDBID);
    pkBuf->PushInt32(Type);
    pkBuf->PushInt32(RoleDataEx::G_ZoneID);
    pkBuf->PushInt32(X);
    pkBuf->PushInt32(Z);
    pkBuf->PushInt32(Item.OrgObjID);
    pkBuf->PushInt32(Item.Serial);
    pkBuf->PushInt32(Item.CreateTime);
    pkBuf->PushInt32(Item.Count);
    pkBuf->PushInt32(Item.ExValue);
    pkBuf->PushInt32(Item.Durable);
    pkBuf->PushInt32(Item.Mode._Mode);
    pkBuf->PushInt32(Item.ImageObjectID);
    pkBuf->PushBinary(Item.Ability, sizeof(Item.Ability));
    pkBuf->PushNVarChar(const_cast<wchar_t*>(charTransfer.GetWCString()));
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    string CmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
    string AbilityBinStr = StringToSqlX( Item.Ability , sizeof(Item.Ability) , false );
    char Buf[8096];
    sprintf_s( Buf, sizeof(Buf), "Insert ItemTradeLog(TargetPlayerDBID,TargetNpcDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,ItemID,ItemSerial,ItemCreateTime,ItemCount,ItemExValue,Note,ItemDurable,ItemMode,ItemImageObjectID,ItemAbility) Values(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,CAST(%s AS nvarchar),%d,%d,%d,cast( %s As binary(32) ))"
        , FromPlayerDBID
        , FromNpcDBID
        , ToPlayerDBID
        , Type
        , RoleDataEx::G_ZoneID
        , X 
        , Z 
        , Item.OrgObjID
        , Item.Serial	 
        , Item.CreateTime
        , Item.Count	
        , Item.ExValue 
        , CmdBinStr.c_str() 
        , Item.Durable
        , Item.Mode._Mode
        , Item.ImageObjectID
        , AbilityBinStr.c_str() );
    Net_DCBase::LogSqlCommand( Buf );	
#endif
}

void		Global::Log_ItemServerLog	( )
{
	if( Global::Ini()->IsServerTotalItemLog == false )
		return;

	map< int , ServerItemLogStruct >::iterator Iter;

	int	BeginTime = MytimeGetTime();

	for( Iter = _St->SrvItemManage.begin() ; Iter != _St->SrvItemManage.end() ; Iter++ 	)
	{
		ServerItemLogStruct& Info = Iter->second;

#if USE_NEW_LOG_CENTER_SERVER
        PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0020); // 32 bytes
        pkBuf->SetCommandType(LC_CMD_ServerItemLog);
        pkBuf->PushInt32(RoleDataEx::G_ZoneID);
        pkBuf->PushInt32(Info.ItemID);
        pkBuf->PushInt32(Info.CreateCount);
        pkBuf->PushInt32(Info.DestroyCount);
        SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
        delete pkBuf;
#else
        char Buf[2048];
        sprintf_s( Buf, sizeof(Buf), "Insert ServerItemLog(ZoneID,ItemID,CreateCount,DestroyCount) Values(%d,%d,%d,%d)"
            , RoleDataEx::G_ZoneID
            , Info.ItemID
            , Info.CreateCount
            , Info.DestroyCount );	
        Net_DCBase::LogSqlCommand( Buf );	
#endif
	}

	int ProcTime = MytimeGetTime() - BeginTime;

	Print( LV3 , "Log_ItemServerLog" , "SrvItemManage Count=%d ProcTime=%d" , _St->SrvItemManage.size() , ProcTime );
	_St->SrvItemManage.clear();
}

void	Global::Log_ItemDestroy( RoleDataEx* Role , ActionTypeENUM Type , ItemFieldStruct& Item , int ItemCount , int NPCDBID , const char* Note )
{
	if( ItemCount == -1 )
		ItemCount = Item.Count;

	GameObjDbStructEx* ItemObj = Global::GetObjDB( Item.OrgObjID );
	if( ItemObj == NULL )
		return;

	ServerItemLogStruct& Info  = _St->SrvItemManage[ ItemObj->GUID ];
	Info.ItemID = ItemObj->GUID;
	Info.DestroyCount += ItemCount;

	if( Role->IsPlayer() == false )
		return;

	//§â¤£¥i°ïÅ|ªºª««~°µLog
	if( ItemObj->MaxHeap == 1 )
	{
		Role->PlayerSelfData->DestroyItemLog.Push( Item );
	}

	//////////////////////////////////////////////////////////////////////////
	//¦pªG¬°ª««~¡A¨Ã¥B¦³§R°£³Bµo¼@±¡
	if( ItemObj->IsItem_Pure() && strlen( ItemObj->Item.OnDestroyScript ) != 0 )
	{
		LUA_VMClass::PCallByStrArg( ItemObj->Item.OnDestroyScript , Role->GUID() , Role->GUID() );
	}
	//////////////////////////////////////////////////////////////////////////

	if( Global::Ini()->IsSaveLog == false )
		return;

	if( ItemObj->Mode.DepartmentStore == true )
	{
		Log_DepartmentStore	( Item , Type , ItemCount , 0 , 0 , 0 , Role->DBID() );
	}

	if( Global::Ini()->IsItemTradeLog == false )
		return;

	if( ItemObj->MaxHeap > 1 && Global::Ini()->IsItemTradeLog_Detail == false )
		return;

	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String( Note );

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x1100); // 4096 + 256 bytes
    pkBuf->SetCommandType(LC_CMD_ItemTradeLog);
    pkBuf->PushInt32(NPCDBID);
    pkBuf->PushInt32(-1);
    pkBuf->PushInt32(Role->DBID());
    pkBuf->PushInt32(Type);
    pkBuf->PushInt32(RoleDataEx::G_ZoneID);
    pkBuf->PushInt32(static_cast<int>(Role->Pos()->X));
    pkBuf->PushInt32(static_cast<int>(Role->Pos()->Z));
    pkBuf->PushInt32(Item.OrgObjID);
    pkBuf->PushInt32(Item.Serial);
    pkBuf->PushInt32(Item.CreateTime);
    pkBuf->PushInt32(Item.Count);
    pkBuf->PushInt32(Item.ExValue);
    pkBuf->PushInt32(Item.Durable);
    pkBuf->PushInt32(Item.Mode._Mode);
    pkBuf->PushInt32(Item.ImageObjectID);
    pkBuf->PushBinary(Item.Ability, sizeof(Item.Ability));
    pkBuf->PushNVarChar(const_cast<wchar_t*>(charTransfer.GetWCString()));
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    string CmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
    string AbilityBinStr = StringToSqlX( Item.Ability , sizeof(Item.Ability) , false );
    char Buf[8096];
    sprintf_s( Buf, sizeof(Buf), "Insert ItemTradeLog(PlayerDBID,TargetNpcDBID,ActionType,ZoneID,PosX,PosZ,ItemID,ItemSerial,ItemCreateTime,ItemExValue,ItemCount,Note,ItemDurable,ItemMode,ItemImageObjectID,ItemAbility) Values(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,CAST(%s AS nvarchar),%d,%d,%d,cast( %s As binary(32) ))"
        , Role->DBID()
        , NPCDBID
        , Type
        , Role->G_ZoneID
        , int(Role->Pos()->X)
        , int(Role->Pos()->Z)
        , Item.OrgObjID
        , Item.Serial		
        , Item.CreateTime		
        , Item.ExValue 
        , ItemCount 
        , CmdBinStr.c_str() 
        , Item.Durable
        , Item.Mode._Mode
        , Item.ImageObjectID
        , AbilityBinStr.c_str() );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}
  
void	Global::Log_Money( int FromPlayerDBID , int FromNpcDBID , RoleDataEx* ToRole , ActionTypeENUM Type , int Money )
{

	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsMoneyLog == false )
		return;

	//int ToPlayerDBID , int X , int Z

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0080); // 128 bytes
    pkBuf->SetCommandType(LC_CMD_MoneyLog);
    pkBuf->PushInt32(FromNpcDBID);
    pkBuf->PushInt32(FromPlayerDBID);
    pkBuf->PushInt32(ToRole->DBID());
    pkBuf->PushInt32(Type);
    pkBuf->PushInt32(RoleDataEx::G_ZoneID);
    pkBuf->PushInt32(static_cast<int>(ToRole->X()));
    pkBuf->PushInt32(static_cast<int>(ToRole->Z()));
    pkBuf->PushInt32(ToRole->BodyMoney() - Money);
    pkBuf->PushInt32(ToRole->BodyMoney());
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[2048];
    sprintf_s( Buf, sizeof(Buf), "Insert MoneyLog(TargetNPCDBID,TargetPlayerDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,NewMoney,OldMoney) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d)"
        , FromNpcDBID
        , FromPlayerDBID
        , ToRole->DBID()
        , Type
        , RoleDataEx::G_ZoneID
        , int( ToRole->X() )
        , int( ToRole->Z() )
        , ToRole->BodyMoney()
        , ToRole->BodyMoney() - Money );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_MoneyEx( int FromPlayerDBID , int FromNpcDBID , RoleDataEx* ToRole , ActionTypeENUM Type , int MoneyType , int Money , int OldMoney )
{

	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsMoneyLog == false )
		return;

	char Buf[2048];
	sprintf_s( Buf, sizeof(Buf), "Insert MoneyLog(TargetNPCDBID,TargetPlayerDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,NewMoney,OldMoney,MoneyType) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
		, FromNpcDBID
		, FromPlayerDBID
		, ToRole->DBID()
		, Type
		, RoleDataEx::G_ZoneID
		, int( ToRole->X() )
		, int( ToRole->Z() )
		, Money
		, OldMoney
		, MoneyType );
	Net_DCBase::LogSqlCommand( Buf );

}

void	Global::Log_Money_Account( int FromPlayerDBID , int FromNpcDBID , RoleDataEx* ToRole , ActionTypeENUM Type , int Money )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsAccountMoneyLog == false )
		return;

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0080); // 128 bytes
    pkBuf->SetCommandType(LC_CMD_AccountMoneyLog);
    pkBuf->PushInt32(FromNpcDBID);
    pkBuf->PushInt32(FromPlayerDBID);
    pkBuf->PushInt32(ToRole->DBID());
    pkBuf->PushInt32(Type);
    pkBuf->PushInt32(RoleDataEx::G_ZoneID);
    pkBuf->PushInt32(static_cast<int>(ToRole->X()));
    pkBuf->PushInt32(static_cast<int>(ToRole->Z()));
    pkBuf->PushInt32(ToRole->BodyMoney_Account());
    pkBuf->PushInt32(ToRole->BodyMoney_Account() - Money);
    pkBuf->PushInt32(ToRole->Level());
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[2048];
    sprintf_s( Buf, sizeof(Buf), "Insert AccountMoneyLog(TargetNPCDBID,TargetPlayerDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,NewAccountMoney,OldAccountMoney,Lv) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
        , FromNpcDBID
        , FromPlayerDBID
        , ToRole->DBID()
        , Type
        , RoleDataEx::G_ZoneID
        , int( ToRole->X() )
        , int( ToRole->Z() )
        , ToRole->BodyMoney_Account()
        , ToRole->BodyMoney_Account() - Money 
        , ToRole->Level() );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_Money_Bonus( int FromPlayerDBID , int FromNpcDBID , RoleDataEx* ToRole , ActionTypeENUM Type , int Money )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsAccountMoneyLog == false )
		return;

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0080); // 128 bytes
    pkBuf->SetCommandType(LC_CMD_BonusMoneyLog);
    pkBuf->PushInt32(FromNpcDBID);
    pkBuf->PushInt32(FromPlayerDBID);
    pkBuf->PushInt32(ToRole->DBID());
    pkBuf->PushInt32(Type);
    pkBuf->PushInt32(RoleDataEx::G_ZoneID);
    pkBuf->PushInt32(static_cast<int>(ToRole->X()));
    pkBuf->PushInt32(static_cast<int>(ToRole->Z()));
    pkBuf->PushInt32(ToRole->PlayerBaseData->Money_Bonus);
    pkBuf->PushInt32(ToRole->PlayerBaseData->Money_Bonus - Money);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[2048];
    sprintf_s( Buf, sizeof(Buf), "Insert BonusMoneyLog(TargetNPCDBID,TargetPlayerDBID,PlayerDBID,ActionType,ZoneID,PosX,PosZ,NewMoney,OldMoney) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d)"
        , FromNpcDBID
        , FromPlayerDBID
        , ToRole->DBID()
        , Type
        , RoleDataEx::G_ZoneID
        , int( ToRole->X() )
        , int( ToRole->Z() )
        , ToRole->PlayerBaseData->Money_Bonus
        , ToRole->PlayerBaseData->Money_Bonus - Money );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_Quest( RoleDataEx* pRole, int iQuestID, int iQuestStatus )
{
	
	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsQuestLog == false )
		return;

	if( pRole->IsPlayer() == false )
		return;

	GameObjDbStructEx* pQuestObj = Global::GetObjDB( iQuestID );
	if( pQuestObj == NULL )
		return;

	int	iQuestType = 0;

	if( pQuestObj->QuestDetail.iCheck_Loop == false )
		iQuestType = 0;
	else
		iQuestType = 1;

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0080); // 128 bytes
    pkBuf->SetCommandType(LC_CMD_QuestLog);
    pkBuf->PushInt32(pRole->Base.DBID);
    pkBuf->PushInt32(pRole->G_ZoneID);
    pkBuf->PushInt32(static_cast<int>(pRole->Pos()->X));
    pkBuf->PushInt32(static_cast<int>(pRole->Pos()->Z));
    pkBuf->PushInt32(pRole->BaseData.Voc);
    pkBuf->PushInt32(pRole->TempData.Attr.Level);
    pkBuf->PushInt32(iQuestID);
    pkBuf->PushInt32(iQuestType);
    pkBuf->PushInt32(iQuestStatus);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[2048];
/*
	sprintf( Buf , "Insert PlayerLog( PlayerDBID, ZoneID, PosX, PosZ, Job, Lv, QuestID, QuestType, QuestStatus ) Values( %d, %d, %d, %d, %d, %d, %d, %d, %d)"
		, pRole->Base.DBID
		, pRole->G_ZoneID
		, int(pRole->Pos()->X)
		, int(pRole->Pos()->Z)
		, pRole->BaseData.Voc
		, pRole->TempData.Attr.Level
		, iQuestID
		, iQuestType
		, iQuestStatus );
*/
	sprintf_s( Buf, sizeof(Buf), "Insert QuestLog(PlayerDBID,ZoneID,PosX,PosZ,Job,Lv,QuestID,QuestType,QuestStatus) Values(%d,%d,%d,%d,%d,%d,%d,%d,%d)"
		, pRole->Base.DBID
		, pRole->G_ZoneID
		, int(pRole->Pos()->X)
		, int(pRole->Pos()->Z)
		, pRole->BaseData.Voc
		, pRole->TempData.Attr.Level
		, iQuestID
		, iQuestType
		, iQuestStatus );
	Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_PlayerActionLog( RoleDataEx* Role )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsPlayerActionLog == false )
		return;

	if( Role->IsPlayer() != true )
		return;

	//¤Q¤ÀÄÁ¦s¤@¦¸
	//if( Role->BaseData.PlayTime - Role->PlayerTempData->Log.SavePlayTime < 10 )
	//	return;

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0100); // 256 bytes
    pkBuf->SetCommandType(LC_CMD_PlayerActionLog);
    pkBuf->PushInt32(Role->DBID());
    pkBuf->PushInt32(Role->PlayerBaseData->Body.Money - Role->PlayerTempData->Log.LastMoney);
    pkBuf->PushInt32(Role->PlayerBaseData->Body.Money_Account - Role->PlayerTempData->Log.LastMoney_Account);
    pkBuf->PushInt32(Role->PlayerTempData->Log.ActionMode._Mode);
    pkBuf->PushInt32(Role->PlayerTempData->Log.DeadCount);
    pkBuf->PushInt32(Role->PlayerTempData->Log.KillCount);
    pkBuf->PushInt32(Role->PlayerTempData->Log.Exp);
    pkBuf->PushInt32(Role->PlayerTempData->Log.Exp_TP);
    pkBuf->PushInt32(Role->BaseData.Voc);
    pkBuf->PushInt32(Role->BaseData.Voc_Sub);
    pkBuf->PushInt32(Role->TempData.Attr.Level);
    pkBuf->PushInt32(Role->BaseData.PlayTime);
    pkBuf->PushInt32(Role->PlayerBaseData->AbilityList[Role->BaseData.Voc].PlayTime);
    pkBuf->PushInt32(Role->PlayerBaseData->Body.Money);
    pkBuf->PushInt32(Role->PlayerBaseData->Body.Money_Account);
    pkBuf->PushInt32(static_cast<int>(Role->X()));
    pkBuf->PushInt32(static_cast<int>(Role->Z()));
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[2048];
    sprintf_s( Buf, sizeof(Buf), "Insert PlayerActionLog(PlayerDBID,Money,Money_Account,ActionMode,DeadCount,KillCount,Exp,Exp_Tp,MainJob,SubJob,Lv,PlayTime_Total,PlayTime_MainJob,Money_Total,Money_Account_Total,X,Z) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
        , Role->DBID()
        , Role->PlayerBaseData->Body.Money - Role->PlayerTempData->Log.LastMoney
        , Role->PlayerBaseData->Body.Money_Account -  Role->PlayerTempData->Log.LastMoney_Account
        , Role->PlayerTempData->Log.ActionMode._Mode
        , Role->PlayerTempData->Log.DeadCount
        , Role->PlayerTempData->Log.KillCount
        , Role->PlayerTempData->Log.Exp
        , Role->PlayerTempData->Log.Exp_TP
        , Role->BaseData.Voc
        , Role->BaseData.Voc_Sub
        , Role->TempData.Attr.Level 
        , Role->BaseData.PlayTime
        , Role->PlayerBaseData->AbilityList[ Role->BaseData.Voc].PlayTime
        , Role->PlayerBaseData->Body.Money
        , Role->PlayerBaseData->Body.Money_Account
        , int( Role->X() )
        , int( Role->Z() ) );
    Net_DCBase::LogSqlCommand( Buf );
#endif

    Role->PlayerTempData->Log.ActionMode._Mode = 0;
    Role->PlayerTempData->Log.DeadCount = 0;
    Role->PlayerTempData->Log.KillCount = 0;
    Role->PlayerTempData->Log.Exp = 0;
    Role->PlayerTempData->Log.LastMoney = Role->PlayerBaseData->Body.Money;
    Role->PlayerTempData->Log.SavePlayTime = Role->BaseData.PlayTime;
    Role->PlayerTempData->Log.Exp_TP = 0;
    Role->PlayerTempData->Log.LastMoney_Account = Role->PlayerBaseData->Body.Money_Account;
}

void	Global::Log_Level( RoleDataEx* Role )
{
	if( Role->IsPlayer() == false )
		return;

	LUA_VMClass::PCallByStrArg( "event_module(0)" , Role->GUID() ,Role->GUID() );

	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsLevelLog == false )
		return;

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0040); // 64 bytes
    pkBuf->SetCommandType(LC_CMD_LevelLog);
    pkBuf->PushInt32(Role->DBID());
    pkBuf->PushInt32(Role->G_ZoneID);
    pkBuf->PushInt32(static_cast<int>(Role->Pos()->X));
    pkBuf->PushInt32(static_cast<int>(Role->Pos()->Z));
    pkBuf->PushInt32(Role->BaseData.Voc);
    pkBuf->PushInt32(Role->TempData.Attr.Level);
    pkBuf->PushInt32(Role->BaseData.PlayTime);
    pkBuf->PushInt32(Role->TempData.Attr.Ability->PlayTime);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[2048];
    sprintf_s( Buf, sizeof(Buf), "Insert LevelLog(PlayerDBID,ZoneID,PosX,PosZ,Job,Lv,PlayTime,JobPlayTime) VALUES(%d,%d,%d,%d,%d,%d,%d,%d)"
        , Role->DBID()
        , Role->G_ZoneID
        , int(Role->Pos()->X)
        , int(Role->Pos()->Z)
        , Role->BaseData.Voc
        , Role->TempData.Attr.Level
        , Role->BaseData.PlayTime
        , Role->TempData.Attr.Ability->PlayTime );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_Dead( RoleDataEx* Deader , RoleDataEx* Killer )
{

	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Killer == NULL )
		Deader = Killer;

#if USE_NEW_LOG_CENTER_SERVER
#else
    char Buf[2048];
#endif

	if( Deader->IsPlayer() == true )
	{
		if( Global::Ini()->IsPlayerDeadLog == false )
			return;

#if USE_NEW_LOG_CENTER_SERVER
        PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0080); // 128 bytes
        pkBuf->SetCommandType(LC_CMD_PlayerDead);
        pkBuf->PushInt32(Deader->DBID());
        if (Killer->IsPlayer())
        {
            pkBuf->PushInt32(Killer->DBID());
            pkBuf->PushInt32(0);
        }
        else
        {
            pkBuf->PushInt32(0);
            pkBuf->PushInt32(Killer->BaseData.ItemInfo.OrgObjID);
        }
        pkBuf->PushInt32(Deader->G_ZoneID);
        pkBuf->PushInt32(static_cast<int>(Deader->Pos()->X));
        pkBuf->PushInt32(static_cast<int>(Deader->Pos()->Z));
        pkBuf->PushInt32(Deader->BaseData.Voc);
        pkBuf->PushInt32(Deader->BaseData.Voc_Sub);
        pkBuf->PushInt32(Deader->TempData.Attr.Level_Sub);
        pkBuf->PushInt32(Deader->TempData.Attr.Level);
        pkBuf->PushInt32(Killer->TempData.Attr.Level);
        pkBuf->PushInt32(Killer->TempData.Attr.Level_Sub);
        pkBuf->PushInt32(Killer->BaseData.Voc);
        pkBuf->PushInt32(Killer->BaseData.Voc_Sub);
        SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
        delete pkBuf;
#else
        if( Killer->IsPlayer() )
		{
            sprintf_s( Buf, sizeof(Buf), "Insert PlayerDead(PlayerDBID,KillerPlayerDBID,KillerNPCOrgObjID,ZoneID,PosX,PosZ,Job,Lv,KillerLv,Job_Sub,KillerJob,KillerJob_Sub,Lv_Sub,KillerLv_Sub) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
                , Deader->DBID()
                , Killer->DBID()
                , 0
                , Deader->G_ZoneID
                , int(Deader->Pos()->X )
                , int(Deader->Pos()->Z )
                , Deader->BaseData.Voc
                , Deader->TempData.Attr.Level
                , Killer->TempData.Attr.Level	
                , Deader->BaseData.Voc_Sub
                , Killer->BaseData.Voc
                , Killer->BaseData.Voc_Sub
                , Deader->TempData.Attr.Level_Sub
                , Killer->TempData.Attr.Level_Sub );
		}
		else
		{
            sprintf_s( Buf, sizeof(Buf), "Insert PlayerDead(PlayerDBID,KillerPlayerDBID,KillerNPCOrgObjID,ZoneID,PosX,PosZ,Job,Lv,KillerLv,Job_Sub,KillerJob,KillerJob_Sub,Lv_Sub,KillerLv_Sub) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
				, Deader->DBID()
				, 0
				, Killer->BaseData.ItemInfo.OrgObjID
				, Deader->G_ZoneID
				, int(Deader->Pos()->X )
				, int(Deader->Pos()->Z )
				, Deader->BaseData.Voc
				, Deader->TempData.Attr.Level
				, Killer->TempData.Attr.Level	
				, Deader->BaseData.Voc_Sub
				, Killer->BaseData.Voc
				, Killer->BaseData.Voc_Sub
				, Deader->TempData.Attr.Level_Sub
				, Killer->TempData.Attr.Level_Sub );
		}
#endif
	}
	else
	{
		if( Global::Ini()->IsMonsterDeadLog == false )
			return;
		if( Killer->IsPlayer() == false )
			return;

#if USE_NEW_LOG_CENTER_SERVER
        PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0040); // 64 bytes
        pkBuf->SetCommandType(LC_CMD_MonsterDead);
        pkBuf->PushInt32(Deader->BaseData.ItemInfo.OrgObjID);
        pkBuf->PushInt32(Killer->DBID());
        pkBuf->PushInt32(Deader->G_ZoneID);
        pkBuf->PushInt32(static_cast<int>(Deader->Pos()->X));
        pkBuf->PushInt32(static_cast<int>(Deader->Pos()->Z));
        pkBuf->PushInt32(Deader->TempData.Attr.Level);
        pkBuf->PushInt32(Killer->BaseData.Voc);
        pkBuf->PushInt32(Killer->TempData.Attr.Level);
        SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
        delete pkBuf;
#else
		sprintf_s( Buf, sizeof(Buf), "Insert MonsterDead(OrgObjID,PlayerDBID,ZoneID,PosX,PosZ,KillerJob,Lv,KillerLv) Values(%d,%d,%d,%d,%d,%d,%d,%d)"
			, Deader->BaseData.ItemInfo.OrgObjID
			, Killer->DBID()
			, Deader->G_ZoneID
			, int(Deader->Pos()->X )
			, int(Deader->Pos()->Z )
			, Killer->BaseData.Voc
			, Deader->TempData.Attr.Level
			, Killer->TempData.Attr.Level );
#endif
	}

#if USE_NEW_LOG_CENTER_SERVER
#else
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void		Global::Log_EnterZone		( RoleDataEx* Role )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Role->IsPlayer() == false )
		return;

#if USE_NEW_LOG_CENTER_SERVER
#else
    char Buf[2048];
#endif

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0040); // 64 bytes
    pkBuf->SetCommandType(LC_CMD_GameLoginLog);
    pkBuf->PushInt32(Role->DBID());
    pkBuf->PushInt32(EM_ActionType_EnterWorld);
    pkBuf->PushInt32(RoleDataEx::G_ZoneID);
    pkBuf->PushInt32(static_cast<int>(Role->Pos()->X));
    pkBuf->PushInt32(static_cast<int>(Role->Pos()->Z));
    pkBuf->PushInt32(Role->BaseData.Voc);
    pkBuf->PushInt32(Role->TempData.Attr.Level);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    sprintf_s( Buf, sizeof(Buf), "Insert GameLoginLog(ActionType,PlayerDBID,ZoneID,PosX,PosZ,Job,Lv) Values(%d,%d,%d,%d,%d,%d,%d)"
        , EM_ActionType_EnterWorld
        , Role->DBID()
        , RoleDataEx::G_ZoneID
        , int(Role->Pos()->X)
        , int(Role->Pos()->Z)
        , Role->BaseData.Voc
        , Role->TempData.Attr.Level );
    Net_DCBase::LogSqlCommand( Role->DBID(), Buf );
#endif

#if USE_NEW_LOG_CENTER_SERVER
    char Buf[2048];
#else
#endif

	sprintf_s( Buf , sizeof(Buf), "Update RoleData Set IsEnterWorld=1,X=%d,Y=%d,Z=%d,ZoneID=%d,RoomID=%d WHERE DBID=%d"
		, int(Role->Pos()->X)
		, int(Role->Pos()->Y)
		, int(Role->Pos()->Z)
		, Role->ZoneID()
		, Role->RoomID()
		, Role->DBID() );

	Net_DCBase::SqlCommand( rand(), Buf );
}

void		Global::Log_LeaveZone		( RoleDataEx* Role )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Role->IsPlayer() == false )
		return;

#if USE_NEW_LOG_CENTER_SERVER

#endif

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0040); // 64 bytes
    pkBuf->SetCommandType(LC_CMD_GameLoginLog);
    pkBuf->PushInt32(Role->DBID());
    pkBuf->PushInt32(EM_ActionType_LeaveWorld);
    pkBuf->PushInt32(Role->G_ZoneID);
    pkBuf->PushInt32(static_cast<int>(Role->Pos()->X));
    pkBuf->PushInt32(static_cast<int>(Role->Pos()->Z));
    pkBuf->PushInt32(Role->BaseData.Voc);
    pkBuf->PushInt32(Role->TempData.Attr.Level);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[2048];
	sprintf_s( Buf, sizeof(Buf), "Insert GameLoginLog(ActionType,PlayerDBID,ZoneID,PosX,PosZ,Job,Lv) Values(%d,%d,%d,%d,%d,%d,%d)"
		, EM_ActionType_LeaveWorld
		, Role->DBID()
		, Role->G_ZoneID
		, int(Role->Pos()->X)
		, int(Role->Pos()->Z)
		, Role->BaseData.Voc
		, Role->TempData.Attr.Level	);
	Net_DCBase::LogSqlCommand( Buf );
#endif

#if USE_NEW_LOG_CENTER_SERVER
    char Buf[2048];
#else
#endif

	sprintf_s( Buf, sizeof(Buf), "Update RoleData Set IsEnterWorld=0 WHERE DBID=%d", Role->DBID() );

	Net_DCBase::SqlCommand( rand(), Buf );
}

void		Global::Log_AddExp			( RoleDataEx* Role , int OldExp )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsExpLog == false )
		return;

	if( Role->IsPlayer() == false )
		return;

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0080); // 128 bytes
    pkBuf->SetCommandType(LC_CMD_ExpLog);
    pkBuf->PushInt32(Role->DBID());
    pkBuf->PushInt32(EM_ActionType_Exp);
    pkBuf->PushInt32(Role->G_ZoneID);
    pkBuf->PushInt32(static_cast<int>(Role->Pos()->X));
    pkBuf->PushInt32(static_cast<int>(Role->Pos()->Z));
    pkBuf->PushInt32(Role->BaseData.Voc);
    pkBuf->PushInt32(Role->TempData.Attr.Level);
    pkBuf->PushInt32(Role->TempData.Attr.Ability->EXP);
    pkBuf->PushInt32(OldExp);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[ 2048];
    sprintf_s( Buf , sizeof(Buf), "Insert ExpLog(ActionType,PlayerDBID , ZoneID, PosX , PosZ , Job , Lv , NewExp , OldExp ) Values(%d,%d,%d,%d,%d,%d,%d,%d,%d)"
        , EM_ActionType_Exp
        , Role->DBID()
        , Role->G_ZoneID
        , int(Role->Pos()->X)
        , int(Role->Pos()->Z)
        , Role->BaseData.Voc
        , Role->TempData.Attr.Level	
        , Role->TempData.Attr.Ability->EXP 
        , OldExp );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void		Global::Log_ServerStatus	( )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

	int RoomCount = 0;
	vector< PrivateRoomInfoStruct >&	RoomList = Global::St()->PrivateRoomInfoList;

	for( unsigned int i = Ini()->BaseRoomCount ; i < RoomList.size() ; i++ )
	{
		if( RoomList[i].IsActive == true )
			RoomCount++;
	}

	CCpuUsage cpuUsage;

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0040); // 64 bytes
    pkBuf->SetCommandType(LC_CMD_LocalServerStatus);
    pkBuf->PushInt32(BaseItemObject::PlayerCount());
    pkBuf->PushInt32(BaseItemObject::TotalCount());
    pkBuf->PushInt32(RoleDataEx::G_ZoneID);
    pkBuf->PushInt32(RoomCount);
    pkBuf->PushInt32(static_cast<float>( GetMemoryUsage( GetCurrentProcessId() ) ) / 1048576.0f); // 1024 x 1204
    pkBuf->PushDouble(cpuUsage.GetCpuUsage( GetCurrentProcessId() ));
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[2048];
    sprintf_s( Buf, sizeof(Buf), "Insert LocalServerStatus(PlayerCount,ObjectCount,ZoneID,RoomCount,CpuUsage,MemUsage) Values (%d,%d,%d,%d,%f,%d)"
        , BaseItemObject::PlayerCount()
        , BaseItemObject::TotalCount()
        , RoleDataEx::G_ZoneID
        , RoomCount
        , float( GetMemoryUsage( GetCurrentProcessId() ) ) / (1024*1024)
        , cpuUsage.GetCpuUsage( GetCurrentProcessId() ) );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_DepartmentStore	( ItemFieldStruct& Item , ActionTypeENUM Type , int Count , int Cost , int Cost_Bonus , int Cost_Free , int PlayerDBID )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

//	if( Item.Mode.DepartmentStore == false )
//		return;
	/*
	char Buf[ 2048];	
	sprintf_s( Buf , sizeof(Buf), "Insert DepartmentStoreLog(Type, ItemID, ItemVesion, ItemCreateTime, ItemCount, Cost, Cost_Bonus , Cost_Free , PlayerDBID) Values (%d,%d,%d,%d,%d,%d,%d,%d,%d)"
		, Type
		, Item.OrgObjID
		, Item.Serial
		, Item.CreateTime
		, Count
		, Cost
		, Cost_Bonus
		, Cost_Free
		, PlayerDBID		);
	*/

	int iClientLanguage = EM_LanguageType_TW;
	BaseItemObject* Obj = GetPlayerObj_ByDBID(PlayerDBID);

	if (Obj)
	{
		iClientLanguage = Obj->CliMiscInfo()->iLanguage;
	}

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0080); // 128 bytes
    pkBuf->SetCommandType(LC_CMD_DepartmentStoreLog);
    pkBuf->PushInt32(Type);
    pkBuf->PushInt32(Item.OrgObjID);
    pkBuf->PushInt32(Item.Serial);
    pkBuf->PushInt32(Item.CreateTime);
    pkBuf->PushInt32(Count);
    pkBuf->PushInt32(Cost);
    pkBuf->PushInt32(Cost_Bonus);
    pkBuf->PushInt32(Cost_Free);
    pkBuf->PushInt32(PlayerDBID);
    pkBuf->PushInt32(iClientLanguage);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[2048];
    sprintf_s( Buf, sizeof(Buf), "Insert DepartmentStoreLog(Type,ItemID,ItemVesion,ItemCreateTime,ItemCount,Cost,Cost_Bonus,Cost_Free,PlayerDBID,Client_Language) Values (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
        , Type
        , Item.OrgObjID
        , Item.Serial
        , Item.CreateTime
        , Count
        , Cost
        , Cost_Bonus
        , Cost_Free
        , PlayerDBID
        , iClientLanguage );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_Design			( int Type , char* Content , int PlayerDBID , int GuildID )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

	CharTransferClass	charTransfer;
	charTransfer.SetString( Content );

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0220); // 512 + 32 bytes
    pkBuf->SetCommandType(LC_CMD_DesignLog);
    pkBuf->PushInt32(Type);
    pkBuf->PushNVarChar(const_cast<wchar_t*>(charTransfer.GetWCString()));
    pkBuf->PushInt32(PlayerDBID);
    pkBuf->PushInt32(GuildID);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    string CmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
    char Buf[2048];
    sprintf_s( Buf, sizeof(Buf), "Insert DesignLog(Type,PlayerDBID,GuildID,Content) Values (%d,%d,%d,CAST( %s AS nvarchar(4000)) )"
        , Type
        , PlayerDBID
        , GuildID
        , CmdBinStr.c_str()	);
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

//©w®ÉÀË¬d¿W¥ß°Ï°ì¬O§_­n¸Ñºc
void	Global::Log_SkillLv( RoleDataEx* Owner , int SkillType , int SkillValue )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0020); // 32 bytes
    pkBuf->SetCommandType(LC_CMD_SkillLvLog);
    pkBuf->PushInt32(Owner->DBID());
    pkBuf->PushInt32(SkillType);
    pkBuf->PushInt32(SkillValue);
    pkBuf->PushInt32(Owner->BaseData.PlayTime);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[2048];
    sprintf_s( Buf , sizeof(Buf), "Insert SkillLvLog(PlayerDBID,SkillType,SkillValue,PlayTime) Values (%d,%d,%d,%d)"
        , Owner->DBID()
        , SkillType 
        , SkillValue
        , Owner->BaseData.PlayTime	);
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_ClientComputerInfo( RoleDataEx* Owner , ClientComputerInfoStruct& Info )
{
	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsClientLog == false )
		return;

	unsigned IPNum	= _Net->GetIpNum( Owner->SockID() );		 

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0080); // 128 bytes
    pkBuf->SetCommandType(LC_CMD_ClientStatus);
    pkBuf->PushInt32(Owner->DBID());
    pkBuf->PushInt32(Info.IPNum);
    pkBuf->PushInt32(Info.CPU);
    pkBuf->PushInt32(Info.RAM_Total);
    pkBuf->PushInt32(Info.RAM_Free);
		pkBuf->PushInt32(Info.RAM_Game);
    //pkBuf->PushInt32(Info.DisplayCardVendorID);
    //pkBuf->PushInt32(Info.DisplayCardDeviceID);
    pkBuf->PushInt32(Info.Ping);
    pkBuf->PushInt32(Info.FPS);
    //pkBuf->PushInt32(Info.OSID);
		//pkBuf->PushChar(Info.OSID,256);
		pkBuf->PushInt32(Info.ZoneID);
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    char Buf[4096];
    sprintf_s( Buf, sizeof(Buf), "INSERT INTO ClientStatus(PlayerDBID,IPNum,CPU,RAM_Total,RAM_Free,RAM_Game,Ping,FPS,ZoneID) VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d)"
        , Owner->DBID()
        , Info.IPNum
        , Info.CPU
        , Info.RAM_Total
        , Info.RAM_Free
				, Info.RAM_Game
        , Info.Ping
        , Info.FPS
        , Info.ZoneID );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_Talk			( TalkLogTypeENUM Type , int ChannelID , int FromPlayerDBID , int ToPlayerDBID , const char* Content )
{

	if( Global::Ini()->IsSaveLog == false )
		return;

	if( Global::Ini()->IsTalkLog == false )
		return;

	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String( Content );

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0420); // 1024 + 32 bytes
    pkBuf->SetCommandType(LC_CMD_TalkLog);
    pkBuf->PushInt32(Type);
    pkBuf->PushInt32(ChannelID);
    pkBuf->PushInt32(FromPlayerDBID);
    pkBuf->PushInt32(ToPlayerDBID);
    pkBuf->PushNVarChar(const_cast<wchar_t*>(charTransfer.GetWCString()));
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    string CmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
    char Buf[4096];
    sprintf_s( Buf, sizeof(Buf), "INSERT INTO TalkLog(Type,ChannelID,FromPlayerDBID,ToPlayerDBID,Content) VALUES (%d,%d,%d,%d,CAST( %s AS nvarchar(4000) ))"
        , Type
        , ChannelID
        , FromPlayerDBID
        , ToPlayerDBID
        , CmdBinStr.c_str()	);
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_Warning_PossibleCheater( RoleDataEx* reportRole , AutoReportBaseInfoStruct& TargetInfo , AutoReportPossibleCheaterENUM Type  )
{
	RoleDataEx* target = GetRoleDataByGUID( TargetInfo.GUID );
	if( target == NULL || target->IsPlayer() == false )
		return;

	char Buf[4096];
	sprintf_s( Buf, sizeof(Buf), "INSERT INTO Warning_PossibleCheater( ZoneID,ReporterDBID, CheaterDBID, CheaterX, CheaterY, CheaterZ, CliCheaterX, CliCheaterY, CliCheaterZ, Type) VALUES (%d,%d,%d,%f,%f,%f,%f,%f,%f,%d)"
		, RoleDataEx::G_ZoneID
		, reportRole->DBID() , target->DBID() 
		, target->X() , target->Y() , target->Z()
		, TargetInfo.X , TargetInfo.Y , TargetInfo.Z
		, Type );
	Net_DCBase::LogSqlCommand( Buf );
}

void		Global::Log_Warning_MoneyVendor( RoleDataEx* VendorRole , int PlayerDBID , const char* Content )
{
	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String( Content );

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0120); // 256 + 32 bytes
    pkBuf->SetCommandType(LC_CMD_Warning_MoneyVendor);
    pkBuf->PushInt32(VendorRole->DBID());
    pkBuf->PushInt32(PlayerDBID);
    pkBuf->PushInt32(RoleDataEx::G_ZoneID);
    pkBuf->PushNVarChar(const_cast<wchar_t*>(charTransfer.GetWCString()));
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    string CmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
    char Buf[4096];
    sprintf_s( Buf, sizeof(Buf), "INSERT INTO Warning_MoneyVendor(MoneyVenderPlayerDBID,PlayerDBID,Zone,Content) VALUES (%d,%d,%d,CAST( %s AS nvarchar(4000) ))"
        , VendorRole->DBID()
        , PlayerDBID
        , RoleDataEx::G_ZoneID
        , CmdBinStr.c_str()	);
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void		Global::Log_Warning_Bot( RoleDataEx* VendorRole , int PlayerDBID , const char* Content )
{
	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String( Content );

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0140); // 256 + 64 bytes
    pkBuf->SetCommandType(LC_CMD_Warning_Bot);
    pkBuf->PushInt32(VendorRole->DBID());
    pkBuf->PushInt32(PlayerDBID);
    pkBuf->PushInt32(RoleDataEx::G_ZoneID);
    pkBuf->PushInt32(static_cast<int>(VendorRole->X()));
    pkBuf->PushInt32(static_cast<int>(VendorRole->Z()));
    pkBuf->PushNVarChar(const_cast<wchar_t*>(charTransfer.GetWCString()));
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    string CmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
    char Buf[4096];
    sprintf_s( Buf, sizeof(Buf), "INSERT INTO Warning_Bot(BotVenderPlayerDBID,PlayerDBID,Zone,Content,X,Z) VALUES (%d,%d,%d,CAST( %s AS nvarchar(4000) ),%d,%d)"
        , VendorRole->DBID()
        , PlayerDBID
        , RoleDataEx::G_ZoneID
        , CmdBinStr.c_str()
        , int( VendorRole->X() )
        , int( VendorRole->Z() ) );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}

void	Global::Log_ExchangeItem( const char* Serial, RoleDataEx* Role, int ItemID, int Count, int Money, int AccountMoney )
{
	string ItemNameX = "";
	string AccountX =  StringToSqlX(Role->Account_ID(), _MAX_ACCOUNT_SIZE_);

	GameObjDbStructEx* pObjDB = g_ObjectData->GetObj( ItemID );

	if (pObjDB != NULL)
	{
		ItemNameX = WStringToSqlX(Utf8ToWchar(pObjDB->Name).c_str(), _MAX_NAMERECORD_SIZE_);
	}

	char Buf[4096];
	sprintf_s( Buf, sizeof(Buf), "INSERT INTO [ExchangeItemLog]([ItemSerial],[AccountID],[PlayerDBID],[ItemID],[ItemName],[Count],[Money],[AccountMoney],[ProcessTime]) VALUES ('%s', CAST( %s AS varchar(63) ), %d, %d, CAST( %s AS nvarchar(32) ), %d, %d, %d, %f)"
		, Serial
		, AccountX.c_str()
		, Role->DBID()
		, ItemID
		, ItemNameX.c_str()
		, Count
		, Money
		, AccountMoney
		, RoleDataEx::G_Now_Float);
	Net_DCBase::LogSqlCommand( Buf );
}

void	Global::Log_ZoneMoneyLog( )
{
	static int LProcTime = RoleDataEx::G_Now / 3600; // 60 x 60
	int NowProcTime = RoleDataEx::G_Now / 3600; // 60 x 60

	if( LProcTime == NowProcTime )
		return;

	LProcTime = NowProcTime;

	char Buf[4096];
	sprintf_s( Buf, sizeof(Buf), "INSERT ZoneMoneyLog(ZoneID,MoneyIn,MoneyOut,AccountMoneyIn,AccountMoneyOut) VALUES(%d,%d,%d,%d,%d)"
		, RoleDataEx::G_ZoneID
        , RoleDataEx::MoneyIn
        , RoleDataEx::MoneyOut
        , RoleDataEx::AccountMoneyIn
        , RoleDataEx::AccountMoneyOut );
	Net_DCBase::SqlCommand( rand(), Buf );

	RoleDataEx::MoneyIn = 0;
	RoleDataEx::MoneyOut = 0;
	RoleDataEx::AccountMoneyIn = 0;
	RoleDataEx::AccountMoneyOut = 0;
}

void	Global::Log_OfflineMsg		( const char* ToPlayerName , const char* FromName , int FromDBID , const char* Msg )
{
	CharTransferClass	charTransfer;
	charTransfer.SetUtf8String( ToPlayerName );
	string ToNameCmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );

	charTransfer.SetUtf8String( FromName );
	string FromNameCmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );

	charTransfer.SetUtf8String( Msg );
	string MsgCmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );

	char Buf[ 8192 ];
	sprintf_s( Buf , sizeof(Buf) , "BEGIN declare @PlayerName nvarchar( 32 );SET @PlayerName=CAST( %s AS nvarchar(4000));declare @FromPlayerName nvarchar( 32 );SET @FromPlayerName=CAST( %s AS nvarchar(4000));declare @Message nvarchar( 4000 );SET @Message=CAST( %s AS nvarchar(4000));EXECUTE CreateOfflineMessage @PlayerName,%d,@FromPlayerName,@Message; END" 
		, ToNameCmdBinStr.c_str()
		, FromNameCmdBinStr.c_str()
		, MsgCmdBinStr.c_str()	
		, FromDBID	);

	Net_DCBase::SqlCommand( rand() , Buf );
}

//////////////////////////////////////////////////////////////////////////

void		Global::Log_Verion( )
{
    char	Buf[512];
    char	ExeFileName[MAX_PATH];
    //¼g¤JVersion¸ê®Æ
    GetModuleFileName( NULL, ExeFileName, MAX_PATH );

    CFileVersion Version;
    if (Version.Open(ExeFileName))
    {
        CString ver = Version.GetFixedFileVersion();
        const char* verStr = ver.GetString();

        sprintf_s( Buf , sizeof(Buf) , "INSERT INTO ServerStartLog( Version,Server) VALUES ( '%s' , 'Zone%d' )" 
            , verStr
            , Ini()->ZoneID	);

        Net_DCBase::LogSqlCommand( Buf );
    }

}

//////////////////////////////////////////////////////////////////////////
void	Global::Log_PlayerMoveErr( RoleDataEx* role , PlayerMoveErrTypeENUM type )
{
	char sqlCmd[ 2048];
	sprintf_s( sqlCmd , sizeof(sqlCmd) , "INSERT INTO Warning_PlayerMoveErr(PlayerDBID,ZoneID,X,Y,Z,Type) VALUES(%d,%d,%f,%f,%f,%d)" 
		, role->DBID()
		, RoleDataEx::G_ZoneID
		, role->X()
		, role->Y()
		, role->Z()
		, type	);

	 Net_DCBase::SqlCommand( rand() , sqlCmd );
}
void	Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , const char* Msg )
{
    CharTransferClass	charTransfer;
    charTransfer.SetUtf8String( Msg );

#if USE_NEW_LOG_CENTER_SERVER
    PG_LCBase_DynamicBuffer<byte>* pkBuf = new PG_LCBase_DynamicBuffer<byte>(0x0220); // 512 + 32 bytes
    pkBuf->SetCommandType(LC_CMD_HouseLog);
    pkBuf->PushInt32(ActionType);
    pkBuf->PushInt32(Player->DBID());
    pkBuf->PushInt32(HouseOwnerDBID);
    pkBuf->PushInt32(HouseDBID);
    pkBuf->PushNVarChar(const_cast<wchar_t*>(charTransfer.GetWCString()));
    SendToLogCenter(pkBuf->Size(), pkBuf->Buffer());
    delete pkBuf;
#else
    string MsgCmdBinStr = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
    char Buf[8192];
    sprintf_s( Buf, sizeof(Buf), "Insert HouseLog(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,ContentTxt) Values(%d,%d,%d,%d,CAST( %s AS nvarchar(512) ))"
        , Player->DBID()
        , HouseOwnerDBID
        , HouseDBID
        , ActionType
        , MsgCmdBinStr.c_str() );
    Net_DCBase::LogSqlCommand( Buf );
#endif
}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 , int Number3 , int Number4 )
//{
//	char Buf[ 1024 ];
//	if( ActionType == EM_HouseActionType_ChangeVoc )
//	{//­×§ïÂ¾·
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType4Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,BeforeVoc,BeforeSubVoc,AfterVoc,AfterSubVoc) Values( %d,%d,%d,%d,%d,%d,%d,%d)"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			, Number1
//			, Number2
//			, Number3
//			, Number4
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 )
//{
//	char Buf[ 1024 ];
//	if( ActionType == EM_HouseActionType_EnergyIntro	||
//		ActionType == EM_HouseActionType_SpaceIntro		||
//		ActionType == EM_HouseActionType_Servant_Employ
//		)
//	{//¯à¶q¸É¥R //ªÅ¶¡ÂX¥R //¤k¹²_¶±¥Î
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType5Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,Number1,Number2) Values( %d,%d,%d,%d,%d,%d)"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			, Number1
//			, Number2
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 , float Float1 , float Float2 , float Float3 , float Float4 )
//{
//	char Buf[ 1024 ];
//	if( ActionType == EM_HouseActionType_Furniture_Appear || ActionType == EM_HouseActionType_Furniture_Disappear )
//	{//³Ã­ÑÂ\³]Åã¥Ü ³Ã­ÑÂ\³]ÁôÂÃ
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType6Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,ItemDBID,OrgObjID,X,Y,Z,Dir) Values( %d,%d,%d,%d,%d,%d,%f,%f,%f,%f)"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			, Number1
//			, Number2
//			, Float1
//			, Float2
//			, Float3
//			, Float4
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number )
//{
//	char Buf[ 1024 ];
//	if( ActionType == EM_HouseActionType_ChangeHouseType	||
//		ActionType == EM_HouseActionType_AddFriend		||
//		ActionType == EM_HouseActionType_DeleteFriend		||
//		ActionType == EM_HouseActionType_Servant_Fire ||
//		( ActionType >= EM_HouseActionType_Servant_Talk && ActionType <= EM_HouseActionType_Servant_CraftCourse )
//		)
//	{//®æ§½ÅÜ§ó //¼W¥[©Ð«Î¦n¤Í //§R°£©Ð«Î¦n¤Í//¤k¹²_¶}°£//¤k¹²_¥æ½Í//¤k¹²_Å]ªk½Òµ{//¤k¹²_§ðÀ»½Òµ{//¤k¹²_¦uÅ@½Òµ{//¤k¹²_²i¶¹½Òµ{//¤k¹²_¤uÃÀ½Òµ{
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType7Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,Number) Values( %d,%d,%d,%d,%d)"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			, Number
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , const char* Msg1 , const char* Msg2 , const char* Msg3 )
//{
//	CharTransferClass	charTransfer;
//	char Buf[ 1024 ];
//	if( ActionType == EM_HouseActionType_HouseGreet)
//	{//¦n¤Í°Ý­Ô»y
//		charTransfer.SetUtf8String( Msg1 );
//		string strmsg1 = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
//		charTransfer.SetUtf8String( Msg2 );
//		string strmsg2 = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
//		charTransfer.SetUtf8String( Msg3 );
//		string strmsg3 = StringToSqlX( (char*)charTransfer.GetWCString() , charTransfer.WCStrLen() * 2 , false );
//
//
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType8Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,Enter,Leave,UseItem) Values( %d,%d,%d,%d,CAST( %s AS nvarchar(256)),CAST( %s AS nvarchar(256)),CAST( %s AS nvarchar(256)))"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			,strmsg1.c_str()
//			,strmsg2.c_str()
//			,strmsg3.c_str()
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
//void		Global::Log_House( RoleDataEx* Player , int HouseDBID , int HouseOwnerDBID , int ActionType , int Number1 , int Number2 , int Number3 , int Number4 , int Number5 , int Number6 )
//{
//	char Buf[ 1024 ];
//	if( ActionType >= EM_HouseActionType_Item_BagToFurniture && ActionType <= EM_HouseActionType_Item_BankToEq )
//	{//²¾°Êª««~
//		sprintf_s( Buf , sizeof(Buf) , "Insert HouseType9Log(PlayerDBID,HouseOwnerDBID,HouseDBID,ActionType,ParentItem1DBID,ParentItem1Pos,ParentItem2DBID,ParentItem2Pos,OrgObjID,OrgObjCount) Values( %d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"
//			, Player->DBID()
//			, HouseOwnerDBID
//			, HouseDBID
//			, ActionType
//			, Number1
//			, Number2
//			, Number3
//			, Number4
//			, Number5
//			, Number6
//			);	
//		Net_DCBase::LogSqlCommand( Buf );
//	}
//}
