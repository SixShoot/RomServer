#ifndef __GLOBAL_H__
#define __GLOBAL_H__
//----------------------------------------------------------------------------------------------------
//#include "../NetWaker/CliNetWaker.h"
#include <Windows.h>
#include <WinSCard.h>
#include "ReaderClass/DbSqlExecClass.h"
#include "IniFile/IniFile.h"
#include "lua/myvm/lua_vmclass.h"
#include "debuglog/errormessage.h"
#include "DBStruct/DBStructTransfer.h"
//#include "BaseItemObject/DC_BaseItemObject.h"
#include "ReaderClass/CreateDBCmdClass.h"
#include "RoleData/RoleDataEx.h"
#include "RoleData/ObjectDataClass.h"
#include "GlobalBase.h"
#include "ControllerClient/ControllerClient.h"
#include "PG/NetWakerPGEnum.h"
//----------------------------------------------------------------------------------------------------
struct DBRoleTempDataInfo
{
    string  Account;
    int     SrvID;
    int     LocalObjID;
    int     DBID;
    int     Field;	

    RoleDataEx* Role;
//    BaseItemObject* Obj;

	PlayerAccountBaseInfoStruct AccountBaseInfo;
};
//----------------------------------------------------------------------------------------------------
class Global : public GlobalBase
{
protected:
    static RecycleBin< DBRoleTempDataInfo >*    					_ReadRole;
    static IniFileClass	                        					_Ini;
    static DBTransferClass*                     					_DBTransfer_Game;		//db�ǰe��ƪ�����
    static DBTransferClass*                     					_DBTransfer_Global;		//db�ǰe��ƪ�����
	static DBTransferClass*                     					_DBTransfer_Import;		//db�ǰe��ƪ�����
	static DBTransferClass*                     					_DBTransfer_Log;		//db�ǰe��ƪ�����
	static DBTransferClass*                     					_DBTransfer_External;	//db�ǰe��ƪ�����
    static DBStructTransferClass< NPCData >*    					_NPCDataDB;       		//db�൲�c������
	static DBStructTransferClass< DB_MailStruct >*					_RD_MailDB;				//db�൲�c������(�����ϥ�SQL�R�O)
	static DBStructTransferClass< DB_ACItemStruct >*				_RD_ACItemDB;			//db�൲�c������(�����ϥ�SQL�R�O)
	static DBStructTransferClass< DB_BillBoardRoleInfoStruct >*		_RD_BillBoardDB;		//db�൲�c������(�����ϥ�SQL�R�O)

	static CreateDBCmdClass<DB_MailItemStruct>*				_RDMailItemSql;
	static CreateDBCmdClass<DB_MailStruct>*		   			_RDMailBaseSql;

	static CreateDBCmdClass<DB_ACItemStruct>*				_RDACBaseSql;
	static CreateDBCmdClass<DB_ACItemHistoryStruct>*		_RDACHistorySql;

	static CreateDBCmdClass<DB_BillBoardRoleInfoStruct>*	_RDBillBoardSql;

	static CreateDBCmdClass<DB_NPC_MoveBaseStruct>*			_RDNPCMoveSql;

	static CreateDBCmdClass<GuildBaseStruct>*		   		_RDGuildBaseSql;
	static CreateDBCmdClass<GuildMemberStruct>*		   		_RDGuildMemberSql;
	static CreateDBCmdClass<GuildBoardStruct>*		   		_RDGuildBoardSql;
	//////////////////////////////////////////////////////////////////////////
	//�ЫΨt��
	static CreateDBCmdClass<HouseBaseDBStruct>*		   		_RDHouseBaseSql;
	static CreateDBCmdClass<HouseItemDBStruct>*		   		_RDHouseItemSql;

	//////////////////////////////////////////////////////////////////////////
	//Account Bag
	static CreateDBCmdClass<AccountBagItemFieldDBStruct>*	_RDAccountBagSql;
	static CreateDBCmdClass<WebShopBagItemFieldDBStruct>*	_RDWebShopBagSql;
	//////////////////////////////////////////////////////////////////////////
	//���u�T�����
	static CreateDBCmdClass<OfflineMessageDBStruct>*		_RDOfflineMessageSql;
    //////////////////////////////////////////////////////////////////////////
	//���|��
	static CreateDBCmdClass<GuildHousesInfoStruct>*		   	_RDGuildHouseBaseSql;
	static CreateDBCmdClass<GuildHouseItemStruct>*		   	_RDGuildHouseItemSql;
	static CreateDBCmdClass<GuildHouseBuildingInfoStruct>*	_RDGuildHouseBuildingSql;

