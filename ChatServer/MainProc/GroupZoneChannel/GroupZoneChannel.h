#pragma		once
#pragma		warning (disable:4786)
//----------------------------------------------------------------------------------------
#include "../GroupObject/GroupObject.h"
#include "../../NetWalker_Member/NetWakerChatInc.h"
#include "roledata/RoleDataEx.h"


class GroupZoneChannel : public GroupObjectClass
{
public:
	static bool Init();
	static bool Release();
	GroupZoneChannel();
	virtual ~GroupZoneChannel();


	//virtual void OnChangeRoleInfo( BaseItemObject*  Obj );		//當角色資料有更變或換區
	
};

//----------------------------------------------------------------------------------------
