#pragma     once
#pragma		warning (disable:4786)

#include "../Global.h"
#include <vector>
#include "PG/PG_Magic.h"

using namespace std;

typedef	bool (*MagicFunction)(  RoleDataEx* , GameObjDbStructEx*  , MagicProcInfo* MagicProc );
typedef	void (*MagicBaseCheckFunction)( RoleDataEx* , int , int );
//----------------------------------------------------------------------------------------
//	GM�R�O�޲z���c
//----------------------------------------------------------------------------------------
enum	MagicHitTypeENUM
{
	EM_MAGIC_HIT_TYPE_HIT		,	//�R��
	EM_MAGIC_HIT_TYPE_MISS		,	//MISS
	EM_MAGIC_HIT_TYPE_CRITICAL	,	//�P�R�@��
	EM_MAGIC_HIT_TYPE_NO_EFFECT	,	//�L��
	EM_MAGIC_HIT_TYPE_CANCEL	,	//�������B�z
	EM_MAGIC_HIT_TYPE_DODGE		,	//Dodge
};

struct MagicTargetInfoStruct
{
	RoleDataEx*			Role;
	MagicHitTypeENUM	HitType;	
	bool				IsSpellOneMagic;	//�ܤ֬I�i�L�@���k�N (�H�@��Group�����)
	bool				IsDel;				//�w�R��
	MagicTargetInfoStruct()
	{
		Role = NULL;
		HitType = EM_MAGIC_HIT_TYPE_HIT;
		IsSpellOneMagic = false;
		IsDel = false;
	}
};
//----------------------------------------------------------------------------------------
class MagicProcessClass
{

public:
	static vector< MagicFunction >			 m_Magic;
	static vector< int >					 m_ShootTarget;
    static vector < MagicTargetInfoStruct >  m_TargetList;				//�j�M�X�Ӫ����ĥؼ�
    static vector < MagicTargetInfoStruct* >  m_ProcTargetList;			//�ثe�B�z���ĥؼ�

//	static vector < MagicTargetInfoStruct >  m_TargetListMemory;		//���t�m�X���B�z�O����

	static int								 m_ProcMagicID;				//�ثe���b�B�z���k�N
	static bool								 m_IsMissErase;				//�p�G�k�N�S�R���h�⦹�ؼв���

	static vector< MagicBaseCheckFunction >	m_CheckMagicBase;			//�ˬd�k�N�����O�_�i�I�i

	static int				m_SysMagicCount;							//�t�Ϊk�ƥثe�ƶq
	static unsigned			m_DisableSysMagicTime;						//����t�Ϊk�N�@����
	static map<int,int>		m_SysMagicIDCount;							//�I�i�U�تk�B������
	//----------------------------------------------------------------------------------------
    static void RangeSelectCircle   ( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList );
    static void RangeSelectLine1    ( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList );
	static void RangeSelectLine2    ( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList );
	static void RangeSelectLine3    ( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList );
    static void RangeSelectSector   ( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList );
    static void RangeSelectLightning1( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList );
    static void RangeSelectLightning2( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList );
	static void RangeSelectHorizontal( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList );
	static void RangeSelectBox		( RoleDataEx* Owner , MagicProcInfo* MagicProc , GameObjDbStructEx* OrgDB , vector<BaseSortStruct>*	SearchList );
	//----------------------------------------------------------------------------------------
	//��l��
	static void Init();

	//�I�k 
	static bool SpellMagic( int OwnerID , int TargetID , float TargetX , float TargetY , float TargetZ , int MagicID , int MagicLv , int ShootAngle = 0 );

	//�I�k�B��(�ǳƬI�k)
	static bool SpellProc_BeginSpell( int OwnerID , int TargetID , float TargetX , float TargetY , float TargetZ , int MagicID , int MagicLv  );
	
	//�t�άI�k
	static int  _SysSpellMagicProc( SchedularInfo* Obj , void* InputClass );   //�g�X�B�z
	static bool	SysSpellMagic ( RoleDataEx* Owner , MagicProcInfo* MagicProc , int DelayTime = 1 );
	//�I�k�B��(�}�l�I�k)
	static bool	SpellProc_OnSpell( RoleDataEx* Owner , MagicProcInfo* MagicProc = NULL );
	
	//�w�ɳB�z
	static int	Process( RoleDataEx* Owner , MagicProcInfo*	 MagicProc );
	
	//�[�J�@�Ӫk�N(�H�˳ƼҦ��[�W)
	static bool WearMagic( RoleDataEx* Owner , int MagicID );

	//�����@�Ӫk�N(�H�˳ƼҦ�����)
	static bool DropMagic( RoleDataEx* Owner , int MagicID );

	//���o�ثe���B�z���k�N
	static int  ProcMagicID();

	//�p��R���v�P�R������
	static void	CalTargetListHitType( RoleDataEx* Owner , GameObjDbStructEx* MagicCollectOrgDB , GameObjDbStructEx* MagicBaseOrgDB );


	//----------------------------------------------------------------------------------------
	static bool Magic00_AddHP				( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc );
	static bool Magic01_Assist				( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc );
	static bool Magic02_Teleport			( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc );
	static bool Magic03_CreatePet			( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc );
	static bool Magic04_SummonItem			( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc );
	static bool Magic05_Steal				( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc );
	static bool Magic06_ItemRunPlot			( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc );
	static bool Magic07_RunPlot				( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc );
	static bool Magic08_Raise				( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc );

