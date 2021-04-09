#include "AccountSrvDef.h"
namespace   NetWakerAccountPackage
{
	/*
    //--------------------------------------------------------------------------------------------------------------
    //      ****** PlayerAccountDBStruct ******
    //--------------------------------------------------------------------------------------------------------------
    ReaderClass<PlayerAccountDBStruct>* PlayerAccountDBStruct::Reader( )
    {
        static  ReaderClass<PlayerAccountDBStruct> Reader;
        static  bool    IsReady = false;
        if( IsReady == false )
        {   
            PlayerAccountDBStruct* Pt = NULL;
            Reader.SetData( "GameID"        	, &Pt->GameID			, ENUM_Type_IntPoint      );
            Reader.SetData( "Account_ID"    	, Pt->Account_ID		, ENUM_Type_CharString     , sizeof( Pt->Account_ID )	);
            Reader.SetData( "Password"      	, Pt->Password			, ENUM_Type_CharString     , sizeof( Pt->Password )		);

			Reader.SetData( "IsMd5Password"     , &Pt->IsMd5Password	, ENUM_Type_BitPoint       	);
			Reader.SetData( "IsAutoConvertMd5"  , &Pt->IsAutoConvertMd5	, ENUM_Type_BitPoint       	);
            Reader.SetData( "ChargType"     	, &Pt->ChargType		, ENUM_Type_IntPoint		);
            Reader.SetData( "Point"         	, &Pt->Point			, ENUM_Type_IntPoint		);
            Reader.SetData( "AccountState"  	, &Pt->AccountState		, ENUM_Type_IntPoint		);
			Reader.SetData( "FreezeType"  		, &Pt->FreezeType		, ENUM_Type_IntPoint		, 0 , ENUM_Indirect_Address , Def_Disabled_Update_Flag );
			Reader.SetData( "LastLoginGameID"	, &Pt->LastLoginGameID	, ENUM_Type_IntPoint		);
			Reader.SetData( "PlayTimeQuota"		, &Pt->PlayTimeQuota	, ENUM_Type_IntPoint		);
			Reader.SetData( "LastLogoutTime"	, &Pt->LastLogoutTime	, ENUM_Type_IntPoint		);
			Reader.SetData( "IsInfant"			, &Pt->IsInfant			, ENUM_Type_BitPoint		);

            IsReady = true;
        }      
        return &Reader;
    }
    //--------------------------------------------------------------------------------------------------------------
    //      ****** PlayerLoginDBStruct ******
    //--------------------------------------------------------------------------------------------------------------
    ReaderClass<PlayerLoginDBStruct>* PlayerLoginDBStruct::Reader( )
    {
        static  ReaderClass<PlayerLoginDBStruct> Reader;
        static  bool    IsReady = false;
        if( IsReady == false )
        {   
            PlayerLoginDBStruct* Pt = NULL;
            Reader.SetData( "Account_ID"    , Pt->Account_ID    , ENUM_Type_CharString     , sizeof( Pt->Account_ID ));
            Reader.SetData( "IP"            , Pt->IP            , ENUM_Type_CharString     , sizeof( Pt->IP ));
            Reader.SetData( "LoginResult"   , &Pt->LoginResult  , ENUM_Type_IntPoint       );
            IsReady = true;
        }      
        return &Reader;
    }
	*/
    //--------------------------------------------------------------------------------------------------------------
    //      ****** PlayerAccountInfo ******
    //--------------------------------------------------------------------------------------------------------------
    PlayerAccountInfo::PlayerAccountInfo( )
    {
        Init();
    }
    //--------------------------------------------------------------------------------------------------------------
    void PlayerAccountInfo::Init()
    {
        memset( this , 0 , sizeof(*this) );
    };
    //--------------------------------------------------------------------------------------------------------------
    //      ****** GameSrvInfo ******
    //--------------------------------------------------------------------------------------------------------------
    GameSrvInfo::GameSrvInfo( )
    {
        Init();
    }
    //--------------------------------------------------------------------------------------------------------------
    void GameSrvInfo::Init( )
    {
        SockID = -1;
        GameID = -1;
        Name[0] = 0;
        PlayerInfo.clear( );
    }
    //--------------------------------------------------------------------------------------------------------------
    bool GameSrvInfo::IsReady() 
    { 
        return GameID != -1; 
    }
    //--------------------------------------------------------------------------------------------------------------
}