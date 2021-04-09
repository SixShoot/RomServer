#pragma		once
#pragma		warning (disable:4786)
//----------------------------------------------------------------------------------------
#include "../GroupObject/GroupObject.h"
#include "../../NetWalker_Member/NetWakerChatInc.h"
#include "roledata/RoleDataEx.h"


class GroupPlayerChannel : public GroupObjectClass
{
	static map< string , GroupPlayerChannel* >		_NameList;				//ÀW¹D¦WºÙ 
public:

	static GroupPlayerChannel* GetObj_ByName( string ChannelName );

protected:

	string					_ChannelName;
	string					_Password;
	bool					_JoinEnabled;
//	BaseItemObject*			_Owner;
	int						_OwnerSockID;
	string					_OwnerName;
	int						_OwnerDBID;

public:
	GroupPlayerChannel();
	virtual ~GroupPlayerChannel();

//	BaseItemObject*	Owner( );
	void			Owner( BaseItemObject* );

	const char*	Password( );
	void		Password( string Pwd );

	bool		JoinEnabled( );
	void 		JoinEnabled( bool Value );

	const char*	ChannelName( );
	bool		ChannelName( string Name );	

	virtual bool    AddUser( BaseItemObject* );
	virtual bool    DelUser( BaseItemObject* );
	virtual bool	ChangeOwner( int PlayerDBID );

	int OwnerSockID(){ return _OwnerSockID; };
	const char* OwnerName() { return _OwnerName.c_str(); };
	int	OwnerDBID(){ return _OwnerDBID; };
};

//----------------------------------------------------------------------------------------