	//----------------------------------------------------------------------------------------
//	static void	Magic00_AddHP_DelayProc( RoleDataEx* Owner , GameObjDbStructEx* MagicBaseDB , vector< MagicTargetInfoStruct* >&  ProcTargetList , vector< MagicAtkEffectInfoStruct >& List );

	//----------------------------------------------------------------------------------------

	static bool Magic_Empty	( RoleDataEx* Owner , GameObjDbStructEx* OrgDB , MagicProcInfo* MagicProc );


	//----------------------------------------------------------------------------------------
	//�O�_�i�H�I�k�ˬd
	//----------------------------------------------------------------------------------------
	static	void	Check_None				( RoleDataEx* Owner , int Arg1 , int Arg2 );				//�L
	static	void	Check_Rand				( RoleDataEx* Owner , int RandValue , int Arg2 );			//�ü�
	static	void	Check_MyEQ				( RoleDataEx* Owner , int RandValue , int EQObjID );		//�ۤv�˳�( ���v , �˳Ƹ��X )
	static	void	Check_TargetEQ			( RoleDataEx* Owner , int RandValue , int EQObjID );		//�ؼи˳�( ���v , �˳Ƹ��X )
	static	void	Check_MyEqType			( RoleDataEx* Owner , int RandValue , int Eqtype );			//�ۤv�˳�����( ���v , �˳����� )	
	static	void	Check_TargetEqType		( RoleDataEx* Owner , int RandValue , int Eqtype );			//�ؼи˳�����( ���v , �˳����� )	
	static	void	Check_MyItem			( RoleDataEx* Owner , int RandValue , int ItemObjID );		//�ۤv���~( ���v , ���~ or ���n���~ )
	static	void	Check_TargetItem		( RoleDataEx* Owner , int RandValue , int ItemObjID );		//�ؼЪ��~( ���v , ���~ or ���n���~ )
	static	void	Check_MyBuff			( RoleDataEx* Owner , int RandValue , int BuffID );			//�ۤvBuff( ���v , �֦��YBuff )
	static	void	Check_TargetBuff		( RoleDataEx* Owner , int RandValue , int BuffID );			//�ؼ�Buff( ���v , �֦��YBuff )
	static	void	Check_MyPos				( RoleDataEx* Owner , int RandValue , int PosID );			//�ۤv��m( ���v , ��m��O�X )
	static	void	Check_TargetPos			( RoleDataEx* Owner , int RandValue , int PosID );			//�ؼЦ�m( ���v , ��m��O�X )
	static	void	Check_MyAttackType		( RoleDataEx* Owner , int RandValue , int Type );			//�ۤv�԰�( ���v , 0�D�԰�/1�԰�  )
	static	void	Check_TargetAttackType	( RoleDataEx* Owner , int RandValue , int Type );			//�ؼо԰�( ���v , 0�D�԰�/1�԰�  )
	static	void	Check_TargetRace		( RoleDataEx* Owner , int RandValue , int Type );			//�ؼкر�( ���v , �ر�ID  )
	static	void	Check_MyHp				( RoleDataEx* Owner , int RandValue , int Type );			//�ۤvHP�C��( ���v , HP�ʤ��� )
	static	void	Check_Time				( RoleDataEx* Owner , int RandValue , int Time );			//�ɶ�( ���v , �ɶ� )
	static	void	Check_Weather			( RoleDataEx* Owner , int RandValue , int WeatherID );		//���( ���v , ��Կ�O�X )	
	static	void	Check_MyBuffGroup		( RoleDataEx* Owner , int RandValue , int Time );			//�ۤvBuff Group( ���v , �֦��YBuff�s�� )
	static	void	Check_TargetBuffGroup	( RoleDataEx* Owner , int RandValue , int WeatherID );		//�ؼ�Buff Group( ���v , �֦��YBuff�s�� )
	static	void	Check_MySkill			( RoleDataEx* Owner , int RandValue , int SkillID );		//�ۤv�Ƿ|�Y�ޯ�( ���v , �k�NID )
	static	void	Check_LV				( RoleDataEx* Owner , int RandValue , int Lv );				//�����ˬd
	static	void	CheckUseLuaProc( RoleDataEx* Owner , GameObjDbStructEx* MagicBaseOrgDB );

	static int      _ShootProc( SchedularInfo* Obj , void* InputClass );   //�g�X�B�z

	//�p��ؼи�T
	static void		_Cal_TargetList			( RoleDataEx* Owner , RoleDataEx* Target , GameObjDbStructEx* MagicCollectOrgDB , MagicProcInfo* MagicProc );

protected:
	//�p�⪫�z��H�ˮ`
	static MagicAtkEffectInfoStruct CalPhyDamage( RoleDataEx* Owner , MagicTargetInfoStruct* TargetInfo , float Damage , GameObjDbStructEx* MagicCollectOrgDB , GameObjDbStructEx*	MagicBaseDB , int MagicLv , float powerRate );
	//�p��k�N��H�ˮ`
	static MagicAtkEffectInfoStruct CalMagicDamage( RoleDataEx* Owner , MagicTargetInfoStruct* TargetInfo , float Damage , GameObjDbStructEx* MagicCollectOrgDB , GameObjDbStructEx*	MagicBaseDB , int MagicLv , float powerRate );

	//�I�k�M���Y�H�������
	static void		ClsHateList( RoleDataEx* Speller , RoleDataEx* Target , int AddSpellerHatePoint );
	//�I�k�M���Y�H��������ܦ��� 1 �I
	static void		ClsHateListToOnePoint( RoleDataEx* Speller , RoleDataEx* Target , int AddSpellerHatePoint );
};
