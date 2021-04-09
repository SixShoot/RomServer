#pragma once
#pragma warning (disable:4949)
#pragma unmanaged

#ifdef DLL_EXPORTS_GAMESERVER
	#define DLLAPI extern "C" __declspec(dllexport)
#elif  DLL_IMPORTS_GAMESERVER
	#define DLLAPI __declspec(dllimport)
#else 
	#define DLLAPI
#endif


struct GameObjDbStruct;		//資料設定結構(靜態)
class  RoleDataEx;			//角色資料(動態)
class  GSrvInfoClass;		//區域資料


//內部如果有事件要通知From做處理
// Dll(遊戲處理) -> Form
class IGameSrvPluginNotify
{
public:
	virtual	void Close() = 0;
	//-- 玩家進入 -----
	virtual void PlayerEnter( int PlayerDBID , char* PlayerName ) = 0;
	//-- 玩家離開 -----
	virtual void PlayerLeave( int PlayerDBID ) = 0;
	//-- 接收到警告的訊息 -----
	virtual void RecvWarningMessage( int PrintLV , char* TitleStr , char* MsgStr ) = 0;

	//所有玩家基本資料
/*	virtual void RecvPlayerMessage( int GUID , int DBID , )
		void    GameSrvForm::AddOnlinePlayer	( int GUID , int DBID , int Race , int Job , int Job_Sub 
		, int LV , int LV_Sub , char* Account , char* RoleName , char* IP )
*/

	virtual void OnAddObj( RoleDataEx* Role ) = 0;
	virtual void OnDelObj( RoleDataEx* Role ) = 0;
};

//Form 對遊戲下命令 或取資料
//Form -> Dll(遊戲處理)
class IFormCommand
{
public:
	virtual	void Command( char* cmd ) = 0;
	virtual void Destroy( ) = 0;
	virtual bool IsCrash( ) = 0;
	virtual bool IsThreadRuning() = 0;
//	virtual bool Process( ) = 0;			//定時處理

	virtual	GameObjDbStruct*	GetObjDB( int OrgObjID ) = 0;
	virtual RoleDataEx*			GetRole( int GUID ) = 0;
	virtual GSrvInfoClass*		GetZoneInfo( ) = 0;
	
	virtual	void				SysMsg( int GUID , char* Msg ) = 0;
	
};



//////////////////////////////////////////////////////////////////////////
//初始化
DLLAPI IFormCommand*  DllCreateServerPluginFunc( char* IniFile , IGameSrvPluginNotify* FromNotify );
//結束釋放資源
DLLAPI void  DllDestoryServerPluginFunc( );


typedef IFormCommand* (*CreatePluginFunc)( char* IniFile , IGameSrvPluginNotify* FromNotify );
typedef void (*DestoryPluginFunc)( void );
//////////////////////////////////////////////////////////////////////////

#pragma managed