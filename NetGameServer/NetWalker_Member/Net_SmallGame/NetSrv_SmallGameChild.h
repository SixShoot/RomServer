#pragma once

#include "NetSrv_SmallGame.h"

class NetSrv_SmallGameChild : public NetSrv_SmallGame
{
public:
    static bool Init();
    static bool Release();

    static bool	JoinGame         ( BaseItemObject* Obj , int Money );
    static bool	LeaveGame        ( BaseItemObject* Obj );

    virtual void JoinGameResultOK   ( char* Account );
    virtual void JoinGameResultFalse( char* Account , int Money );
    virtual void GameOver           ( char* Account , int Money );
    virtual void TrainMoney         ( char* Account , int Money );

};
