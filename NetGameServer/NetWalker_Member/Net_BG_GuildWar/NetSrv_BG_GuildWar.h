#pragma once
#include "../../MainProc/Global.h"
#include "PG/PG_BG_GuildWar.h"

class NetSrv_BG_GuildWar:public Global
{
protected:
	static NetSrv_BG_GuildWar *This;
	static bool _Init();
	static bool _Release();

	static void _PG_BG_GuildWar_CtoBL_GuildNameRequest		( int Sockid , int Size , void* Data );
	static void _PG_BG_GuildWar_BLtoL_GuildNameRequest		( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_LtoBL_GuildNameResult		( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_CtoL_OpenMenuRequest		( int Sockid , int Size , void* Data );
	static void _PG_BG_GuildWar_LtoBL_OpenMenuRequest		( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_BLtoL_OpenMenuResult		( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_CtoL_RegisterRequest		( int Sockid , int Size , void* Data );
	static void _PG_BG_GuildWar_LtoBL_RegisterRequest		( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_BLtoL_RegisterResult		( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_BLtoL_Status				( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_BLtoL_HouseLoadRequest		( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_LtoBL_HouseBaseInfo			( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_LtoBL_HouseBuildingInfo		( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_LtoBL_HouseFurnitureInfo	( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_LtoBL_HouseLoadOK			( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_CtoL_EnterRequest			( int Sockid , int Size , void* Data );
	static void _PG_BG_GuildWar_LtoBL_EnterRequest			( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_BLtoL_EnterResult			( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_CtoBL_LeaveRequest			( int Sockid , int Size , void* Data );
	static void _PG_BG_GuildWar_LtoBL_DebugTime				( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_BLtoL_EndWarResult			( int FromWorldId , int FromNetID , int Size , void* Data );

	static void _PG_BG_GuildWar_CtoBL_PlayerInfoRequest		( int Sockid , int Size , void* Data );
	static void _PG_BG_GuildWar_DtoBL_OrderInfo				( int Sockid , int Size , void* Data );
	static void _OnDataCenterConnectEvent					( EM_SERVER_TYPE ServerType, DWORD dwServerLocalID );


	static void _PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo		( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest	( int Sockid , int Size , void* Data );
	static void _PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest	( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_BLtoL_GuildWarHisotry			( int FromWorldId , int FromNetID , int Size , void* Data );
	static void _PG_BG_GuildWar_BLtoL_GuildWarRankInfo			( int FromWorldId , int FromNetID , int Size , void* Data );
public:

	//要求跨伺服器公會名稱要求
	virtual	void RC_GuildNameRequest( BaseItemObject* OwnerObj , int WorldGuildID ) = 0;
		//要求跨伺服器公會名稱要求
		static	void SL_GuildNameRequest( int ToWorldID , int ToGSrvID ,int PlayerDBID , int WorldGuildID );
		virtual	void RBL_GuildNameRequest( int FromWorldId , int FromNetID ,int PlayerDBID , int WorldGuildID ) = 0;
			//要求跨伺服器公會名稱結果
			static	void SBL_GuildNameResult( int ToWorldID , int ToNetID ,int PlayerDBID , int WorldGuildID , const char* GuildName );
			virtual	void RL_GuildNameResult( int FromWorldId , int FromNetID ,int PlayerDBID , int WorldGuildID , const char* GuildName ) = 0;
				//要求跨伺服器公會名稱結果
				static	void SC_GuildNameResult( int SendtoID , int WorldGuildID , const char* GuildName );

	//開啟註冊公會戰介面
	virtual	void RC_OpenMenuRequest( BaseItemObject* OwnerObj ) = 0;
		//要求公會戰資訊
		static	void SBL_OpenMenuRequest( int ToWorldID , int ToGSrvID ,int PlayerDBID );
		virtual	void RL_OpenMenuRequest( int FromWorldId , int FromNetID ,int PlayerDBID ) = 0;
			//公會戰開啟狀況
			static	void SL_OpenMenuResult( int ToWorldID , int ToNetID ,int PlayerDBID , GuildHouseWarStateENUM State , int NextTime , vector<GuildHouseWarInfoStruct>& List );
			virtual	void RBL_OpenMenuResult( int FromWorldId , int FromNetID ,int PlayerDBID , int ZoneGroupID , int NextTime , GuildHouseWarStateENUM State , int Count , GuildHouseWarInfoStruct List[1000] ) = 0;
				//開啟註冊公會戰介面
				static	void SC_OpenMenuResult( int SendtoID , int PlayerDBID , int ZoneGroupID , GuildHouseWarStateENUM State , int NextTime , int Count , GuildHouseWarInfoStruct List[1000] );

	//註冊公會戰( or 取消 )
	virtual	void RC_RegisterRequest( BaseItemObject* OwnerObj , int ZoneGroupID , GuildWarRegisterTypeENUM Type , int GuildCount , bool IsAcceptAssignment , int TargetGuild ) = 0;
		//註冊公會戰( or 取消 )
		static	void SBL_RegisterRequest( int ToWorldID , int ToGSrvID ,int GuildID , int GuildLv , const char* GuildName , int Score , GuildWarRegisterTypeENUM Type , int GuildCount , int PlayerDBID , bool IsAcceptAssignment , int TargetGuild );
		virtual	void RL_RegisterRequest( int FromWorldId , int FromNetID , GuildWarRegisterInfoStruct& Info ) = 0;
			//註冊公會戰( or 取消 )結果
			static	void SL_RegisterResult( int ToWorldID , int ToNetID ,int GuildID , GuildWarRegisterTypeENUM Type , GuildHouseWarInfoStruct& Info , GuildWarRegisterResultENUM Result , int PlayerDBID );
			virtual	void RBL_RegisterResult( int FromWorldId , int FromNetID ,int GuildID , GuildWarRegisterTypeENUM Type , GuildHouseWarInfoStruct& Info , GuildWarRegisterResultENUM Result , int PlayerDBID ) = 0;
				//註冊公會戰( or 取消 )結果
				static	void SC_RegisterResult( int SendtoID , GuildWarRegisterTypeENUM Type , GuildHouseWarInfoStruct& Info , GuildWarRegisterResultENUM Result );

	//公會戰狀況( 開始 , 準備 , 結束 )
	static	void SL_Status( int ToWorldID , int ToGSrvID , GuildWarStatusTypeENUM Status , int NextTime );
	static	void SL_AllWorld_Status( int ToGSrvID , GuildWarStatusTypeENUM Status , int NextTime );
	virtual	void RBL_Status( int FromWorldId , int FromNetID , GuildWarStatusTypeENUM Status , int NextTime ) = 0;	
		//公會戰狀況( 開始 , 準備 , 結束 )
		static	void SC_Status( int SendtoDBID , GuildWarStatusTypeENUM Status , int NextTime );
		static  void SC_AllPlayer_Status( GuildWarStatusTypeENUM Status , int NextTime );

	//要求公會屋資料(取zone401的資料)
	static	void SL_HouseLoadRequest( int ToWorldID , int ToGSrvID ,int GuildID );
	virtual	void RBL_HouseLoadRequest( int FromWorldId , int FromNetID ,int GuildID ) = 0;
		//公會屋基本資料
		static	void SBL_HouseBaseInfo( int ToWorldID , int ToNetID ,GuildHousesInfoStruct& HouseBaseInfo );
		virtual	void RL_HouseBaseInfo( int FromWorldId , int FromNetID ,GuildHousesInfoStruct& HouseBaseInfo ) = 0;
		//公會屋建築資料
		static	void SBL_HouseBuildingInfo( int ToWorldID , int ToNetID ,GuildHouseBuildingInfoStruct Building );
		virtual	void RL_HouseBuildingInfo( int FromWorldId , int FromNetID ,GuildHouseBuildingInfoStruct Building ) = 0;
		//公會屋家具資料
		static	void SBL_HouseFurnitureInfo( int ToWorldID , int ToNetID ,GuildHouseFurnitureItemStruct Item );
		virtual	void RL_HouseFurnitureInfo( int FromWorldId , int FromNetID ,GuildHouseFurnitureItemStruct Item ) = 0;
		//公會屋資料讀取完畢
		static	void SBL_HouseLoadOK( int ToWorldID , int ToNetID ,int GuildID );
		virtual	void RL_HouseLoadOK( int FromWorldId , int FromNetID ,int GuildID ) = 0;

	//要求進入公會戰場
	virtual	void RC_EnterRequest( BaseItemObject* OwnerObj , int ZoneGroupID ) = 0;
		//要求進入公會戰場
		static	void SBL_EnterRequest( int ToWorldID , int ToGSrvID ,int PlayerDBID , int GuildID );
		virtual	void RL_EnterRequest( int FromWorldId , int FromNetID ,int PlayerDBID , int GuildID ) = 0;
			//要求進入結果
			static	void SL_EnterResult( int ToWorldID , int ToNetID ,int PlayerDBID , int GuildID , bool Result );
			virtual	void RBL_EnterResult( int FromWorldId , int FromNetID , int ZoneGroupID ,int PlayerDBID , int GuildID , bool Result ) = 0;
				//要求進入結果
				static	void SC_EnterResult( int SendtoDBID , bool Result );

	//要求離開公會戰場
	virtual	void RC_LeaveRequest( BaseItemObject* OwnerObj ) = 0;
	//要求離開公會戰場結果
	static	void SC_LeaveResult( int SendtoID , bool Result );

	//////////////////////////////////////////////////////////////////////////
	//公會戰績分資訊
	static	void SC_GuildScore( int SendtoID , GuildHouseWarFightStruct& Info );
	static	void SC_AllRoom_GuildScore( int RoomID , GuildHouseWarFightStruct& Info );
	//公會戰開始
	static	void SC_BeginWar( int SendtoID );
	static	void SC_AllRoom_BeginWar( int RoomID );
	//公會戰結束
	static	void SC_EndWar( int SendtoID );
	static	void SC_AllRoom_EndWar( int RoomID );
	static	void SC_AllPlayer_EndWar( );

	static	void SBL_DebugTime( int Time_Day , int Time_Hour );
		virtual void  RL_DebugTime( int FromWorldId , int FromNetID , int Time_Day , int Time_Hour ) = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual void RC_PlayerInfoRequest( BaseItemObject*  Obj ) = 0;
		static void SC_PlayerInfoResult( int SendID , int TotalCount , int ID , GuildWarPlayerInfoStruct& Info );

	static void SC_AllRoom_PlayerListInfo_Zip( int RoomID , vector<GuildWarPlayerInfoStruct*>& PlayerList );

	static void SC_PlayerFinalInfo( int RoomID , int PlayerDBID , int PrizeType , int Score , int OrderID , Voc_ENUM Voc , Voc_ENUM Voc_Sub , int Level , int Level_Sub );
	//////////////////////////////////////////////////////////////////////////
	static void SL_EndWarResult( int ToWorldID , int ToGSrvID , int GuildID , GameResultENUM GameResult , int GuildScore , GuildHouseWarInfoStruct* TargetGuild );
		virtual void RBL_EndWarResult( int GuildID , GameResultENUM GameResult , int GuildScore , GuildWarBaseInfoStruct& TargetGuild ) = 0;
	//////////////////////////////////////////////////////////////////////////
	static void SD_LoadInfo( int ZoneGroupID );
		virtual void RD_OrderInfo( int Count , GuildHouseWarInfoStruct List[] ) = 0;

	virtual void OnDataCenterConnect( ) = 0;
	//////////////////////////////////////////////////////////////////////////
	virtual void RD_RegGuildWarRankInfo( GuildWarRankInfoStruct& Info , GuildHouseWarHistoryStruct Hisotry[50] ) = 0;
	virtual void RC_GuildWarRankInfoRequest( BaseItemObject* Obj , GuildWarRankInfoTypeENUM Type , int WorldGuildID ) = 0;
	virtual void RL_GuildWarRankInfoRequest( int FromWorldId , int FromNetID , int PlayerDBID , GuildWarRankInfoTypeENUM Type , int WorldGuildID ) = 0;
	virtual void RBL_GuildWarHisotry( int PlayerDBID , GuildHouseWarHistoryStruct Hisotry[50] ) = 0;
	virtual void RBL_GuildWarRankInfo( GuildWarRankInfoStruct& Info , int PlayerDBID ) = 0;
	
	static	void SBL_GuildWarRankInfoRequest	( int PlayerDBID , int WorldGuildID , GuildWarRankInfoTypeENUM Type );
	static	void SL_GuildWarHisotry				( int ToWorldID , int ToNetID , int	PlayerDBID , GuildHouseWarHistoryStruct	Hisotry[50] );
	static	void SC_GuildWarHisotry				( int PlayerDBID , GuildHouseWarHistoryStruct Hisotry[50] );
	static	void SL_GuildWarRankInfo			( int ToWorldID , int ToNetID , int PlayerDBID , GuildWarRankInfoStruct& Info );
	static	void SC_GuildWarRankInfo			( int PlayerDBID , GuildWarRankInfoStruct& Info );
};
