#include "FlagPosList.h"
#include "../Global.h"
#include "../../netwalker_member/net_dcbase/Net_DCBaseChild.h"

vector< vector < FlagPosInfo > >*    FlagPosClass::_FlagList = NULL;

void    FlagPosClass::Init()
{
    if( _FlagList != NULL )
        return;

    _FlagList = NEW( vector< vector < FlagPosInfo > > );
}
void    FlagPosClass::Release()
{
    if( _FlagList == NULL )
        return;

    delete _FlagList;
}

bool    FlagPosClass::AddFlag( BaseItemObject* FlagObj , int FlagObjID , int ID , float X , float Y , float Z , float Dir , int FlagDBID )
{
//	int ObjID = FlagObjID - Def_ObjectClass_Flag;
	int ObjID;
	if( FlagObjID >= Def_ObjectClass_Flag )
		ObjID = FlagObjID - Def_ObjectClass_Flag;
	else
		ObjID = FlagObjID;

    vector < FlagPosInfo > TempVector;
    FlagPosInfo            TempData; 
    FlagPosInfo*           Data; 

    if( (unsigned)ObjID >= 10000  )
        return false;

    if( ID != -1 &&  (unsigned)ID >= 1000 )
        return false;
    
    while( (*_FlagList).size() <= (unsigned)ObjID )
        (*_FlagList).push_back( TempVector );

    if( ID == -1 )
    {
        ID = int((*_FlagList)[ObjID].size());
    }

    while( (*_FlagList)[ObjID].size() <= (unsigned)ID )
        (*_FlagList)[ObjID].push_back( TempData );

    Data = &(*_FlagList)[ObjID][ID];
    if( Data->FlagObj != NULL )
	{
        Data->FlagObj->Destroy( "SYS AddFlag" );
	}

    if( FlagObj != NULL )
        FlagObj->Role()->SelfData.PID = ID;

	if( FlagDBID == -1 && Data->FlagDBID != -1 )
	{
		if( FlagObj )
			FlagObj->Role()->Base.DBID = Data->FlagDBID ;

		FlagDBID = Data->FlagDBID;
		if( Data->FlagObj != NULL )
			Data->FlagObj->Role()->Base.DBID = -1;
	}

    Data->FlagObj   = FlagObj;
    //Data->ObjID      = ObjID;
	Data->ObjID		= FlagObjID;
    Data->ID        = ID;
    Data->X         = X;
    Data->Y         = Y;
    Data->Z         = Z;
	Data->Dir		= Dir;
	Data->FlagDBID	= FlagDBID;

    return true;
}
bool    FlagPosClass::DelFlag( BaseItemObject* FlagObj )
{
    FlagPosInfo*           Data; 
    FlagObj->Destroy( "SYS DelFlag-1" );
    if( FlagObj->Role()->IsFlag() == false )
        return false;

    unsigned int ObjID   = FlagObj->Role()->BaseData.ItemInfo.OrgObjID - Def_ObjectClass_Flag;
    unsigned int ID      = FlagObj->Role()->SelfData.PID;

    if( (*_FlagList).size() <= (unsigned)ObjID || (*_FlagList)[ ObjID ].size() <= (unsigned)ID )
        return false;

    Data = &(*_FlagList)[ObjID][ID];

	if( Data->FlagObj != FlagObj )
		return false;

    if( Data->FlagObj != NULL )
    {
        Data->FlagObj->Destroy("SYS DelFlag-2");
        Data->Init();
    }
	//通知資料庫刪除


	while( (*_FlagList)[ObjID].size() != 0 
		&& (*_FlagList)[ObjID].back().IsEmpty() )
	{
		(*_FlagList)[ObjID].pop_back();
	}

    return true;
}
bool	FlagPosClass::DelFlag( int ObjID , int ID , const char* Account )
{
	char BufReason[512];
	sprintf( BufReason , "%s-DelFlag" , Account );

	FlagPosInfo*           Data; 

	if( Def_ObjectClass_Flag <= ObjID )
		ObjID   -= Def_ObjectClass_Flag;

    if( (*_FlagList).size() <= (unsigned)ObjID || (*_FlagList)[ ObjID ].size() <= (unsigned)ID )
        return false;

    Data = &(*_FlagList)[ObjID][ID];

	if( Data->IsEmpty() )
		return false;

	Net_DCBase::DropNPCRequest( Data->FlagDBID , BufReason );
    if( Data->FlagObj != NULL )
    {		
		Data->FlagObj->ReleaseDBID( );
		Data->FlagObj->Role()->Base.DBID = -1;
        Data->FlagObj->Destroy( BufReason );
    }
	Data->Init();

	//通知資料庫刪除
	while( (*_FlagList)[ObjID].size() != 0 
		&& (*_FlagList)[ObjID].back().IsEmpty() )
	{
		(*_FlagList)[ObjID].pop_back();
	}

    return true;
}
bool	FlagPosClass::DelFlag_Group( int ObjID , const char* Account )
{
	char BufReason[512];
	sprintf( BufReason , "%s-DelFlag_Group" , Account );

	FlagPosInfo*           Data; 

	if( Def_ObjectClass_Flag <= ObjID )
		ObjID   -= Def_ObjectClass_Flag;

    if( (*_FlagList).size() <= (unsigned)ObjID  )
        return false;

	for( unsigned int i = 0 ; i < ((*_FlagList)[ObjID]).size() ; i++ )
	{
		Data = &(*_FlagList)[ObjID][i];
		if( Data->IsEmpty() )
			continue;

		Net_DCBase::DropNPCRequest( Data->FlagDBID , BufReason );
		if( Data->FlagObj != NULL )
		{
			Data->FlagObj->Role()->Base.DBID = -1;
			Data->FlagObj->ReleaseDBID( );
			Data->FlagObj->Destroy( BufReason );
		}

	}
	((*_FlagList)[ObjID]).clear();

	return true;
}


