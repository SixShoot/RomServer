#pragma once
//#include "../baseitemobject/BaseItemObject.h"

#include "RoleData\RoleDataEx.h"
#include "RoleData\ObjectStruct.h"

class SchedularInfo;

struct ComboBaseInfoStruct
{
	int					ID;
	map< int , ComboBaseInfoStruct* > NextCombo;
	string				Note;
	int					Count;
	int					BonusMagicID;
	int					FinalMagicID;
	MagicComboTypeENUM	Type[5];

	ComboBaseInfoStruct()
	{
		Init();
	};
	void Init()
	{
		ID = 0;
		Count = 0;
		BonusMagicID = 0;
		FinalMagicID = 0;
		NextCombo.clear();
		memset( Type , -1 , sizeof(Type) );
	}
};



class SkillComboClass
{
	ComboBaseInfoStruct				_ComboRoot;
	vector< ComboBaseInfoStruct* >	_ComboList;

	static int      _ComboDelayProc( SchedularInfo* Obj , void* InputClass );    //延後換區
public:
	SkillComboClass();
	~SkillComboClass();
	void Init();
	void Release();

	//取得此Combo效果帶來的能量累積倍率
	float		GetPowerRate( int ComboID );

	//連攜前致處理
	void		CheckSkillCombo_PreProc( RoleDataEx* Owner , int TargetID , MagicComboTypeENUM Type );
	//檢查連攜是否成立,並處理
	//回傳連攜的結果
	void		CheckSkillCombo( RoleDataEx* Owner , int TargetID , MagicComboTypeENUM Type );

	int			CalComboPower( RoleDataEx* Target , int AddPower );

};