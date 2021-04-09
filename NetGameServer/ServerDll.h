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


struct GameObjDbStruct;		//��Ƴ]�w���c(�R�A)
class  RoleDataEx;			//������(�ʺA)
class  GSrvInfoClass;		//�ϰ���


//�����p�G���ƥ�n�q��From���B�z
// Dll(�C���B�z) -> Form
class IGameSrvPluginNotify
{
public:
	virtual	void Close() = 0;
	//-- ���a�i�J -----
	virtual void PlayerEnter( int PlayerDBID , char* PlayerName ) = 0;
	//-- ���a���} -----
	virtual void PlayerLeave( int PlayerDBID ) = 0;
	//-- ������ĵ�i���T�� -----
	virtual void RecvWarningMessage( int PrintLV , char* TitleStr , char* MsgStr ) = 0;

	//�Ҧ����a�򥻸��
/*	virtual void RecvPlayerMessage( int GUID , int DBID , )
		void    GameSrvForm::AddOnlinePlayer	( int GUID , int DBID , int Race , int Job , int Job_Sub 
		, int LV , int LV_Sub , char* Account , char* RoleName , char* IP )
*/

	virtual void OnAddObj( RoleDataEx* Role ) = 0;
	virtual void OnDelObj( RoleDataEx* Role ) = 0;
};

//Form ��C���U�R�O �Ψ����
//Form -> Dll(�C���B�z)
class IFormCommand
{
public:
	virtual	void Command( char* cmd ) = 0;
	virtual void Destroy( ) = 0;
	virtual bool IsCrash( ) = 0;
	virtual bool IsThreadRuning() = 0;
//	virtual bool Process( ) = 0;			//�w�ɳB�z

	virtual	GameObjDbStruct*	GetObjDB( int OrgObjID ) = 0;
	virtual RoleDataEx*			GetRole( int GUID ) = 0;
	virtual GSrvInfoClass*		GetZoneInfo( ) = 0;
	
	virtual	void				SysMsg( int GUID , char* Msg ) = 0;
	
};



//////////////////////////////////////////////////////////////////////////
//��l��
DLLAPI IFormCommand*  DllCreateServerPluginFunc( char* IniFile , IGameSrvPluginNotify* FromNotify );
//��������귽
DLLAPI void  DllDestoryServerPluginFunc( );


typedef IFormCommand* (*CreatePluginFunc)( char* IniFile , IGameSrvPluginNotify* FromNotify );
typedef void (*DestoryPluginFunc)( void );
//////////////////////////////////////////////////////////////////////////

#pragma managed