FlagPosInfo*    FlagPosClass::GetFlag( RoleDataEx* FlagObj )
{
	//unsigned int ObjID   = FlagObj->BaseData.ItemInfo.OrgObjID - Def_ObjectClass_Flag;
	unsigned int ID      = FlagObj->SelfData.PID;


	return GetFlag( FlagObj->BaseData.ItemInfo.OrgObjID , ID );
}
void    FlagPosClass::Hide( int iFlagObjID )
{
//	int ObjID = iFlagObjID - Def_ObjectClass_Flag;
	int ObjID;
	if( iFlagObjID >= Def_ObjectClass_Flag )
		ObjID = iFlagObjID - Def_ObjectClass_Flag;
	else
		ObjID = iFlagObjID;

    FlagPosInfo*           Data;
    if( iFlagObjID == -1 )
    {
        for( unsigned int i = 0 ; i < (*_FlagList).size() ; i++ )
        {
            for( unsigned int j = 0 ; j < (*_FlagList)[i].size() ; j++ )
            {
                Data = &(*_FlagList)[i][j];
                if( Data->FlagObj != NULL )
                {               
					if( Data->FlagObj->Role()->Base.DBID == -2 )
					{
						Data->FlagObj->Role()->Base.DBID = -3;
					}
					else
					{
						Data->FlagObj->Role()->Base.DBID = -1;
						Data->FlagObj->Destroy( "SYS Hide-1" );
						Data->FlagObj = NULL;
					}
                }
            }
        }
    }
    else if( (*_FlagList).size() > ( unsigned )ObjID )
    {
        for( unsigned int j = 0 ; j < (*_FlagList)[ObjID].size() ; j++ )
        {
            Data = &(*_FlagList)[ObjID][j];
            if( Data->FlagObj != NULL )
            {               
					if( Data->FlagObj->Role()->Base.DBID == -2 )
					{
						Data->FlagObj->Role()->Base.DBID = -3;
					}
					else
					{
						Data->FlagObj->Role()->Base.DBID = -1;
						Data->FlagObj->Destroy("SYS Hide-2");
						Data->FlagObj = NULL;
					}
            }
        }
    }

}

