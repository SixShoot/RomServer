#pragma once
#include "NetSrv_BG_GuildWar.h"
//////////////////////////////////////////////////////////////////////////
//公會戰Rank資訊
struct AllGuildRankInfoStruct
{
	GuildWarRankInfoStruct		Info;
	GuildHouseWarHistoryStruct	Hisotry[50];	
};
//////////////////////////////////////////////////////////////////////////
class NetSrv_BG_GuildWarChild : public NetSrv_BG_GuildWar
{
	static int  _LoadGuildHouseProc( SchedularInfo* Obj , void* InputClass );
	static bool _SendHouseInfo( int FromWorldId , int FromNetID ,int GuildID );

	static int  _BG_LoadGuildHouseProc( SchedularInfo* Obj , void* InputClass );
	
	static map<int , AllGuildRankInfoStruct >	_AllWorldRankInfoMap;
public:
	static bool Init();
	static bool Release();


	//要求跨伺服器公會名稱要求
	virtual	void RC_GuildNameRequest( BaseItemObject* OwnerObj , int WorldGuildID );
	//要求跨伺服器公會名稱要求
	virtual	void RBL_GuildNameRequest( int FromWorldId , int FromNetID ,int PlayerDBID , int WorldGuildID );
	//要求跨伺服器公會名稱結果
	virtual	void RL_GuildNameResult( int FromWorldId , int FromNetID ,int PlayerDBID , int WorldGuildID , const char* GuildName );

	//開啟註冊公會戰介面
	virtual	void RC_OpenMenuRequest( BaseItemObject* OwnerObj );
	//要求公會戰資訊
	virtual	void RL_OpenMenuRequest( int FromWorldId , int FromNetID ,int PlayerDBID );
	//公會戰開啟狀況
	virtual	void RBL_OpenMenuResult( int FromWorldId , int FromNetID ,int PlayerDBID , int ZoneGroupID , int NextTime , GuildHouseWarStateENUM State , int Count , GuildHouseWarInfoStruct List[1000] );
	//註冊公會戰( or 取消 )
	virtual	void RC_RegisterRequest( BaseItemObject* OwnerObj , int ZoneGroupID , GuildWarRegisterTypeENUM Type , int GuildCount , bool IsAcceptAssignment , int TargetGuild );
	//註冊公會戰( or 取消 )
	virtual	void RL_RegisterRequest( int FromWorldId , int FromNetID , GuildWarRegisterInfoStruct& Info );
	//註冊公會戰( or 取消 )結果
	virtual	void RBL_RegisterResult( int FromWorldId , int FromNetID ,int GuildID , GuildWarRegisterTypeENUM Type , GuildHouseWarInfoStruct& Info , GuildWarRegisterResultENUM Result , int PlayerDBID );
	//公會戰狀況( 開始 , 準備 , 結束 )
	virtual	void RBL_Status( int FromWorldId , int FromNetID , GuildWarStatusTypeENUM Status , int NextTime );
	//要求公會屋資料(取zone401的資料)
	virtual	void RBL_HouseLoadRequest( int FromWorldId , int FromNetID ,int GuildID );
	//公會屋基本資料
	virtual	void RL_HouseBaseInfo( int FromWorldId , int FromNetID ,GuildHousesInfoStruct& HouseBaseInfo );
	//公會屋建築資料
	virtual	void RL_HouseBuildingInfo( int FromWorldId , int FromNetID ,GuildHouseBuildingInfoStruct Building );
	//公會屋家具資料
	virtual	void RL_HouseFurnitureInfo( int FromWorldId , int FromNetID ,GuildHouseFurnitureItemStruct Item );
	//公會屋資料讀取完畢
	virtual	void RL_HouseLoadOK( int FromWorldId , int FromNetID ,int GuildID );
	//要求進入公會戰場
	virtual	void RC_EnterRequest( BaseItemObject* OwnerObj , int ZoneGroupID );
	//要求進入公會戰場
	virtual	void RL_EnterRequest( int FromWorldId , int FromNetID ,int PlayerDBID , int GuildID );
	//要求進入結果
	virtual	void RBL_EnterResult( int FromWorldId , int FromNetID , int ZoneGroupID ,int PlayerDBID , int GuildID , bool Result );
	//要求離開公會戰場
	virtual	void RC_LeaveRequest( BaseItemObject* OwnerObj );
	//設定目前公會戰時間
	virtual void RL_DebugTime( int FromWorldId , int FromNetID , int Time_Day , int Time_Hour );

	//Client 端要求公會戰場玩家資料
	virtual void RC_PlayerInfoRequest( BaseItemObject*  Obj );

	//公會戰結果
	virtual void RBL_EndWarResult( int GuildID , GameResultENUM GameResult , int GuildScore , GuildWarBaseInfoStruct& TargetGuild );
	//////////////////////////////////////////////////////////////////////////
	//戰爭開始
	static void WarPerpare		( int GuildID1 , int GuildID2 , bool IsAssignment);

	//通知某人戰場的分數
	static	void SC_GuildScore_ByRoomID( int SendToID , int RoomID );
	//////////////////////////////////////////////////////////////////////////
	virtual void RD_OrderInfo( int Count , GuildHouseWarInfoStruct List[] );	
	virtual void OnDataCenterConnect( );
	//////////////////////////////////////////////////////////////////////////
	virtual void RD_RegGuildWarRankInfo( GuildWarRankInfoStruct& Info , GuildHouseWarHistoryStruct Hisotry[50] );
	virtual void RC_GuildWarRankInfoRequest( BaseItemObject* Obj , GuildWarRankInfoTypeENUM Type , int WorldGuildID );
	virtual void RL_GuildWarRankInfoRequest( int FromWorldId , int FromNetID , int PlayerDBID , GuildWarRankInfoTypeENUM Type , int WorldGuildID );
	virtual void RBL_GuildWarHisotry( int PlayerDBID , GuildHouseWarHistoryStruct Hisotry[50] );
	virtual void RBL_GuildWarRankInfo( GuildWarRankInfoStruct& Info , int PlayerDBID );


};

