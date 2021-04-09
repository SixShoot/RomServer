#pragma once

#include "NetDC_Houses.h"



//--------------------------------------------------------------------------------------------
class CNetDC_HousesChild : public CNetDC_Houses
{
	//��l���J
	static bool 	_SQLCmdInitProc( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void 	_SQLCmdInitProcResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	//--------------------------------------------------------------------------------------------
	//�إߩЫ�
	static bool _SQLCmdBuildHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdBuildHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//--------------------------------------------------------------------------------------------
	//�j�M�Фl
	static bool _SQLCmdFindHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdFindHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//--------------------------------------------------------------------------------------------
	//���J�Ыθ��
	static bool _SQLCmdLoadHouse( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdLoadHouseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//--------------------------------------------------------------------------------------------
	//�ߧY�x�s����P�Τl�洫�����~���
//	static bool _SQLCmdSaveSwapItem( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
//	static void _SQLCmdSaveSwapItemResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//--------------------------------------------------------------------------------------------
	//�x�s�Ыΰ򥻸��
	static bool _SQLCmdSaveHouseBase( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSaveHouseBaseResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );
	//--------------------------------------------------------------------------------------------
	//�x�s�ЫΪ��~���
	static bool _SQLCmdSaveHouseItem( vector<int>& , SqlBaseClass* sqlBase , void* UserObj , ArgTransferStruct& Arg );
	static void _SQLCmdSaveHouseItemResult ( vector<int>& , void* UserObj , ArgTransferStruct& Arg , bool ResultOK );

	static bool	_IsInitHouse_OK;
public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

	virtual void RL_BuildHouseRequest		( int ServerID , int PlayerDBID , int HouseType , const char* Account_ID );
	virtual void RL_FindHouseRequest		( int ServerID , int PlayerDBID , int HouseDBID , char* Password , int ManageLv );
	virtual void RL_FindHouseRequest_ByName ( int ServerID , int PlayerDBID , const char* RoleName , const char* Password , int ManageLv );
	virtual void RL_HouseInfoLoading		( int ServerID , int HouseDBID );
//	virtual void RL_SaveSwapBodyHouseItem	( int ServerID , int PlayerDBID , int BodyPos , ItemFieldStruct& BodyItem ,	HouseItemDBStruct& HouseItem );
	virtual void RL_SaveHouseBase			( int ServerID , int PlayerDBID , HouseBaseDBStruct& HouseBaseInfo );
	virtual void RL_SaveItem				( int ServerID , int PlayerDBID , HouseItemDBStruct& Item);
};
