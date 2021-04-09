#include "PartyInfoList.h"

PartyInfoListClass* PartyInfoListClass::_This = NULL;

//-----------------------------------------------------------------------------------------------
bool PartyInfoListClass::Init( )
{
	if( _This != NULL  )
		return false;

	_This = NEW( PartyInfoListClass );

	return true;
}
bool PartyInfoListClass::Release( )
{
	if( _This == NULL  )
		return false;

	delete _This;

	return true;
}
//-----------------------------------------------------------------------------------------------
//	動態
//-----------------------------------------------------------------------------------------------
PartyInfoListClass::PartyInfoListClass()
{

}
PartyInfoListClass::~PartyInfoListClass()
{

}
//-----------------------------------------------------------------------------------------------
//設定新的PartyInfo
bool	PartyInfoListClass::AddPartyBaseInfo( PartyBaseInfoStruct& Info )
{
	while( _List.size() <= (unsigned)Info.PartyID )
	{
		PartyInfoStruct Temp;
		_List.push_back( Temp );
		
	}

	_List[ Info.PartyID ].Info = Info;

	vector< PartyMemberInfoStruct >& Member = _List[ Info.PartyID ].Member;

	int  LeaderID = 0;
	for( int i = 0 ; i < (int)Member.size() ; i++  )
	{
		if( strcmp( Member[i].Name.Begin() , Info.LeaderName.Begin() ) == 0  )
		{
			LeaderID = i;
			break;
		}
	}

	if( LeaderID != 0 )
	{
		Member.insert( Member.begin() , Member[ LeaderID ] );
		Member.erase( Member.begin() +  LeaderID + 1 );
		for( int i = 0 ; i < (int)Member.size() ; i++  )
		{
			Member[i].MemberID = i;
		}
	}



	Print( LV2 , "AddPartyBaseInfo" , "PartyID=%d" , Info.PartyID );
	return true;
}

int		PartyInfoListClass::AddPartyBaseInfo_Zone( PartyBaseInfoStruct& Info )
{
	Info.PartyID = 0;
	if( _UnUsed.size() == 0 )
	{
		Info.PartyID = (int)_List.size();

		PartyInfoStruct Temp;
		_List.push_back( Temp );
	}
	else
	{
		Info.PartyID = _UnUsed.back();
		_UnUsed.pop_back();
	}
	
	_List[ Info.PartyID ].Info = Info;
	_List[ Info.PartyID ].Member.clear();

	return Info.PartyID;
}

bool	PartyInfoListClass::DelPartyID_Zone( int PartyID )
{
	if( _List.size() <= (unsigned)PartyID )
		return false;

	if( _List[ PartyID ].Info.PartyID != -1 )
		return false;

	_List[ PartyID ].Info.PartyID = -1;
	_UnUsed.push_back( PartyID );

	return true;
}


bool	PartyInfoListClass::AddMember( int PartyID , PartyMemberInfoStruct& Member )
{
	PartyInfoStruct* Info = GetPartyInfo( PartyID );
	if( Info == NULL )
	{
		Print( LV2 , "AddMember" , "Info == NULL PartyID=%d" , PartyID );
		return false;
	}
	
	//找是否有重復的
	for( int i = 0 ; i < (int)Info->Member.size() ; i++ )
	{
		if( Info->Member[i].DBID == Member.DBID )
		{
			Info->Member[ i ] = Member;
			Print( LV2 , "AddMember" , "Info->Member[i].DBID == Member.DBID" );
			return false;
		}
	}

	Info->Member.push_back( Member );

	return true;
}
bool	PartyInfoListClass::DelMember( int PartyID , PartyMemberInfoStruct& Member )
{
	PartyInfoStruct* Info = GetPartyInfo( PartyID );
	if( Info == NULL )
	{
		Print( LV2 , "DelMember" , "Info == NULL PartyID=%d" , PartyID );
		return false;
	}

	for( int i = 0 ; i < (int)Info->Member.size() ; i++ )
	{
		if( Info->Member[i].DBID == Member.DBID )
		{
			Info->Member.erase( Info->Member.begin() + i );
			return true;
		}
	}
	/*
	if( (unsigned)Member.MemberID >= Info->Member.size() )
	{
		Print( LV2 , "DelMember" , "Member.MemberID > Info->Member.size()" );
		return false;
	}
	Info->Member.erase( Info->Member.begin() + Member.MemberID );
	
	for( unsigned i = 0 ; i < Info->Member.size() ; i++ )
	{
		Info->Member[i].MemberID = i;
	}
	*/
	return false;
}

bool	PartyInfoListClass::FixMember( int PartyID , PartyMemberInfoStruct& Member )
{
	PartyInfoStruct* Info = GetPartyInfo( PartyID );
	if( Info == NULL )
	{
		Print( LV2 , "FixMember" , "Info == NULL PartyID=%d",PartyID );
		return false;
	}	
	for( int i = 0 ; i < (int)Info->Member.size() ; i++ )
	{
		if( Info->Member[i].DBID == Member.DBID )
		{
			Info->Member[ i ] = Member;
			return true;
		}
	}

	return false;
}

PartyInfoStruct*	PartyInfoListClass::GetPartyInfo( int PartyID )
{
	if( _List.size() <= (unsigned)PartyID )
		return NULL;

	return &_List[ PartyID ];
}