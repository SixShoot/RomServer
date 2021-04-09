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

	static int      _ComboDelayProc( SchedularInfo* Obj , void* InputClass );    //���ᴫ��
public:
	SkillComboClass();
	~SkillComboClass();
	void Init();
	void Release();

	//���o��Combo�ĪG�a�Ӫ���q�ֿn���v
	float		GetPowerRate( int ComboID );

	//�s��e�P�B�z
	void		CheckSkillCombo_PreProc( RoleDataEx* Owner , int TargetID , MagicComboTypeENUM Type );
	//�ˬd�s��O�_����,�óB�z
	//�^�ǳs�⪺���G
	void		CheckSkillCombo( RoleDataEx* Owner , int TargetID , MagicComboTypeENUM Type );

	int			CalComboPower( RoleDataEx* Target , int AddPower );

};