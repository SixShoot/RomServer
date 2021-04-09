#include "DC_BaseItemObject.h"
#include <string.h>
#include "../Global.h"
#include "AllOnlinePlayerInfo/AllOnlinePlayerInfo.h"
#include "../../NetWalker_Member/NetWakerRoleDataCenterInc.h"
#include "CreateDump/CreateDump.h"
#include <direct.h>

//----------------------------------------------------------------------------------------
//		�R�A�ܼ�
//----------------------------------------------------------------------------------------
BaseItemObject::StaticStruct*	BaseItemObject::_St = NULL;
//----------------------------------------------------------------------------------------
//		�R�A
//----------------------------------------------------------------------------------------
BaseItemObject::StaticStruct::StaticStruct()
{
//	Schedular = FunctionSchedularClass::New();
}
BaseItemObject::StaticStruct::~StaticStruct()
{
//	Schedular->Del();
}
//----------------------------------------------------------------------------------------
void    BaseItemObject::Init( )
{
	if( _St != NULL )
		return;

	_St = NEW( StaticStruct );
	//_St->Schedular = FunctionSchedularClass::New();
    //�]�w�w�ɰ���OnTimeProc
    _OnTimeProc_Sec( NULL , NULL );
}
void    BaseItemObject::Release( )
{
	map< int , BaseItemObject* >::iterator Iter;
	
	_St->DelList.clear();
	for(	Iter = _St->AllPlayerObj.begin();
			Iter != _St->AllPlayerObj.end()	;
			Iter++ )
	{
		if( Iter->second->_IsInDelList == false )
		{
			Iter->second->_IsInDelList = true;
			_St->DelList.push_back( Iter->second );
		}
	}

	for( int i = 0 ; i < (int)_St->DelList.size() ; i++ )
	{
		delete _St->DelList[i];
	}
	_St->DelList.clear();

	if( _St != NULL )
		delete _St;
	_St = NULL;
}
//----------------------------------------------------------------------------------------
int		BaseItemObject::_OnTimeProc_Sec( SchedularInfo* Obj , void* InputClass )
{
    OnTimeProc( );
	Global::Schedular()->Push( _OnTimeProc_Sec , _Def_OnTimeProc_Time_ , NULL , NULL );
    return 0;
}
//----------------------------------------------------------------------------------------
bool	BaseItemObject::CheckMemory( )
{ 
    return _St->RoleDataRecycle.CheckAllMem( ); 
}
//----------------------------------------------------------------------------------------
//���o�Y��ID��������
BaseItemObject* BaseItemObject::GetObj( int ID )
{
    map< int , BaseItemObject* >::iterator    Iter;
    Iter = _St->AllPlayerObj.find( ID );
    if( Iter == _St->AllPlayerObj.end() )
        return NULL;

    return Iter->second;
}
//----------------------------------------------------------------------------------------
vector< BaseItemObject*>* BaseItemObject::GetObj_Account( char* Account )
{
    _St->Ret.clear();

	_strlwr( Account );
    multimap< string , BaseItemObject* >::iterator  MulIter;
    int Count = (int)_St->AllPlayerObj_ByAccounID.count( Account );
    int i;

    MulIter = _St->AllPlayerObj_ByAccounID.find( Account );

    for( i = 0 ; i < Count ; i++ , MulIter++ )
    {
        _St->Ret.push_back( MulIter->second );
    }

    return &(_St->Ret);
}
//----------------------------------------------------------------------------------------
//�����x�s��
void	BaseItemObject::SaveAll_Block()
{
	map< int , BaseItemObject* >::iterator    Iter;
	BaseItemObject*     Obj;
	//�B�z���ϰ�Ҧ�������

	for( Iter = _St->AllPlayerObj.begin() ; Iter != _St->AllPlayerObj.end() ; Iter++ )
	{
		Obj = Iter->second;

		if(  Obj->_IsNeedSave == false )
			continue;

		if( Obj->IsSave() == false )
		{
			Global::SavePlayerDB( Obj );
		}

	}

	for( Iter = _St->AllPlayerObj.begin() ; Iter != _St->AllPlayerObj.end() ; Iter++ )
	{
		Obj = Iter->second;

		while( Obj->IsSave() == true )
		{
			Global::DBProcess();
			Sleep(1000);
		}
	}


}
//�D�n�ݦ��S������n�R��(�w�ɭn�B�z) 5 �C��
void	BaseItemObject::OnTimeProc( )
{
    map< int , BaseItemObject* >::iterator    Iter;
    BaseItemObject*     Obj;
    //�B�z���ϰ�Ҧ�������
	_St->WaitLogoutCount = 0;
	_St->NeedSaveCount = 0;
	_St->SaveCount	= 0;

    for( Iter = _St->AllPlayerObj.begin() ; Iter != _St->AllPlayerObj.end() ; Iter++ )
    {
        Obj = Iter->second;

		Obj->_SaveTime --;

		if(  Obj->_IsNeedSave == true )
			_St->NeedSaveCount++;


		if( Obj->_SaveTime < 0 &&  Obj->_IsNeedSave == true )
		{	
			if( Obj->IsSave() == false )
			{
				_St->SaveCount++;
				Global::SavePlayerDB( Obj );
			}
			
			continue;
		}

        if( Obj->_LiveTime - clock() > 0  )
		{
			Obj->_DelErrCount = 0;
            continue;
		}

        if( Obj->_IsNeedSave == true )
        {
			if( Obj->IsSave() == false )
			{
				_St->SaveCount++;
				Global::SavePlayerDB( Obj );
			}
            continue;
        }

        if( Obj->_ThreadUseCount > 0 )
		{
//			if( Obj->_DelErrCount++ < 10 )
			continue;				

//			Print( LV4 , "OnTimeProc" , "�R�������D�� (�j��R��), (Obj->_ThreadUseCount=%d > 0) Not Leave World DBID = %d" , Obj->_ThreadUseCount , Obj->_DBID );
		}

		if( Obj->Role() == NULL )
		{
			if( Obj->_IsInDelList == false )
			{
				Obj->_IsInDelList = true;
				_St->DelList.push_back( Obj );
			}
			continue;
		}
			

		OnlinePlayerInfoStruct* OnlinePlayer = AllOnlinePlayerInfoClass::This()->GetInfo_ByDBID( Obj->Role()->Base.DBID );
		if( OnlinePlayer != NULL  )
		{			
			if( Obj->_DelErrCount++ > 30 )
			{
				AllOnlinePlayerInfoClass::This()->DelPlayer( Obj->Role()->Base.DBID );
				Print( LV4 , "OnTimeProc" , "�R�������D�� (�j��R��), Not Leave World DBID = %d Account = %s Name = %s" , OnlinePlayer->DBID , OnlinePlayer->Account.Begin() , Utf8ToChar( OnlinePlayer->Name.Begin() ).c_str()  );
			}
			else
			{
				if( Obj->_DelErrCount % 10 == 0 )
				{
					CNetDC_DCBase::SL_CheckRoleLiveTime( OnlinePlayer->ZoneID , OnlinePlayer->DBID );
				}
				Print( LV3 , "OnTimeProc" , "�R�������D�� , Not Leave World DBID = %d Account = %s Name = %s" , OnlinePlayer->DBID , OnlinePlayer->Account.Begin() , Utf8ToChar( OnlinePlayer->Name.Begin() ).c_str()  );
				_St->WaitLogoutCount++;
				continue;

			}
		}


		if( Obj->_IsInDelList == false )
		{
			Obj->_IsInDelList = true;
			_St->DelList.push_back( Obj );
		}
    }

    for( int i = 0 ; i < (int)_St->DelList.size() ; i++ )
    {
        delete _St->DelList[i];
    }

    _St->DelList.clear();
}
//----------------------------------------------------------------------------------------
//		�ʺA
//----------------------------------------------------------------------------------------
BaseItemObject::BaseItemObject(  PlayerRoleData* Role  )
{	
	memset( _CheckMemory , -1 , sizeof( _CheckMemory ) );
	_IsInDelList = false;

    //��ܨ����Ƥw�g�s�b�h���B�z
	if(		Role->Base.DBID == -1 
		||	_St->AllPlayerObj.find( Role->Base.DBID ) != _St->AllPlayerObj.end() )
    {
        _LiveTime = 0;
        _Role     = NULL;
        _DBID     = -1;
        Print(LV2, "BaseItemObject", "DBID = %d �w�s�b�A?????????\n" , Role->Base.DBID );
        //���[�J_St->AllPlayerObj �ҥH�n�ۤv Del
		_IsInDelList = true;
        _St->DelList.push_back( this );
        return;
    }

    _Role  = _St->RoleDataRecycle.NewObj();
	_Role->SetLink();
	//_RoleDB = _St->RoleDataRecycle.NewObj();
    
    if( _Role == NULL  )
    {
        _LiveTime = 0;
        _DBID     = -1;
        Print(LV2, "BaseItemObject", "���͹L�h������A�����ƪ��󲣥ͤ��X�� or ��Ƭ��^��\n");

        //���[�J_St->AllPlayerObj �ҥH�n�ۤv Del
        _St->DelList.push_back( this );
        return ;
    }


//  memcpy( _Role   , Role , sizeof( _Role ) );
//	memcpy( _RoleDB , Role , sizeof( _RoleDB ) );
	
	_Role->Copy( Role );
	//_RoleDB->Copy( Role );
	_RoleDBZip = NULL;
	SetRoleDBZip( _Role );

    _DBID                                   = Role->Base.DBID;
    _AccountID                              = Role->Base.Account_ID.Begin();
    _St->AllPlayerObj[ _DBID ]              = this;
    _LiveTime                               = clock() + 600*1000;
	_IsNeedSave								= false;
    _IsSave                                 = false;
	_SaveTime								= 12 * 30;	//�ȩw�b�p��
    _ThreadUseCount                         = 0;
	_DelErrCount							= 0;
	_IsLoadOK								= false;
	_IsNeedSaveAll							= false;
	_strlwr( (char*)_AccountID.c_str() );

    typedef multimap< string , BaseItemObject* >::value_type valueType;
    _St->AllPlayerObj_ByAccounID.insert( valueType( _AccountID , this ) );

    typedef multimap< int , BaseItemObject* >::value_type valueType1;
    _St->AllPlayerObj_DBID.insert( valueType1(_DBID , this ) );

}
//----------------------------------------------------------------------------------------
void		BaseItemObject::SetRoleDBZip( PlayerRoleData* role )
{
	if( _RoleDBZip != NULL )
		delete _RoleDBZip;
	_RoleDBZip = NULL;

	char	zipBuf[200000];
	//�t�m���Y�ɰO����
	MyLzoClass myZip;
	_RoleDBZipSize = myZip.Encode( (char*)(role) , sizeof( *role ) , (char*)zipBuf );
	_RoleDBZip = new char[ _RoleDBZipSize] ;
	memcpy( _RoleDBZip , zipBuf , _RoleDBZipSize );
}
bool		BaseItemObject::GetRoleDB( PlayerRoleData& role )
{
	if( _RoleDBZip == NULL )
		return false;

	MyLzoClass myZip;
	int RetSize = myZip.Decode( _RoleDBZip , _RoleDBZipSize , sizeof(PlayerRoleData) , (char*)&role );
	return true;
}
//----------------------------------------------------------------------------------------
void		BaseItemObject::ClearRoleDB()
{
	_LiveTime                               = clock() + 600*1000;
	_IsNeedSave								= true;
	_SaveTime								= 12 * 30;	//�ȩw�b�p��
	_IsNeedSaveAll							= true;
}
//----------------------------------------------------------------------------------------
void		OuputMemoryInfo( int checkMemory[256] )
{
	static int dumpCount = 0;
	for( int i = 0 ; i < 256 ; i+=4 )
		//for( int j = 0 ; j < 8 ; j++ )
		{
			Print( LV5 , "Memory Err" , "%03d %08x %08x %08x %08x"
				, i
				, checkMemory[ i ]
				, checkMemory[ i+1 ]
				, checkMemory[ i+2 ]
				, checkMemory[ i+3 ] );
		}
		g_pPrint->OutputBuffToFile();

	if( dumpCount > 10 )
		return;
	dumpCount++;


	_mkdir("Dump");
	wchar_t fileName[256];
	swprintf( fileName , L"Dump/PlayerCenter_%d.dmp" , TimeStr::Now_Value()  );

	CreateDump( fileName , EM_DumpType_MAX  );
}

