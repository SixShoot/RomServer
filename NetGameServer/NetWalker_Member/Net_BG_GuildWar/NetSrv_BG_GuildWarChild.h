#pragma once
#include "NetSrv_BG_GuildWar.h"
//////////////////////////////////////////////////////////////////////////
//���|��Rank��T
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


	//�n�D����A�����|�W�٭n�D
	virtual	void RC_GuildNameRequest( BaseItemObject* OwnerObj , int WorldGuildID );
	//�n�D����A�����|�W�٭n�D
	virtual	void RBL_GuildNameRequest( int FromWorldId , int FromNetID ,int PlayerDBID , int WorldGuildID );
	//�n�D����A�����|�W�ٵ��G
	virtual	void RL_GuildNameResult( int FromWorldId , int FromNetID ,int PlayerDBID , int WorldGuildID , const char* GuildName );

	//�}�ҵ��U���|�Ԥ���
	virtual	void RC_OpenMenuRequest( BaseItemObject* OwnerObj );
	//�n�D���|�Ը�T
	virtual	void RL_OpenMenuRequest( int FromWorldId , int FromNetID ,int PlayerDBID );
	//���|�Զ}�Ҫ��p
	virtual	void RBL_OpenMenuResult( int FromWorldId , int FromNetID ,int PlayerDBID , int ZoneGroupID , int NextTime , GuildHouseWarStateENUM State , int Count , GuildHouseWarInfoStruct List[1000] );
	//���U���|��( or ���� )
	virtual	void RC_RegisterRequest( BaseItemObject* OwnerObj , int ZoneGroupID , GuildWarRegisterTypeENUM Type , int GuildCount , bool IsAcceptAssignment , int TargetGuild );
	//���U���|��( or ���� )
	virtual	void RL_RegisterRequest( int FromWorldId , int FromNetID , GuildWarRegisterInfoStruct& Info );
	//���U���|��( or ���� )���G
	virtual	void RBL_RegisterResult( int FromWorldId , int FromNetID ,int GuildID , GuildWarRegisterTypeENUM Type , GuildHouseWarInfoStruct& Info , GuildWarRegisterResultENUM Result , int PlayerDBID );
	//���|�Ԫ��p( �}�l , �ǳ� , ���� )
	virtual	void RBL_Status( int FromWorldId , int FromNetID , GuildWarStatusTypeENUM Status , int NextTime );
	//�n�D���|�θ��(��zone401�����)
	virtual	void RBL_HouseLoadRequest( int FromWorldId , int FromNetID ,int GuildID );
	//���|�ΰ򥻸��
	virtual	void RL_HouseBaseInfo( int FromWorldId , int FromNetID ,GuildHousesInfoStruct& HouseBaseInfo );
	//���|�Ϋؿv���
	virtual	void RL_HouseBuildingInfo( int FromWorldId , int FromNetID ,GuildHouseBuildingInfoStruct Building );
	//���|�ήa����
	virtual	void RL_HouseFurnitureInfo( int FromWorldId , int FromNetID ,GuildHouseFurnitureItemStruct Item );
	//���|�θ��Ū������
	virtual	void RL_HouseLoadOK( int FromWorldId , int FromNetID ,int GuildID );
	//�n�D�i�J���|�Գ�
	virtual	void RC_EnterRequest( BaseItemObject* OwnerObj , int ZoneGroupID );
	//�n�D�i�J���|�Գ�
	virtual	void RL_EnterRequest( int FromWorldId , int FromNetID ,int PlayerDBID , int GuildID );
	//�n�D�i�J���G
	virtual	void RBL_EnterResult( int FromWorldId , int FromNetID , int ZoneGroupID ,int PlayerDBID , int GuildID , bool Result );
	//�n�D���}���|�Գ�
	virtual	void RC_LeaveRequest( BaseItemObject* OwnerObj );
	//�]�w�ثe���|�Ԯɶ�
	virtual void RL_DebugTime( int FromWorldId , int FromNetID , int Time_Day , int Time_Hour );

	//Client �ݭn�D���|�Գ����a���
	virtual void RC_PlayerInfoRequest( BaseItemObject*  Obj );

	//���|�Ե��G
	virtual void RBL_EndWarResult( int GuildID , GameResultENUM GameResult , int GuildScore , GuildWarBaseInfoStruct& TargetGuild );
	//////////////////////////////////////////////////////////////////////////
	//�Ԫ��}�l
	static void WarPerpare		( int GuildID1 , int GuildID2 , bool IsAssignment);

	//�q���Y�H�Գ�������
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