void    FlagPosClass::Show( int iFlagObjID )
{
	int ObjID;
	if( iFlagObjID >= Def_ObjectClass_Flag )
		ObjID = iFlagObjID - Def_ObjectClass_Flag;
	else
		ObjID = iFlagObjID;
//	int ObjID= iFlagObjID - Def_ObjectClass_Flag;

    FlagPosInfo*           Data;
    if( iFlagObjID == -1 )
    {
        for( unsigned int i = 0 ; i < (*_FlagList).size() ; i++ )
        {
            for( unsigned int j = 0 ; j < (*_FlagList)[i].size() ; j++ )
            {
                Data = &(*_FlagList)[i][j];
                if( !Data->IsEmpty() && Data->FlagObj == NULL )
                {
                    Global::CreateFlag_ShowFlag( Data->ObjID , Data->ID );
                }
            }
        }
    }
    else if( (*_FlagList).size() > ( unsigned )ObjID )
    {
        for( unsigned int j = 0 ; j < (*_FlagList)[ObjID].size() ; j++ )
        {
            Data = &(*_FlagList)[ObjID][j];
            if( !Data->IsEmpty() && Data->FlagObj == NULL )
            {
                Global::CreateFlag_ShowFlag( Data->ObjID , Data->ID );
            }
        }
    }

}

FlagPosInfo* FlagPosClass::GetFlag( int iFlagObjID , int ID )
{
	int FlagObjID;
	if( iFlagObjID >= Def_ObjectClass_Flag )
		FlagObjID = iFlagObjID - Def_ObjectClass_Flag;
	else
		FlagObjID = iFlagObjID;

    if( (unsigned)FlagObjID >= _FlagList->size() || FlagObjID < 0 )
        return NULL;
    if( (*_FlagList)[ FlagObjID ].size() == 0 )
        return NULL;

    if( ID == -1 )
    {
        int TempValue = rand() % int((*_FlagList)[ FlagObjID ].size() );
        return &( (*_FlagList)[ FlagObjID ][ TempValue ] );
    }

    if( (int)( (*_FlagList)[ FlagObjID ].size() ) <= ID )
        return NULL;

    return & ( (*_FlagList)[ FlagObjID ][ID] );
}

int		FlagPosClass::Count( int FlagObjID )
{
//	FlagObjID = FlagObjID - Def_ObjectClass_Flag;
	if( FlagObjID >= Def_ObjectClass_Flag )
		FlagObjID = FlagObjID - Def_ObjectClass_Flag;


	if( (unsigned)FlagObjID >= _FlagList->size() )
		return 0;

	return (int)( (*_FlagList)[ FlagObjID ].size() );
}

vector < FlagPosInfo >* FlagPosClass::GetFlagList( int iFlagObjID )
{
//	int FlagObjID = iFlagObjID - Def_ObjectClass_Flag;
	int FlagObjID;
	if( iFlagObjID >= Def_ObjectClass_Flag )
		FlagObjID = iFlagObjID - Def_ObjectClass_Flag;
	else
		FlagObjID = iFlagObjID;

	if( FlagObjID >= (int)_FlagList->size() )
		return NULL;

	return &((*_FlagList)[ FlagObjID ]);
}


bool	FlagPosClass::DelFlagList( int iFlagObjID , const char* Account )
{
	char BufReason[512];
	sprintf( BufReason , "%s-DelFlagList" , Account );
	
	//int FlagObjID = iFlagObjID - Def_ObjectClass_Flag;
	int FlagObjID;
	if( iFlagObjID >= Def_ObjectClass_Flag )
		FlagObjID = iFlagObjID - Def_ObjectClass_Flag;
	else
		FlagObjID = iFlagObjID;

	if( (unsigned)FlagObjID >= _FlagList->size() )
		return false;

	vector < FlagPosInfo >& TempList = (*_FlagList)[ FlagObjID ];
	for( int i = 0 ; i < (int)TempList.size() ; i++ )
	{
		if( TempList[i].IsEmpty() )
			continue;
		
		if( TempList[i].FlagObj != NULL )
			TempList[i].FlagObj->Destroy( BufReason );
		else
		{
			Net_DCBase::DropNPCRequest( TempList[i].FlagDBID , BufReason );
		}
	}

	TempList.clear();
	return true;
}
