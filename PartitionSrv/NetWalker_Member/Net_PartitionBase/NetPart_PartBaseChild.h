#pragma once

#include "NetPart_PartBase.h"

//--------------------------------------------------------------------------------------------
class CNetPart_PartBaseChild : public CNetPart_PartBase
{

public:
//--------------------------------------------------------------------------------------------
	static bool Init();
	static bool Release();

    virtual void SetLocal               ( int SrvNetID , int ZoneID , char* MapFileName , int MapID );    
    virtual void AddObj                 ( int GItemID , char* RoleName , int ParallelID , float X , float Y , float Z , float Dir );    
    virtual void MoveObj                ( int GItemID , float X , float Y , float Z , float Dir );    
    virtual void DelObj                 ( int GItemID );   
    virtual void DataTransfer           ( int GItemID , int DataSize , void* Data  );    
    virtual void OnLocalSrvDisconnect   ( int dwServerLocalID );
//--------------------------------------------------------------------------------------------
};
