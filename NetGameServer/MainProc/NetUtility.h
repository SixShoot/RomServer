#pragma once

/*
#include "NetWaker/GSrvNetWaker.h"
#include <Windows.h>
#include "BaseItemObject/BaseItemObject.h"
#include "GlobalDef.h"
#include "GSrvIni/GSrvIniFile.h"
#include "GMCommand.h"
#include "GlobalBase.h"
#include "ObjectDataFile/ObjectDataBaseFile.h"
*/


#include "RoleData/RoleDataEx.h"

class CNetUtility
{
public:
	static void			OnEvent_RoleDead			( RoleDataEx* pKiller, RoleDataEx* pDeader );
		// 當物件死亡時通知
	static void			OnEvent_RoleLogin			( RoleDataEx* pRole );

	static void			OnEvent_RoleMove			( RoleDataEx* pRole );
	static void			OnEvent_RoleCast			( RoleDataEx* pRole );
	static void			OnEvent_RoleBeHit			( RoleDataEx* pRole );



};


