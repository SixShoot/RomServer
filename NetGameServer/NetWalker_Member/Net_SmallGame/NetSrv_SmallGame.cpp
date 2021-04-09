#include "NetSrv_SmallGame.h"
#include "../../MainProc/Global.h"
#include "../NetWakerGSrvInc.h"
//-------------------------------------------------------------------
NetSrv_SmallGame*    NetSrv_SmallGame::This         = NULL;

//-------------------------------------------------------------------
bool NetSrv_SmallGame::_Init()
{
    _Net->RegOnSrvPacketFunction			( EM_PG_SmallGame_G1toL_JoinGameResult,		_PG_SmallGame_G1toL_JoinGameResult		);
    _Net->RegOnSrvPacketFunction			( EM_PG_SmallGame_G1toL_GameOver      ,		_PG_SmallGame_G1toL_GameOver      		);
    _Net->RegOnSrvPacketFunction			( EM_PG_SmallGame_G1toL_TrainMoney    ,		_PG_SmallGame_G1toL_TrainMoney    		);
    return false;
}
//-------------------------------------------------------------------
bool NetSrv_SmallGame::_Release()
{
    return false;
}
//-------------------------------------------------------------------
void NetSrv_SmallGame::_PG_SmallGame_G1toL_JoinGameResult   ( int NetID , int Size , void* Data )
{
    PG_SmallGame_G1toL_JoinGameResult* PG = (PG_SmallGame_G1toL_JoinGameResult*)Data;
    if( PG->Result == true )
    {
        This->JoinGameResultOK( PG->Account_ID );
    }
    else
    {
        This->JoinGameResultFalse( PG->Account_ID , PG->Money );
    }
}

void NetSrv_SmallGame::_PG_SmallGame_G1toL_GameOver         ( int NetID , int Size , void* Data )
{
    PG_SmallGame_G1toL_GameOver* PG = (PG_SmallGame_G1toL_GameOver*)Data;

    This->GameOver( PG->Account_ID , PG->Money );

}
void NetSrv_SmallGame::_PG_SmallGame_G1toL_TrainMoney       ( int NetID , int Size , void* Data )
{
    PG_SmallGame_G1toL_TrainMoney* PG = (PG_SmallGame_G1toL_TrainMoney*)Data;
    This->TrainMoney( PG->Account_ID , PG->Money );
}
//-------------------------------------------------------------------

void NetSrv_SmallGame::JoinGamePG         ( char* Account , int Money , int SockID )
{
    PG_SmallGame_LtoG1_JoinGame Send;
    memcpy( Send.Account_ID , Account , sizeof( Send.Account_ID ) );
    Send.Money = Money;
    SendToSmallGame( sizeof( Send ) , &Send );

    int SrvSockID = CNet_ServerList_Child::SrvTypeToSrvID( EM_SERVER_TYPE_SMALLGAME );
    _Net->ConnectCliAndSrv( SrvSockID , SockID );

}

void NetSrv_SmallGame::LeaveGamePG        ( char* Account )
{
    PG_SmallGame_LtoG1_LeaveGame Send;
    memcpy( Send.Account_ID , Account , sizeof( Send.Account_ID ) );
    SendToSmallGame( sizeof( Send ) , &Send );
}

void NetSrv_SmallGame::TrainMoneyOK    ( char* Account , int Money )
{
    PG_SmallGame_LtoG1_TrainMoneyResult Send;
    memcpy( Send.Account_ID , Account , sizeof( Send.Account_ID ) );
    Send.Money = Money;
    Send.Result = true;

    SendToSmallGame( sizeof( Send ) , &Send );
}

void NetSrv_SmallGame::TrainMoneyFalse ( char* Account , int Money )
{
    PG_SmallGame_LtoG1_TrainMoneyResult Send;
    memcpy( Send.Account_ID , Account , sizeof( Send.Account_ID ) );
    Send.Money = Money;
    Send.Result = false;

    SendToSmallGame( sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
/*
void NetSrv_SmallGame::_PG_SmallGame_G1toL_CreateRoomResult ( int NetID , int Size , void* Data )
{
    PG_SmallGame_G1toL_CreateRoomResult* PG = (PG_SmallGame_G1toL_CreateRoomResult*)Data;
    if( PG->Result == true )
    {
        This->CreateRoomOK( PG->Account_ID , PG->RomeID , PG->Money );
    }
    else
    {
        This->CreateRoomFalse( PG->Account_ID , PG->RomeID );
    }

}
void NetSrv_SmallGame::_PG_SmallGame_G1toL_AddPlayerResult  ( int NetID , int Size , void* Data )
{
    PG_SmallGame_G1toL_AddPlayerResult* PG = (PG_SmallGame_G1toL_AddPlayerResult*)Data;

    if( PG->Result == true )
    {
        This->AddPlayerOK( PG->Account_ID , PG->RomeID , PG->Money );
    }
    else
    {
        This->AddPlayerFalse( PG->Account_ID , PG->RomeID );
    }
}
void NetSrv_SmallGame::_PG_SmallGame_G1toL_GameOver            ( int NetID , int Size , void* Data )
{
    PG_SmallGame_G1toL_GameOver* PG = (PG_SmallGame_G1toL_GameOver*)Data;
    This->GameOver( PG->Account_ID , PG->Money );
}
//-------------------------------------------------------------------
void NetSrv_SmallGame::CreateRoom( RoleDataEx* Player )
{
    PG_SmallGame_LtoG1_CreateRoom Send;
    strncpy( Send.Account_ID , Player->Base.Account_ID , sizeof(Send.Account_ID) );
    Send.DBID = Player->Base.DBID;
    Send.SockID = Player->TempData.Sys.SockID;

    SendToSmallGame( sizeof( Send ) , &Send );

    int SrvSockID = CNet_ServerList_Child::SrvTypeToSrvID( EM_SERVER_TYPE_SMALLGAME );
    _Net->ConnectCliAndSrv( SrvSockID , Player->TempData.Sys.SockID );
}
//-------------------------------------------------------------------
void NetSrv_SmallGame::AddPlayer( RoleDataEx* Player , int RoomID )
{
    PG_SmallGame_LtoG1_AddPlayer Send;
    strncpy( Send.Account_ID , Player->Base.Account_ID , sizeof(Send.Account_ID) );
    Send.RoomID = RoomID;
    Send.DBID = Player->Base.DBID;
    Send.SockID = Player->TempData.Sys.SockID;

    SendToSmallGame( sizeof( Send ) , &Send );

    int SrvSockID = CNet_ServerList_Child::SrvTypeToSrvID( EM_SERVER_TYPE_SMALLGAME );
    _Net->ConnectCliAndSrv( SrvSockID , Player->TempData.Sys.SockID );

}
//-------------------------------------------------------------------
void NetSrv_SmallGame::LeaveRoom( char* Account_ID )
{
    PG_SmallGame_LtoG1_LeaveGame Send;
    strncpy( Send.Account_ID , Account_ID , sizeof(Send.Account_ID) );
    
    SendToSmallGame( sizeof( Send ) , &Send );
}
//-------------------------------------------------------------------
*/