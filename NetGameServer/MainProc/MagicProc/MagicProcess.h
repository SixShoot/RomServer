#pragma     once
#pragma		warning (disable:4786)

#include "../Global.h"
#include <vector>
#include "PG/PG_Magic.h"

using namespace std;

typedef	bool (*MagicFunction)(  RoleDataEx* , GameObjDbStructEx*  , MagicProcInfo* MagicProc );
typedef	void (*MagicBaseCheckFunction)( RoleDataEx* , int , int );
//----------------------------------------------------------------------------------------
//	GM命令管理結構
//----------------------------------------------------------------------------------------
enum	MagicHitTypeENUM
{
	EM_MAGIC_HIT_TYPE_HIT		,	//命中
	EM_MAGIC_HIT_TYPE_MISS		,	//MISS
	EM_MAGIC_HIT_TYPE_CRITICAL	,	//致命一擊
	EM_MAGIC_HIT_TYPE_NO_EFFECT	,	//無效
	EM_MAGIC_HIT_TYPE_CANCEL	,	//取消不處理
	EM_MAGIC_HIT_TYPE_DODGE		,	//Dodge
};

struct MagicTargetInfoStruct
{
	RoleDataEx*			Role;
	MagicHitTypeENUM	HitType;	
	bool				IsSpellOneMagic;	//至少施展過一次法術 (以一個Group為單位)
	bool				IsDel;				//已刪除
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
    static vector < MagicTargetInfoStruct >  m_TargetList;				//搜尋出來的有效目標
    static vector < MagicTargetInfoStruct* >  m_ProcTargetList;			//目前處理有效目標

//	static vector < MagicTargetInfoStruct >  m_TargetListMemory;		//先配置出的處理記憶體

	static int								 m_ProcMagicID;				//目前正在處理的法術
	static bool								 m_IsMissErase;				//如果法術沒命中則把此目標移除

	static vector< MagicBaseCheckFunction >	m_CheckMagicBase;			//檢查法術元素是否可施展

	static int				m_SysMagicCount;							//系統法數目前數量
	static unsigned			m_DisableSysMagicTime;						//停止系統法術一秒鐘
	static map<int,int>		m_SysMagicIDCount;							//施展各種法處的次數
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
	//初始化
	static void Init();

	//施法 
	static bool SpellMagic( int OwnerID , int TargetID , float TargetX , float TargetY , float TargetZ , int MagicID , int MagicLv , int ShootAngle = 0 );

	//施法處裡(準備施法)
	static bool SpellProc_BeginSpell( int OwnerID , int TargetID , float TargetX , float TargetY , float TargetZ , int MagicID , int MagicLv  );
	
	//系統施法
	static int  _SysSpellMagicProc( SchedularInfo* Obj , void* InputClass );   //射出處理
	static bool	SysSpellMagic ( RoleDataEx* Owner , MagicProcInfo* MagicProc , int DelayTime = 1 );
	//施法處裡(開始施法)
	static bool	SpellProc_OnSpell( RoleDataEx* Owner , MagicProcInfo* MagicProc = NULL );
	
	//定時處理
	static int	Process( RoleDataEx* Owner , MagicProcInfo*	 MagicProc );
	
	//加入一個法術(以裝備模式加上)
	static bool WearMagic( RoleDataEx* Owner , int MagicID );

	//移除一個法術(以裝備模式移除)
	static bool DropMagic( RoleDataEx* Owner , int MagicID );

	//取得目前的處理的法術
	static int  ProcMagicID();