BaseItemObject::~BaseItemObject( )
{
	{
		bool IsMemoryErr = false;
		for( int i = 0 ; i < 256 ; i++ )
		{
			if( _CheckMemory[i] != -1 )
			{		
				OuputMemoryInfo( _CheckMemory );
				break;
			}
		}
	}

    if( _DBID == -1 )
    {
		Print( LV2 , "~BaseItemObject" , "�R�������D��BaseItemObject DBID = -1" );
        return;
    }

	map< int , BaseItemObject* >::iterator          Iter;
	Iter = _St->AllPlayerObj.find( _DBID );
	if( Iter == _St->AllPlayerObj.end() )
	{
		Print( LV4 , "~BaseItemObject" , "�����D _DBID(%d) >= _St->AllPlayerObj.size() ????????????" , _DBID );
		return ;
	}

	if( _Role != NULL )
	{
		Print(LV2, "~BaseItemObject", "�R�� BaseItemObject DBID = %d RoleName(%s)\n" , _DBID  , Utf8ToChar( _Role->BaseData.RoleName.Begin() ).c_str() );
	}


    _St->AllPlayerObj.erase( Iter );
    if( _Role == NULL  )
    {
        Print( LV4 , "~BaseItemObject" , "�L�k�^����������" );
        return;
    }
	if( _RoleDBZip != NULL )
		delete _RoleDBZip;

    multimap< string , BaseItemObject* >::iterator  MulIter;
    int Count = (int)_St->AllPlayerObj_ByAccounID.count( _AccountID );
    int i;

    MulIter = _St->AllPlayerObj_ByAccounID.find( _AccountID );

    for( i = 0 ; i < Count ; i++ , MulIter++ )
    {
        if( MulIter->second == this )
        {
            _St->AllPlayerObj_ByAccounID.erase( MulIter );
            break;
        }
    }



    _St->RoleDataRecycle.DeleteObj( _Role );
//	_St->RoleDataRecycle.DeleteObj( _RoleDB );
    _St->AllPlayerObj_DBID.erase( _DBID );


	vector< BaseItemObject*>* RoleList =  GetObj_Account( (char*)_AccountID.c_str() );
	if( RoleList->size() == 0 )
	{
		Global::UpdateAccountLogout( _AccountID.c_str() );
		Global::UpdateRolePlayerCount( _AccountID.c_str() );
	}
}
//----------------------------------------------------------------------------------------
//�ˬd�O�_�Y�� Account �O�Ū�
bool             BaseItemObject::CheckAccountEmpty( string Account )
{
	_strlwr( (char*)Account.c_str() );
    multimap< string , BaseItemObject* >::iterator  MulIter;
    int Count = (int)_St->AllPlayerObj_ByAccounID.count( Account );
    if( Count == 0 )
        return true;
    return false;
}

