#include	"GSrvinifile.h"
#include "RoleData/ObjectDataClass.h"
#include "../PathManage/PathManage.h"
#include "netwaker/GSrvNetWakerClass.h"

void	GSrvIniFileClass::SetKeyValueMap( GameObjDbStructEx* OrgDB )
{
	for( int i = 0 ; i < 30 ; i++ )
	{
		if( strlen( OrgDB->Zone.ZoneIni[i].Key ) == 0 )
			continue;
		Print( LV2 , "SetKeyValueMap" , "Key:%s Value:%s", OrgDB->Zone.ZoneIni[i].Key, OrgDB->Zone.ZoneIni[i].Value);
		KeyValueMap[ OrgDB->Zone.ZoneIni[i].Key ] = OrgDB->Zone.ZoneIni[i].Value;
	}
}

int		GSrvIniFileClass::Int(const char* KeyStr )
{
	string a=Str(KeyStr);
	if (a.length()==0)
		return 0;

	return atoi(a.c_str());
}
bool	GSrvIniFileClass::Int( const char* KeyStr , int&	OutInt )
{
	string tempStr;
	if( Str( KeyStr , tempStr ) == false )
		return false;

	OutInt = atoi( tempStr.c_str() );
	return true;
}
string	GSrvIniFileClass::Str(const char* KeyStr )
{
	string RetStr;
	Str( KeyStr , RetStr );
	return RetStr;
}
bool	GSrvIniFileClass::Str( const char* KeyStr , string&	OutString )
{
	bool Ret = IniFileClass::Str( KeyStr , OutString );
	if( Ret != false )
		return true;
	map<string,string>::iterator Iter;
	Iter = KeyValueMap.find( KeyStr );
	if( Iter == KeyValueMap.end() )
		return false;

	OutString = Iter->second;

	return true;
	//KeyValueMap.insert()
}

