#include "NetDC_ItemChild.h"	
//-------------------------------------------------------------------

//-------------------------------------------------------------------

bool    Net_ItemChild::Init()
{
	Net_Item::_Init();

	if( This != NULL)
		return false;

	This = NEW( Net_ItemChild );


	return true;
}

//--------------------------------------------------------------------------------------
bool    Net_ItemChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	Net_Item::_Release();
	return true;

}	
//////////////////////////////////////////////////////////////////////////
struct AccountBagInfoRequestTempStruct
{
	int ServerID;
	int PlayerDBID;
	string Account;

	//---------------------------------------
	int					MaxCount;
	int					ItemDBID[20];
	ItemFieldStruct		Item[20];
};

void  Net_ItemChild::RL_AccountBagInfoRequest( int ServerID , int PlayerDBID , char* Account )
{
	AccountBagInfoRequestTempStruct* TempData = NEW(AccountBagInfoRequestTempStruct);
	TempData->ServerID = ServerID;
	TempData->PlayerDBID = PlayerDBID;
	TempData->Account = Account;
	TempData->MaxCount = 0;

	_RD_NormalDB_Import->SqlCmd( PlayerDBID , _SQLCmdAccountBagProc , _SQLCmdAccountBagResult , TempData	, NULL );
}

bool Net_ItemChild::_SQLCmdAccountBagProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg )
{
	AccountBagInfoRequestTempStruct* TempData = (AccountBagInfoRequestTempStruct*) UserObj;

	char SqlCmd[512];
	
	MyDbSqlExecClass MyDBProc( sqlBase );
	sprintf( SqlCmd , "SELECT COUNT(GUID) FROM AccountBag WHERE Account_ID = '%s' and WorldID=%d and ( PlayerDBID =-1 or PlayerDBID =%d)" , TempData->Account.c_str() , Global::GetWorldID() , TempData->PlayerDBID );
	MyDBProc.SqlCmd( SqlCmd );
	MyDBProc.Bind( 1, SQL_C_LONG, SQL_C_DEFAULT , (LPBYTE)&TempData->MaxCount );
	MyDBProc.Read();
	MyDBProc.Close();

	if( TempData->MaxCount == 0 )
		return false;

	//g_CritSect()->Enter();
	sprintf( SqlCmd , "WHERE Account_ID = '%s' and WorldID=%d and ( PlayerDBID =-1 or PlayerDBID =%d)" 
		, TempData->Account.c_str() 
		, Global::GetWorldID()  
		, TempData->PlayerDBID		);
	MyDBProc.SqlCmd( _RDAccountBagSql->GetSelectStr( SqlCmd , 20 ).c_str() );
	//g_CritSect()->Leave();

	AccountBagItemFieldDBStruct TempAccountBagItem;
	TempAccountBagItem.Init();
	MyDBProc.Bind( TempAccountBagItem , RoleDataEx::ReaderRD_AccountBag() );
	int Count = 0;
	while( MyDBProc.Read( ) )
	{
		TempData->ItemDBID[ Count ] = TempAccountBagItem.ItemDBID;
		TempData->Item[ Count ] = TempAccountBagItem.Item;

		Count++;
	}
	
	MyDBProc.Close();
	return true;
}

void Net_ItemChild::_SQLCmdAccountBagResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK )
{
	AccountBagInfoRequestTempStruct* TempData = (AccountBagInfoRequestTempStruct*) UserObj;

	SL_AccountBagInfoResult( TempData->ServerID , TempData->PlayerDBID , TempData->MaxCount , TempData->ItemDBID , TempData->Item );

	delete TempData;
}
