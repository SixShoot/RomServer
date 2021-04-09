#include "../NetWakerGSrvInc.h"
#include "NetSrv_SmallGameChild.h"
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool    NetSrv_SmallGameChild::Init()
{
    NetSrv_SmallGame::_Init();

    if( This != NULL)
        return false;

    This = NEW( NetSrv_SmallGameChild );

    return true;
}
//-----------------------------------------------------------------
bool    NetSrv_SmallGameChild::Release()
{
    if( This == NULL )
        return false;

    delete This;
    This = NULL;

    NetSrv_SmallGame::_Release();
    return true;
    
}
//-----------------------------------------------------------------
bool NetSrv_SmallGameChild::JoinGame  ( BaseItemObject* Obj , int Money  )
{
    if( Obj->Role()->TempData.SmallGame.GameType != EM_SmallGameType_None )
    {
        Print( LV1 , "JoinGame GameType != EM_SmallGameType_None ���Х[�J");
        return false;
    }

    Obj->Role()->TempData.SmallGame.GameType = EM_SmallGameType_WaitJoint;
    JoinGamePG( Obj->Role()->Base.Account_ID , Money , Obj->Role()->TempData.Sys.SockID );
    return true;
}

bool NetSrv_SmallGameChild::LeaveGame ( BaseItemObject* Obj )
{
    if( Obj->Role()->TempData.SmallGame.GameType != EM_SmallGameType_Big2 )
    {
        Print( LV1 , "JoinGame GameType != EM_SmallGameType_Big2 �S�[�J�C��");
        return false;
    }
    LeaveGamePG( Obj->Role()->Base.Account_ID );
    return false;
}
//-----------------------------------------------------------------
void NetSrv_SmallGameChild::JoinGameResultOK   ( char* Account )
{
    BaseItemObject* Obj = BaseItemObject::GetObjByAccountName( Account );
    if( Obj == NULL )
    {
        Print( LV3 , "CreateRoomOK Obj == NULL �䤣�쪫��" );
        LeaveGamePG( Account );
        return;
    }

    SmallGameInfoStruct* GameInfo = &(Obj->Role()->TempData.SmallGame);    
    if( GameInfo->GameType != EM_SmallGameType_WaitJoint )
    {
        Print( LV3 , "JoinGameResultOK GameInfo->GameType != EM_SmallGameType_WaitJoint ���A�����D" );
        LeaveGamePG( Account );
        return;
    }

    GameInfo->GameType = EM_SmallGameType_Big2;
}

void NetSrv_SmallGameChild::JoinGameResultFalse( char* Account , int Money )
{
    BaseItemObject* Obj = BaseItemObject::GetObjByAccountName( Account );
    if( Obj == NULL )
    {
        Print( LV3 , "JoinGameResultFalse Obj == NULL �䤣�쪫��" );
        return;
    }

    SmallGameInfoStruct* GameInfo = &(Obj->Role()->TempData.SmallGame);    
    if( GameInfo->GameType != EM_SmallGameType_WaitJoint )
    {
        Print( LV3 , "CreateRoomOK GameInfo->GameType != EM_SmallGameType_WaitJoint ���A�����D" );
    }

    GameInfo->GameType = EM_SmallGameType_None;

}
void NetSrv_SmallGameChild::GameOver           ( char* Account , int Money )
{
    BaseItemObject* Obj = BaseItemObject::GetObjByAccountName( Account );
    if( Obj == NULL )
    {
        Print( LV3 , "GameOver Obj == NULL �䤣�쪫�� Money = %d" , Money );
        return;
    }

    SmallGameInfoStruct* GameInfo = &(Obj->Role()->TempData.SmallGame);
    
    if( GameInfo->GameType != EM_SmallGameType_Big2 ) 
    {
        Print( LV3 , "GameOver OGameInfo->GameType != EM_SmallGameType_Big2  ���A�����D Money = %d" , Money );
    }

    Obj->Role()->BaseData.Money += Money;
    GameInfo->GameType = EM_SmallGameType_None;
}

void NetSrv_SmallGameChild::TrainMoney         ( char* Account , int Money )
{
    BaseItemObject* Obj = BaseItemObject::GetObjByAccountName( Account );
    if( Obj == NULL )
    {
        Print( LV3 , "TrainMoney Obj == NULL �䤣�쪫�� "  );
        LeaveGamePG( Account );
        return;
    }

    if( Money > Obj->Role()->BaseData.Money )
    {
        Print( LV1 , " TrainMoney �������� ���W�� = %d , �n�D����� = %d" , Obj->Role()->BaseData.Money, Money );
        TrainMoneyFalse( Account , Money );
        return;
    }
    Obj->Role()->BaseData.Money += Money;
    TrainMoneyOK( Account , Money );
}
//-----------------------------------------------------------------
/*
void NetSrv_SmallGameChild::CreateRoomOK    ( char* Account_ID , int RoomID , int Money )
{
    BaseItemObject* Obj = BaseItemObject::GetObjByAccountName( Account_ID );
    if( Obj == NULL )
    {
        Print( LV3 , "CreateRoomOK Obj == NULL �䤣�쪫��" )
        LeaveRoom( Account_ID );
        return;
    }
    
    SmallGameInfoStruct* GameInfo = &(Obj->Role()->TempData.SmallGame);
    if( GameInfo->Ready == true )
    {
        Print( LV3 , "CreateRoomOK GameInfo->Ready == true �����Х[�J�C��" )
        LeaveRoom( Account_ID );
        return;
    }
    GameInfo->Ready = true;
    GameInfo->TradeMoney = Money;
    
}
//-----------------------------------------------------------------
void NetSrv_SmallGameChild::CreateRoomFalse ( char* Account_ID , int RoomID )
{
    
}
//-----------------------------------------------------------------
void NetSrv_SmallGameChild::AddPlayerOK     ( char* Account_ID , int RoomID , int Money )
{
    BaseItemObject* Obj = BaseItemObject::GetObjByAccountName( Account_ID );
    if( Obj == NULL )
    {
        Print( LV3 , "CreateRoomOK Obj == NULL �䤣�쪫��" )
            LeaveRoom( Account_ID );
        return;
    }

    SmallGameInfoStruct* GameInfo = &(Obj->Role()->TempData.SmallGame);
    if( GameInfo->Ready == true )
    {
        Print( LV3 , "CreateRoomOK GameInfo->Ready == true �����Х[�J�C��" )
            LeaveRoom( Account_ID );
        return;
    }
    GameInfo->Ready = true;
    GameInfo->TradeMoney = Money;

}
//-----------------------------------------------------------------
void NetSrv_SmallGameChild::AddPlayerFalse  ( char* Account_ID , int RoomID )
{

}
//-----------------------------------------------------------------
void NetSrv_SmallGameChild::GameOver        ( char* Account_ID , int Money )
{

}
//-----------------------------------------------------------------
*/