bool	GSrvIniFileClass::Init(const char* IniFile)
{
	map< string , int > MapNameID;

	if( SetIniFile( IniFile ) == false )
		return false;	

    Ini = (char*)IniFile;

	//所有LocalServer公用設定資訊檔案
	BaseIni = Str( "BaseIni" );

	if( SetIniFile( (char*)BaseIni.c_str() ) == false )
		return false;

	SetIniFile( "Global.ini" );

	g_pPrint->SetPrintLV( Int( "DebugPrintLv" ) );
	g_pPrint->SetFileOutputLV( Int( "DebugOutputLv" ) );
	g_pPrint->SetOutputFieldName( Str( "DebugOutputField" ).c_str() );

    DataPath	= Str( "DataPath" );
	Str( "MapFileName" , MapFileName );
	ResourcePath		= Str( "ResourcePath" );

	// 讀取 Server Data 與 String 的設定
	if( Str( "Datalanguage", Datalanguage ) == false )
	{
		Datalanguage = "rw";
	}

	Datalanguage = strlwr( (char*)Datalanguage.c_str() );

	if( Str( "Stringlanguage", Stringlanguage ) == false )
	{
		Stringlanguage = "";
	}
	int isLoadAll = 1;
	LoadAll = true;
	if (Int( "LoadAllData", isLoadAll ) == true)
	{
		if (isLoadAll == 0)
		{
				LoadAll = false;
		}
	}


	Stringlanguage = strlwr( (char*)Stringlanguage.c_str() );

	PathManageClass::InitBase( ResourcePath.c_str() , MapFileName.c_str() );

	//ObjectDataClass::InitShareMemory();
    ObjectDataClass::Init( DataPath.c_str(), Datalanguage.c_str(), Stringlanguage.c_str() ,LoadAll);
	//////////////////////////////////////////////////////////////////////////
	ZoneID = Int( "ZoneID" );			//區域ID
	//////////////////////////////////////////////////////////////////////////
	//設定區域資料
	GameObjDbStructEx* ObjDB = g_ObjectData->GetObj( ( ZoneID % _DEF_ZONE_BASE_COUNT_ ) + Def_ObjectClass_Zone );
	if( ObjDB != NULL )
	{//沒有區域資料物件
		SetKeyValueMap( ObjDB );
		
		ZoneName = ObjDB->Name;
		MapFileName = ObjDB->Zone.MapFile;
		BlockSize = ObjDB->Zone.ViewBlock;
		if( ObjDB->Zone.IsPrivateZone == false )
		{
			RoomCount		= ObjDB->Zone.BaseRoomCount;
			PlayRoomCount	= 0;
		}
		else
		{
			RoomCount		= ObjDB->Zone.BaseRoomCount + ObjDB->Zone.PlayRoomCount;
			PlayRoomCount	= ObjDB->Zone.PlayRoomCount;
		}
		BaseRoomCount = ObjDB->Zone.BaseRoomCount;
		

		CtrlMinX = ObjDB->Zone.CtrlMinX;
		CtrlMinZ = ObjDB->Zone.CtrlMinZ;
		CtrlMaxX = ObjDB->Zone.CtrlMaxX;
		CtrlMaxZ = ObjDB->Zone.CtrlMaxZ;

		IsPrivateZone = ObjDB->Zone.IsPrivateZone;
		PrivateZoneType = ObjDB->Zone.PrivateZoneType;
		PrivateZoneLiveTime = ObjDB->Zone.PrivateZoneLiveTime;		
		ZoneLiveTime = ObjDB->Zone.MirrorLimitTime;
		RoomPlayerCount = ObjDB->Zone.RoomPlayerCount;
//		PKType = ObjDB->Zone.PKType;
		CampStatuteObj = ObjDB->Zone.CampStatueObjID;

		EnabledRevPoint = ObjDB->Zone.IsEnabledRevPoint;
		RevZoneID = ObjDB->Zone.RevZoneID;
		RevX = ObjDB->Zone.RevX;
		RevY = ObjDB->Zone.RevY;
		RevZ = ObjDB->Zone.RevZ;
		MapID = ObjDB->Zone.MapID;
		for( int i = 0 ; i < 1000 ; i++  )
		{
			GameObjDbStructEx* ObjDB = g_ObjectData->GetObj( i + Def_ObjectClass_Zone );
			if( ObjDB == NULL )
				break;
	
			MapNameID[ ObjDB->Zone.MapFile ] = i + 1;
		}

		IsEnabledRide = ObjDB->Zone.IsEnabledRide;		
	}

	//////////////////////////////////////////////////////////////////////////
	int		GMCommandValue = 0;
	Int( "GMCommand" , GMCommandValue );			//區塊大小
	IsEnabledGMCommand = ( GMCommandValue == 1 );
	Str( "ZoneName" , ZoneName );		//區域名稱
	

	Int( "ViewBlock" , BlockSize );			//區塊大小
	Int( "BaseRoomCount" , BaseRoomCount );
	Int( "PlayRoomCount" , PlayRoomCount );
	if( BaseRoomCount == 0 )
		BaseRoomCount = 1;  

	int CratePathMaker = 0;
	Int( "CratePathMaker" , CratePathMaker );


	RoomCount = BaseRoomCount + PlayRoomCount;

	
	if( ObjDB != NULL && ObjDB->Zone.IsEnabledBlockCtrl != false )
	{
		Int( "CtrlMinX" , CtrlMinX ); 
		Int( "CtrlMinZ" , CtrlMinZ ); 
		Int( "CtrlMaxX" , CtrlMaxX );
		Int( "CtrlMaxZ" , CtrlMaxZ );
		PathManageClass::Init( ResourcePath.c_str() , MapFileName.c_str() , CtrlMinX , CtrlMinZ , CtrlMaxX , CtrlMaxZ , CratePathMaker != 0  );
	}
	else
	{
		PathManageClass::Init( ResourcePath.c_str() , MapFileName.c_str() , MapID , CratePathMaker != 0 );
		CtrlMinX = PathManageClass::MinCtrlX();
		CtrlMinZ = PathManageClass::MinCtrlZ();
		CtrlMaxX = PathManageClass::MaxCtrlX();
		CtrlMaxZ = PathManageClass::MaxCtrlZ();
		Int( "CtrlMinX" , CtrlMinX ); 
		Int( "CtrlMinZ" , CtrlMinZ ); 
		Int( "CtrlMaxX" , CtrlMaxX );
		Int( "CtrlMaxZ" , CtrlMaxZ );
	}

	CtrlMinX    = CtrlMinX / BlockSize * BlockSize;
	CtrlMinZ    = CtrlMinZ / BlockSize * BlockSize;
	CtrlMaxX    = ( CtrlMaxX - 1 )/ BlockSize * BlockSize;
	CtrlMaxZ    = ( CtrlMaxZ - 1 )/ BlockSize * BlockSize;

	ViewMinX    = CtrlMinX - 480 * 2;
	ViewMinZ    = CtrlMinZ - 480 * 2;
	ViewMaxX    = CtrlMaxX + 480 * 2;
	ViewMaxZ    = CtrlMaxZ + 480 * 2;

	int		TempInt;
	
	if( Int( "PrivateZone" , TempInt ) != false )
	{
		IsPrivateZone = (TempInt != 0);
	}

	if( Int( "PrivateZoneType" , TempInt ) != false )
	{
		PrivateZoneType = (TempInt);
	}
	Int( "PrivateZoneLiveTime" , PrivateZoneLiveTime );

	PartitionWidth     = ( ViewMaxX - ViewMinX ) / BlockSize + 1;	//區塊分割寬
	PartitionHeight    = ( ViewMaxZ - ViewMinZ ) / BlockSize + 1;	//區塊分割高

	if( PartitionWidth == 0 || PartitionHeight == 0 )    
		return false;

	PartitionMaxItem   = 60000;

	if( Int( "PKType" , PKType ) == false )
		PKType = 0;

	Int( "CampStatuteObj" , CampStatuteObj );

	if( Int( "EnabledRevPoint" , TempInt ) != false )
	{
		EnabledRevPoint = (TempInt != 0);
	}

	Int( "RevZoneID" , RevZoneID );
	Int( "RevX" , RevX );	 
	Int( "RevY" , RevY );	 
	Int( "RevZ" , RevZ );	 
	//MapID = MapNameID[ MapFileName ];

	if( Int( "BattleGroundType", BattleGroundType ) == false )
	{
		BattleGroundType = 0;
	}
	
	if( Int( "DisableCheckRoleDataSize" ) == 0 )
	{
		IsCheckRoleDataSize = true;
	}
	else
	{
		IsCheckRoleDataSize = false;
	}



	if( Int( "DisabledSavePosition" , TempInt ) != false )
	{
		DisabledSavePosition = (TempInt != 0);
	}
	else
		DisabledSavePosition = false;

	if( Int( "DisableGoodEvil" , TempInt ) != false )
	{
		DisableGoodEvil= (TempInt != 0);
	}
	else
		DisableGoodEvil = false;

	if( Int( "IsDisableSendGift" , TempInt ) != false )
	{
		IsDisableSendGift= (TempInt != 0);
	}
	else
		IsDisableSendGift = false;


	if( Int( "TryException" ) != 0 )
		IsEnableTryException = true;
	else
		IsEnableTryException = false;

	if( Int( "IsSendAllPlayerPos" ) != 0 )
		IsSendAllPlayerPos = true;
	else
		IsSendAllPlayerPos = false;


	if( Int( "IsSaveLog" ) != 0 )
		IsSaveLog = true;
	else
		IsSaveLog = false;

	if( Int( "IsTalkLog" ) != 0 )
		IsTalkLog = true;
	else
		IsTalkLog = false;

	if( Int( "IsLoadTxtLuaScript" ) != 0 )
		IsLoadTxtLuaScript = true;
	else
		IsLoadTxtLuaScript = false;

	if( Int( "IsLoadFdbLuaScript" ) != 0 )
		IsLoadFdbLuaScript = true;
	else
		IsLoadFdbLuaScript = false;


	if( Int( "IsExpLog" ) != 0 )
		IsExpLog = true;
	else
		IsExpLog = false;

	if( Int( "IsLevelLog" ) != 0 )
		IsLevelLog = true;
	else
		IsLevelLog = false;

	if( Int( "IsItemTradeLog" ) != 0 )
		IsItemTradeLog = true;
	else
		IsItemTradeLog = false;

	if( Int( "IsItemTradeLog_Detail" ) != 0 )
	{
		IsItemTradeLog = true;
		IsItemTradeLog_Detail = true;
	}
	else
		IsItemTradeLog_Detail = false;


	if( Int( "IsMoneyLog" ) != 0 )
		IsMoneyLog = true;
	else
		IsMoneyLog = false;

	if( Int( "IsAccountMoneyLog" ) != 0 )
		IsAccountMoneyLog = true;
	else
		IsAccountMoneyLog = false;

	if( Int( "IsPlayerActionLog" ) != 0 )
		IsPlayerActionLog = true;
	else
		IsPlayerActionLog = false;

	if( Int( "IsPlayerDeadLog" ) != 0 )
		IsPlayerDeadLog = true;
	else
		IsPlayerDeadLog = false;

	if( Int( "IsMonsterDeadLog" ) != 0 )
		IsMonsterDeadLog = true;
	else
		IsMonsterDeadLog = false;

	// Wait to fix
	if( Int( "IsQuesetLog" ) != 0 )
		IsQuestLog = true;
	else
		IsQuestLog = false;

	if( IsQuestLog == false && Int( "IsQuestLog" ) != 0 )
		IsQuestLog = true;

	if( Int( "IsClientLog" ) != 0 )
		IsClientLog = true;
	else
		IsClientLog = false;


	if( Int( "IsSendNpcInfoToGMTools" ) != 0 )
		IsSendNpcInfoToGMTools = true;
	else
		IsSendNpcInfoToGMTools = false;

	if( Int( "IsGlobalDrop" ) != 0 )
		IsGlobalDrop = true;
	else
		IsGlobalDrop = false;

	if( Int( "IsServerTotalItemLog" ) != 0 )
		IsServerTotalItemLog = true;
	else
		IsServerTotalItemLog = false;

	if( Int( "IsIgnoreCollision" ) != 0 )
		IsIgnoreCollision = true;
	else
		IsIgnoreCollision = false;


	if( Int( "IsHouseZone" ) != 0 )
		IsHouseZone = true;
	else
		IsHouseZone = false;

	if( Int( "IsGuildHouseWarZone" ) != 0 )
		IsGuildHouseWarZone = true;
	else
		IsGuildHouseWarZone = false;

	if( Int( "IsReturnSavePoint" ) != 0 )
		IsReturnSavePoint = true;
	else
		IsReturnSavePoint = false;

		
	if( Int( "IsGuildHouseZone" ) != 0 )
		IsGuildHouseZone = true;
	else
		IsGuildHouseZone = false;

	if( Int( "IsHouseMoveItemLog" ) != 0 )
		IsHouseMoveItemLog = true;
	else
		IsHouseMoveItemLog = false;

	if( Int( "IsHouseActionLog" ) != 0 )
		IsHouseActionLog = true;
	else
		IsHouseActionLog = false;

	if( Int( "IsHouseServantLog" ) != 0 )
		IsHouseServantLog = true;
	else
		IsHouseServantLog = false;

	if( Int( "DisableGMPassword" ) != 0 )
		DisableGMPassword = true;
	else
		DisableGMPassword = false;

	if (Str("GMPassword", GMPassword) == false)
		GMPassword = "mydefaultpassword";
/*
	LanguageType = EM_LanguageType_TW;
	if( stricmp( Datalanguage.c_str() , "CN" ) == 0 )
		LanguageType = EM_LanguageType_CN;
	else if( stricmp( Datalanguage.c_str() , "ENUS" ) == 0 )
		LanguageType = EM_LanguageType_ENUS;
	else if( stricmp( Datalanguage.c_str() , "ENEU" ) == 0 )
		LanguageType = EM_LanguageType_ENEU;
	else if( stricmp( Datalanguage.c_str() , "JP" ) == 0 )
		LanguageType = EM_LanguageType_JP;
	else if( stricmp( Datalanguage.c_str() , "DE" ) == 0 )
		LanguageType = EM_LanguageType_DE;
	else if( stricmp( Datalanguage.c_str() , "SG" ) == 0 )
		LanguageType = EM_LanguageType_SG;
	else if( stricmp( Datalanguage.c_str() , "BR" ) == 0 )
		LanguageType = EM_LanguageType_BR;
	else if( stricmp( Datalanguage.c_str() , "TR" ) == 0 )
		LanguageType = EM_LanguageType_TR;
	else if( stricmp( Datalanguage.c_str() , "RU" ) == 0 )
		LanguageType = EM_LanguageType_RU;
	else if( stricmp( Datalanguage.c_str() , "FR" ) == 0 )
		LanguageType = EM_LanguageType_FR;
	else if( stricmp( Datalanguage.c_str() , "PT" ) == 0 )
		LanguageType = EM_LanguageType_PT;
	else if( stricmp( Datalanguage.c_str() , "PL" ) == 0 )
		LanguageType = EM_LanguageType_PL;
*/
	CountryType = g_ObjectData->QueryCountryID( Datalanguage.c_str() );
	Switch_GServerPort  = Int( "Switch_GSrvPort" );
	SwitchIP            = Str( "SwitchIP" );


	Switch_CliPort	  = Int( "Switch_CliPort" );
	SwitchIP_Outside  = Str( "SwitchIP_Outside" );

	LuaPath             = Str( "LuaPath" );
	LuaPathEx           = Str( "LuaPathEx" );

	LuaFuncInitZone		= Str( "LuaFuncInitZone" );
	EnterZoneScript		= Str( "EnterZoneScript" );
	EnterZoneScript_Pcall	= Str( "EnterZoneScript_Pcall" );

	if( LuaFuncInitZone.size() == 0 )
	{
		char szLuaZone[256];
		sprintf_s( szLuaZone , sizeof(szLuaZone) , "LuaFunc_InitZone_%d", ZoneID );
		LuaFuncInitZone = szLuaZone;
	}

	LuaFunc_PVP		= Str( "LuaFunc_PVP" );
	ReviveScript	= Str( "ReviveScript" );

	// 戰場設定

	if( Int( "BGLimit", BGLimit ) == false )
	{
		BGLimit = 0;
	}

	if( Int( "IsPvE" ) != 0 )
		IsPvE = true;
	else
		IsPvE = false;

	RoleDataEx::G_IsPvE = IsPvE;
	RoleDataEx::G_TimeZone = Int("TimeZone");
	RoleDataEx::G_MaxVocCount = Int("MaxVocCount");
	if( RoleDataEx::G_MaxVocCount == 0 )
		RoleDataEx::G_MaxVocCount = 16;

	DeadExpDownRate  = Int( "DeadExpDownRate" );
	DeadDebtExpRate  = Int( "DeadDebtExpRate" );
	DeadDebtTpRate  = Int( "DeadDebtTpRate" );

	ZoneChannelBroadcastLv  = Int( "ZoneChannelBroadcastLv" );
	MailLv = Int( "MailLv" ); 

	if( Int( "IsSysChannelNeedItem" ) != 0 )
		IsSysChannelNeedItem = true;
	else
		IsSysChannelNeedItem = false;

	if( Int( "IsBattleWorld" ) != 0 )
		IsBattleWorld = true;
	else
		IsBattleWorld = false;

	if( Int( "IsZonePartyEnabled" ) != 0 )
		IsZonePartyEnabled = true;
	else
		IsZonePartyEnabled = false;

	if( Int( "IsDisableParty" ) != 0 )
		IsDisableParty = true;
	else
		IsDisableParty = false;

	CreateTombX  = Int( "CreateTombX" );
	CreateTombY  = Int( "CreateTombY" );
	CreateTombZ  = Int( "CreateTombZ" );
	CreateTombZoneID= Int( "CreateTombZoneID" );

	if( Int( "HouseLifeTime" , HouseLifeTime ) == false )
		HouseLifeTime = 60*10;

	HouseLifeTime = HouseLifeTime * 1000;

	if( Int( "AC_AccountMoneyTrade" ) != 0 )
		AC_AccountMoneyTrade = true;
	else
		AC_AccountMoneyTrade = false;

	if( Int( "AC_Item_AccountMoneyTrade" ) != 0 )
		AC_Item_AccountMoneyTrade = true;
	else
		AC_Item_AccountMoneyTrade = false;

	if( Int( "IsAutoRevive" ) == 0 )
		IsAutoRevive = false;
	else
		IsAutoRevive = true;

	if( Int( "IsNeedCheckDeadZone" ) == 0 )
		IsNeedCheckDeadZone = false;
	else
		IsNeedCheckDeadZone = true;

	if( Int( "SwitchPacketTimeProcLog" ) == 0 )
		GSrvNetWakerClass::G_SwitchPacketTimeProcLog = false;
	else
		GSrvNetWakerClass::G_SwitchPacketTimeProcLog = true;
	

	if( Int( "IsDisablePVPRule" ) == 0 )
	{
		RoleDataEx::IsDisablePVPRule = false;
		IsDisablePVPRule = false;
	}
	else
	{
		RoleDataEx::IsDisablePVPRule = true;
		IsDisablePVPRule = true;
	}

	if( Int( "IsDisableTrade" ) == 0 )
		IsDisableTrade	= false;	// 關掉交易 ( 預設 )
	else
		IsDisableTrade	= true;		

	if( Int( "IsDisableDual" ) == 0 )
		IsDisableDual	= false;	// 關掉挑戰 ( 預設 )
	else
		IsDisableDual	= true;		


	if( Int( "IsAllowHonorSystem" ) == 0 )
		IsAllowHonorSystem = 0;
	else
		IsAllowHonorSystem = 1;

	if( Int( "ResetHonorValue", ResetHonorValue ) == false )
	{
		ResetHonorValue = 25;
	}
	MaxRoleLevel = Int( "MaxRoleLevel" );

	RoleDataEx::gPlayerBaseData.MaxLv = MaxRoleLevel;

	WaitUnlockPosTime = Int( "WaitUnlockPosTime" );
	if( WaitUnlockPosTime < 60 )
		WaitUnlockPosTime = 60;

	ZoneValue_TPBonus = Int( "ZoneValue_TPBouns" );
	ZoneValue_EXPBonus = Int( "ZoneValue_EXPBouns" );
	ZoneValue_TreasureBouns = Int( "ZoneValue_TreasureBouns" );

	Max_GuildWorld_PlayerCount = Int( "Max_GuildWorld_PlayerCount" );

	PlayerCenterCount = Int( "PlayerCenterCount") ;
	if( PlayerCenterCount == 0 )
		PlayerCenterCount =1;
/*
	if( Int( "InstanceSaveID", InstanceSaveID ) == false )
	{
		InstanceSaveID = -1;
	}	
	if( unsigned( InstanceSaveID ) >= _MAX_INSTANCE_SETTING_COUNT_ )
	{
		InstanceSaveID = -1;
	}
	*/

	if( ZoneValue_TPBonus < 0 )
		ZoneValue_TPBonus = 0 ;
	if( ZoneValue_EXPBonus < 0 )
		ZoneValue_EXPBonus = 0;
	if( ZoneValue_TreasureBouns < 0 )
		ZoneValue_TreasureBouns = 0;

	RoleDataEx::GlobalSetting.ExpRate = 1 + (ZoneValue_EXPBonus / 100.0f);
	RoleDataEx::GlobalSetting.TpRate = 1 + (ZoneValue_TPBonus / 100.0f);
	RoleDataEx::GlobalSetting.DropTreasureRate = 1 + (ZoneValue_TreasureBouns / 100.0f);


	if( Int( "IsDisableMailMoney" ) == 0 )
		IsDisableMailMoney = 0;
	else
		IsDisableMailMoney = 1;

	if( Int( "IsDisableMailAccountMoney" ) == 0 )
		IsDisableMailAccountMoney = 0;
	else
		IsDisableMailAccountMoney = 1;

	if( Int( "IsDisablePKProtectBuff" ) == 0 )
		IsDisablePKProtectBuff = 0;
	else
		IsDisablePKProtectBuff = 1;

	if( Int( "IsDisableDropRateDesc" ) == 0 )
		IsDisableDropRateDesc = 0;
	else
		IsDisableDropRateDesc = 1;

	if( Int( "IsNoSwimming" ) == 0 )
		IsNoSwimming = 0;
	else
		IsNoSwimming = 1;

	if( Int( "IsCheckHeight" ) == 0 )
		IsCheckHeight = 0;
	else
		IsCheckHeight = 1;

	if( Int( "IsAllowLuaDebugger" ) == 0 )
		IsAllowLuaDebugger = 0;
	else
		IsAllowLuaDebugger = 1;

	AdjustGuildWarTime = Int( "AdjustGuildWarTime" );

	LuaDebuggerAgentIP	  = Str( "LuaDebuggerAgentIP " );
	LuaDebuggerAgentPort  = Int( "LuaDebuggerAgentPort" );

	BG_Independence_Game			= Int( "BG_Independence_Game" ) != 0;
	BG_Independence_Game_Money		= Int( "BG_Independence_Game_Money" );
	BG_Independence_Game_Item		= Int( "BG_Independence_Game_Item" );
	BG_Independence_Game_KeyItem	= Int( "BG_Independence_Game_KeyItem" );
	BG_Independence_Game_Title		= Int( "BG_Independence_Game_Title" );
	BG_Independence_Game_Card		= Int( "BG_Independence_Game_Card" );
	BG_BG_LeaveScript	  			= Str( "BG_BG_LeaveScript" );
	BG_OrgWorld_LeaveScript			= Str( "BG_OrgWorld_LeaveScript" );

	DiableGuildHouseWarTime_Begin	= Int( "DiableGuildHouseWarTime_Begin" );
	DiableGuildHouseWarTime_End		= Int( "DiableGuildHouseWarTime_End" );
	return true;
}
//----------------------------------------------------------------------------------------