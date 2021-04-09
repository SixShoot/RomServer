#pragma once
#pragma warning (disable:4786)
#include "../Global.h"
//#include "../../netwalker_member/NetWakerGSrvInc.h"

class PartyInfoListClass 
{
	//-----------------------------------------------------------------------------------------------
	// �R�A( ���� �^
	//-----------------------------------------------------------------------------------------------
protected:
	static PartyInfoListClass *_This;
public:

	static bool Init( );
	static bool Release( );
	static PartyInfoListClass* This(){ return _This; };

	//-----------------------------------------------------------------------------------------------
	//	�ʺA
	//-----------------------------------------------------------------------------------------------
protected:
	vector< PartyInfoStruct >	_List;
	vector< int >				_UnUsed;
public:
	PartyInfoListClass();
	~PartyInfoListClass();

	void	Clear( ) { _List.clear(); }; 
	//�]�w�s��PartyInfo
	bool	AddPartyBaseInfo( PartyBaseInfoStruct& Info );
	bool	AddMember( int PartyID , PartyMemberInfoStruct& Member );
	bool	DelMember( int PartyID , PartyMemberInfoStruct& Member );
	bool	FixMember( int PartyID , PartyMemberInfoStruct& Member );
	
	PartyInfoStruct*	GetPartyInfo( int PartyID );

	int		AddPartyBaseInfo_Zone( PartyBaseInfoStruct& Info );
	bool	DelPartyID_Zone( int PartyID );
};
