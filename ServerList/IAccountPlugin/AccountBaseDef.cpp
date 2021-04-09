#pragma once
#include "AccountBaseDef.h"

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
		Reader.SetData( "GameID"			, &Pt->GameID			, ENUM_Type_IntPoint      );
		Reader.SetData( "Account_ID"		, Pt->Account_ID		, ENUM_Type_CharString     , sizeof( Pt->Account_ID )	);
		Reader.SetData( "Password"			, Pt->Password			, ENUM_Type_CharString     , sizeof( Pt->Password )		);
		Reader.SetData( "AccountState"		, &Pt->AccountState		, ENUM_Type_IntPoint      );
		Reader.SetData( "FreezeType"  		, &Pt->FreezeType		, ENUM_Type_IntPoint		, 0 , ENUM_Indirect_Address , Def_Disabled_Update_Flag );
		Reader.SetData( "LastLoginGameID"   , &Pt->LastLoginGameID  , ENUM_Type_IntPoint      );
		Reader.SetData( "IsMd5Password"     , &Pt->IsMd5Password	, ENUM_Type_BitPoint      );
		Reader.SetData( "IsAutoConvertMd5"  , &Pt->IsAutoConvertMd5	, ENUM_Type_BitPoint      );
		Reader.SetData( "UserState"			, &Pt->UserState		, ENUM_Type_IntPoint      );
		Reader.SetData( "ValidTime"			, &Pt->ValidTime		, ENUM_Type_SmallDateTime	);
		Reader.SetData( "Age"				, &Pt->Age				, ENUM_Type_SmallDateTime	);
		Reader.SetData( "VivoxLicenseTime"	, &Pt->VivoxLicenseTime	, ENUM_Type_SmallDateTime	);
		Reader.SetData( "ResetPassword"		, &Pt->ResetPassword	, ENUM_Type_BitPoint		);
		Reader.SetData( "FreezeUntil"		, &Pt->FreezeUntil		, ENUM_Type_SmallDateTime	);

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