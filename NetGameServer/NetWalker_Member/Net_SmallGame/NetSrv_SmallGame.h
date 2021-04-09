#pragma once

#include "PG_SmallGame.h"
#include "../../MainProc/Global.h"
#include "../../MainProc/BaseItemObject/BaseItemObject.h"

class NetSrv_SmallGame : public Global
{
protected:
    //-------------------------------------------------------------------
    static NetSrv_SmallGame* This;
    static bool _Init();
    static bool _Release();
   
    static void	_PG_SmallGame_G1toL_JoinGameResult   ( int NetID , int iSize , void* pData );
    static void	_PG_SmallGame_G1toL_GameOver         ( int NetID , int iSize , void* pData );
    static void	_PG_SmallGame_G1toL_TrainMoney       ( int NetID , int iSize , void* pData );
public:

    static void	JoinGamePG       ( char* Account , int Money , int SockID );
    static void	LeaveGamePG      ( char* Account );
    static void	TrainMoneyOK     ( char* Account , int Money );
    static void	TrainMoneyFalse  ( char* Account , int Money );


    virtual void JoinGameResultOK   ( char* Account ) = 0;
    virtual void JoinGameResultFalse( char* Account , int Money ) = 0;
    virtual void GameOver           ( char* Account , int Money ) = 0;
    virtual void TrainMoney         ( char* Account , int Money ) = 0;

    
};