	//計算命重率與命中類型
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
	//是否可以施法檢查
	//----------------------------------------------------------------------------------------
	static	void	Check_None				( RoleDataEx* Owner , int Arg1 , int Arg2 );				//無
	static	void	Check_Rand				( RoleDataEx* Owner , int RandValue , int Arg2 );			//亂數
	static	void	Check_MyEQ				( RoleDataEx* Owner , int RandValue , int EQObjID );		//自己裝備( 機率 , 裝備號碼 )
	static	void	Check_TargetEQ			( RoleDataEx* Owner , int RandValue , int EQObjID );		//目標裝備( 機率 , 裝備號碼 )
	static	void	Check_MyEqType			( RoleDataEx* Owner , int RandValue , int Eqtype );			//自己裝備類型( 機率 , 裝備類型 )	
	static	void	Check_TargetEqType		( RoleDataEx* Owner , int RandValue , int Eqtype );			//目標裝備類型( 機率 , 裝備類型 )	
	static	void	Check_MyItem			( RoleDataEx* Owner , int RandValue , int ItemObjID );		//自己物品( 機率 , 物品 or 重要物品 )
	static	void	Check_TargetItem		( RoleDataEx* Owner , int RandValue , int ItemObjID );		//目標物品( 機率 , 物品 or 重要物品 )
	static	void	Check_MyBuff			( RoleDataEx* Owner , int RandValue , int BuffID );			//自己Buff( 機率 , 擁有某Buff )
	static	void	Check_TargetBuff		( RoleDataEx* Owner , int RandValue , int BuffID );			//目標Buff( 機率 , 擁有某Buff )
	static	void	Check_MyPos				( RoleDataEx* Owner , int RandValue , int PosID );			//自己位置( 機率 , 位置辨別碼 )
	static	void	Check_TargetPos			( RoleDataEx* Owner , int RandValue , int PosID );			//目標位置( 機率 , 位置辨別碼 )
	static	void	Check_MyAttackType		( RoleDataEx* Owner , int RandValue , int Type );			//自己戰鬥( 機率 , 0非戰鬥/1戰鬥  )
	static	void	Check_TargetAttackType	( RoleDataEx* Owner , int RandValue , int Type );			//目標戰鬥( 機率 , 0非戰鬥/1戰鬥  )
	static	void	Check_TargetRace		( RoleDataEx* Owner , int RandValue , int Type );			//目標種族( 機率 , 種族ID  )
	static	void	Check_MyHp				( RoleDataEx* Owner , int RandValue , int Type );			//自己HP低於( 機率 , HP百分比 )
	static	void	Check_Time				( RoleDataEx* Owner , int RandValue , int Time );			//時間( 機率 , 時間 )
	static	void	Check_Weather			( RoleDataEx* Owner , int RandValue , int WeatherID );		//氣候( 機率 , 氣候辨別碼 )	
	static	void	Check_MyBuffGroup		( RoleDataEx* Owner , int RandValue , int Time );			//自己Buff Group( 機率 , 擁有某Buff群組 )
	static	void	Check_TargetBuffGroup	( RoleDataEx* Owner , int RandValue , int WeatherID );		//目標Buff Group( 機率 , 擁有某Buff群組 )
	static	void	Check_MySkill			( RoleDataEx* Owner , int RandValue , int SkillID );		//自己學會某技能( 機率 , 法術ID )
	static	void	Check_LV				( RoleDataEx* Owner , int RandValue , int Lv );				//等級檢查
	static	void	CheckUseLuaProc( RoleDataEx* Owner , GameObjDbStructEx* MagicBaseOrgDB );

	static int      _ShootProc( SchedularInfo* Obj , void* InputClass );   //射出處理

	//計算目標資訊
	static void		_Cal_TargetList			( RoleDataEx* Owner , RoleDataEx* Target , GameObjDbStructEx* MagicCollectOrgDB , MagicProcInfo* MagicProc );

protected:
	//計算物理單人傷害
	static MagicAtkEffectInfoStruct CalPhyDamage( RoleDataEx* Owner , MagicTargetInfoStruct* TargetInfo , float Damage , GameObjDbStructEx* MagicCollectOrgDB , GameObjDbStructEx*	MagicBaseDB , int MagicLv , float powerRate );
	//計算法術單人傷害
	static MagicAtkEffectInfoStruct CalMagicDamage( RoleDataEx* Owner , MagicTargetInfoStruct* TargetInfo , float Damage , GameObjDbStructEx* MagicCollectOrgDB , GameObjDbStructEx*	MagicBaseDB , int MagicLv , float powerRate );

	//施法清除某人的仇恨值
	static void		ClsHateList( RoleDataEx* Speller , RoleDataEx* Target , int AddSpellerHatePoint );
	//施法清除某人的仇恨值變成為 1 點
	static void		ClsHateListToOnePoint( RoleDataEx* Speller , RoleDataEx* Target , int AddSpellerHatePoint );
};