	static CreateDBCmdClass<GuildHouseFurnitureItemStruct>*	_RDGuildHouseFurnitureSql;

	//////////////////////////////////////////////////////////////////////////
    static bool                                 _IsDelay;
    static bool                                 _IsDestory;
	static int									_TimeZone;
	static int									_WorldID;
	
	//////////////////////////////////////////////////////////////////////////
	//�x�s���ե�
	static int									_SaveCount;
	static int									_MaxSaveCount;
	static int									_ProcSaveCount;
	static int									_BeginTime;	
	//static LanguageTypeENUM						_LanguageType;
	static CountryTypeENUM						_CountryType;
	//////////////////////////////////////////////////////////////////////////
	//����|
	static float								_AC_Tax_Money;
	static float								_AC_Tax_AccountMoney;
	static bool									_DisableLottery;
	static bool									_EnableWorldConfigTable;
	static bool									_IgnoreObjNameStringDB;

	//����DB�x�s�t��
	static void   _TestDBSaveEventResultCB ( vector<int>& ,void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	static bool   _TestDBSaveEventEventCB ( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );

	static int		_OnTimeProc_10Sec_( SchedularInfo* Obj , void* InputClass );
public:
	//static int									_DebugPacket;
public:
	//���|�ξԸ��
	static CreateDBCmdClass<GuildHouseWarInfoStruct>*		_RDGuildHouseWarInfoSql;
	static CreateDBCmdClass<GuildHouseWarHistoryStruct>*	_RDGuildHouseWarHistorySql;

	static DBStructTransferClass< int >* _RD_NormalDB;
	static DBStructTransferClass< int >* _RD_NormalDB_Global;
	static DBStructTransferClass< int >* _RD_NormalDB_Import;
	static DBStructTransferClass< int >* _RD_NormalDB_Log;
	static DBStructTransferClass< int >* _RD_NormalDB_External;
	//�@��ID
	//static int									_WorldID;
	static int		GetWorldID();
    static bool     Init( char* IniFile );
    static bool     Release( );
    static bool     Process( );
	static void		Destory() { _IsDestory = true; };

	static bool		SendToCli_ByRoleName( const char* RoleName , int Size , void* Data );
	static IniFileClass*	Ini() {return &_Ini; };

	//����DB�x�s�t��
	static void		SaveTest( int Count );

	static void		Log_ServerStatus( );
	static void		Log_GuildLog( int GuildID , GuildActionTypeENUM Type , int LeaderDBID , int PlayerDBID );
	static void		Log_Money( int PlayerDBID , ActionTypeENUM Type , int Money );
	static void		Log_Money_Account( int PlayerDBID , ActionTypeENUM Type , int Money );
	static void		Log_Money_Bonus( int PlayerDBID , ActionTypeENUM Type , int Money );
	static void		Log_AC( DB_ACItemStruct* Item , AcActionTypeENUM Type , int BuyerDBID , int EventMoney  );

	static void		GuildResourceLog( int GuildID , int PlayerDBID , GuildResourceStruct& Resource , int LogType , int LogType_ObjID );

	static  void	SendToOtherWorld_GSrvID				( int WorldID , int GSrvID , int Size , void* Data );
	static  void	SendToOtherWorld_NetID				( int WorldID , int NetID , int Size , void* Data );
	static  void	SendToOtherWorld_ServerType			( int WorldID , EM_SERVER_TYPE iServerType , int Size , void* Data );

	static	void	SendToOtherWorldSrvClient_ByDBID	( int iWorldID, int iSrvID, int iClientDBID, int iSize, void* pData );
	static	void	SendToOtherWorldZoneClient_ByDBID	( int iWorldID, int iZoneID, int iClientDBID, int iSize, void* pData );


};
/*
// �p���۩w Define
//------------------------------------------------------
#define M_PACKET( type ) type* pPacket = (type*)pData
//----------------------------------------------------------------------------------------------------
*/
#endif //__GLOBAL_H__