bool             BaseItemObject::CheckAccountIsLoading( string Account )    //�ˬd�O�_�Y�� Account �O�Ū�
{
	_strlwr( (char*)Account.c_str() );
	multimap< string , BaseItemObject* >::iterator  MulIter;
	int Count = (int)_St->AllPlayerObj_ByAccounID.count( Account );
	if( Count == 0 )
		return false;

	MulIter = _St->AllPlayerObj_ByAccounID.find( Account );

	for( int i = 0 ; i < Count ; i++ , MulIter++ )
	{
		if( MulIter->second->IsLoadOK() == false  )
			return true;
	}
	return false;
}
//----------------------------------------------------------------------------------------
//���o������
PlayerRoleData*		BaseItemObject::Role( ) 
{ 
    return _Role; 
}		
//----------------------------------------------------------------------------------------

//���o������
char*		BaseItemObject::RoleDBZip( ) 
{ 
    return _RoleDBZip; 
}	
//----------------------------------------------------------------------------------------
void			BaseItemObject::Destroy( ) 
{ 
    _LiveTime = 0;
}
//----------------------------------------------------------------------------------------
BaseItemObject* BaseItemObject::GetObj_DBID( int DBID )
{
    map< int , BaseItemObject* >::iterator Iter;
    Iter = _St->AllPlayerObj_DBID.find( DBID );
    if( Iter == _St->AllPlayerObj_DBID.end() )
        return NULL;

    if( Iter->second->Role() == NULL ) 
        return NULL;

    return Iter->second;
}
BaseItemObject* BaseItemObject::GetObj_Name( const char* Name )
{
	map< int , BaseItemObject* >::iterator Iter;
	for( Iter = _St->AllPlayerObj.begin() ; Iter != _St->AllPlayerObj.end() ; Iter++ )
	{
		if( stricmp( Iter->second->Role()->BaseData.RoleName , Name ) == 0 )
			return Iter->second;
	}
	return NULL;
}
//----------------------------------------------------------------------------------------
void    BaseItemObject::SetLiveTime( int Value )
{
    _LiveTime = clock() + Value;
}
//----------------------------------------------------------------------------------------
void    BaseItemObject::PushToThread( )
{
    _ThreadUseCount++;
}

void    BaseItemObject::PopFromThread( )
{
    _ThreadUseCount--;
    if( _ThreadUseCount < 0 )
    {
        Print( LV5 , "PopFromThread" , "���~ " );
    }
}