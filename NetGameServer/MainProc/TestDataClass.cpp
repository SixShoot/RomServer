#pragma	warning (disable:4786)
#pragma warning (disable:4949)
#include "TestDataClass.h"

#pragma unmanaged

namespace NS_TestData
{
    void Init( ObjectDataFileClass<GameObjDbStructEx,RuStreamFile>* ObjectDB )
    {
		//GameObjDbStructEx* pObj;

		/*
        //-------------------------------------------------------------------------------------
        static GameObjDbStructEx PlayerTest[2];
		// 男素體
        PlayerTest[0].GUID = Def_ObjectClass_Player;
        PlayerTest[0].Classification = EM_ObjectClass_Player;
//		strcpy( PlayerTest[0].ACTWorld, "model\\character\\pc\\human\\male\\act_hm_male.ros" );
        PlayerTest[0].ImageID = 570000;
		ObjectDB->SetData( &PlayerTest[0] );
				
		// 女素體 
		PlayerTest[1].GUID = Def_ObjectClass_Player + 1;
		PlayerTest[1].Classification = EM_ObjectClass_Player;
		//strcpy( PlayerTest[1].ACTWorld, "model\\character\\pc\\human\\male\\act_hm_male.ros" );
        PlayerTest[1].ImageID = 570000;
		ObjectDB->SetData( &PlayerTest[1] );

		static GameObjDbStructEx RoleBodyTest[10];
		// 臉型
		RoleBodyTest[0].GUID = Def_ObjectClass_BodyObj;
		RoleBodyTest[0].Classification = EM_ObjectClass_BodyObj;
		//strcpy( RoleBodyTest[0].ACTWorld, "young01" );
        RoleBodyTest[0].ImageID = 570012;
		ObjectDB->SetData( &RoleBodyTest[0] );

		// 髮型
		RoleBodyTest[1].GUID = Def_ObjectClass_BodyObj + 1;
		RoleBodyTest[1].Classification = EM_ObjectClass_BodyObj;
		//strcpy( RoleBodyTest[1].ACTWorld, "orange" );
        RoleBodyTest[1].ImageID = 570011;
		ObjectDB->SetData( &RoleBodyTest[1] );
		*/

		/*
		pObj = ObjectDB->GetObj( Def_ObjectClass_Player );
		if ( pObj )
			pObj->GUID = Def_ObjectClass_Player;
		*/
		
		/*
		// 翅膀
		pObj = ObjectDB->GetObj( 221578 );
		if ( pObj ) {
			pObj->ImageObj->ImageType = EM_GameObjectImageType_Wing;
		}

		// 旗幟
		pObj = ObjectDB->GetObj( 220977 );
		if ( pObj ) {
			pObj->ImageObj->ImageType = EM_GameObjectImageType_Flag;
			strcpy(pObj->ImageObj->ACTWorld, "model/item/guild_standard/act_guild_standard.ros");
			strcpy(pObj->ImageObj->Flag.Type, "002");
			strcpy(pObj->ImageObj->Flag.Insignia, "003");
			strcpy(pObj->ImageObj->Flag.Banner, "003");
			pObj->ImageObj->Flag.InsigniaColor[0] = 0xFFFF0000;
			pObj->ImageObj->Flag.InsigniaColor[1] = 0xFFFFFF00;
			pObj->ImageObj->Flag.BannerColor[0] = 0xFF000000;
			pObj->ImageObj->Flag.BannerColor[1] = 0xFF0000FF;
		}
		*/

		// --------------------------------------------------------------------
        /*
		// Effect
		// 測試物品1(攻擊法術)
		Obj = ObjectDB->GetObject( 500000 );
        Obj->Magic.MagicFunc = EM_MAGICFUN_HPMP;
        Obj->Magic.MagicorPhy = EM_Magic_Magic;

        Obj->Magic.EffectRange      = 200;
        Obj->Magic.AttackDistance   = 300;
        Obj->Magic.SpellTime        = 2;
        Obj->Magic.HateCost         = 0;

        Obj->Magic.RangeType        = EM_MagicEffect_Target;
        Obj->Magic.TargetType       = EM_Releation_Enemy;      
        Obj->Magic.WeaponType       = EM_Weapon_None;
        Obj->Magic.EffectCount      = 3;

        memset( &Obj->Magic.Attack , 0 , sizeof(Obj->Magic.Attack) );
        Obj->Magic.Attack.HPRate[0] = -100;
        Obj->Magic.Attack.HPType[0] = EM_MagicAttackCal_None;
        Obj->Magic.Attack.HPRate[1] = -10;
        Obj->Magic.Attack.HPType[1] = EM_MagicAttackCal_INT;

        Obj->Magic.HitRate          = 1.0f;
        Obj->Magic.DLv_HitRate      = 0;
        Obj->Magic.Attack.VampireRate   = 0.5f;    //吸收比
        Obj->Magic.Attack.Rand          = 0.3f;     //傷害 變化範圍
        //-----------------------------------------------------------------
        Obj = ObjectDB->GetObject( 500001 );
        Obj->Magic.MagicFunc = EM_MAGICFUN_Assist;
        Obj->Magic.MagicorPhy = EM_Magic_Magic;

        Obj->Magic.EffectRange      = 200;
        Obj->Magic.AttackDistance   = 300;
        Obj->Magic.SpellTime        = 2;
        Obj->Magic.HateCost         = 0;

        Obj->Magic.RangeType        = EM_MagicEffect_Target;
        Obj->Magic.TargetType       = EM_Releation_Enemy;      
        Obj->Magic.WeaponType       = EM_Weapon_None;
        Obj->Magic.EffectCount      = 3;
        Obj->Magic.HitRate          = 1.0f;
        Obj->Magic.DLv_HitRate      = 0;

        
        Obj->Magic.AssistType       = EM_Magic_Type_NULL;   //法術類型(主要用於輔助法術互斥處理)
        Obj->Magic.EffectLV         = 1;	                //輔助法術威力等級
        Obj->Magic.Effect._Value    = 0;		    //影響效果
        Obj->Magic.ClearTime._Value = 0;      //消除條件
        Obj->Magic.Ability.DMG      = 100;	    //影響屬性

        Obj->Magic.DotMagic.Base    = -100;
        Obj->Magic.DotMagic.Type    = 0;
        Obj->Magic.DotMagic.Time    = 1;

        Obj->Magic.EffectTime     = 10;

		// 小輝測劇情
		//-----------------------------------------------------------------]
        Obj = ObjectDB->GetObject( 200002 );
        Obj->Item.IncMagic_Onuse = 500001;
*/
/*
		// 做出 Quest NPC 

		strcpy( PlayerTest[0].Name , "該死的大叔" );
		PlayerTest[0].GUID			= Def_ObjectClass_NPC + 100;
		PlayerTest[0].Mode.Fight	= 0;
		//PlayerTest[0].Classification = EM_ObjectClass_Player;
		//strcpy( PlayerTest[0].ACTWorld, "model\\character\\pc\\human\\male\\act_hm_male.ros" );
		ObjectDB->SetData( &PlayerTest[0] );
	
		GameObjDbStruct	QuestCollectID[1];

		QuestCollectID[0].GUID							= Def_ObjectClass_QuestCollect + 1;
		QuestCollectID[0].QuestCollect.iQuestCollectID	= 1;
		strcpy( QuestCollectID[0].QuestCollect.szQuestNpcText , "年輕人!! 可以幫大叔的忙嗎?" );

		ZeroMemory( QuestCollectID[0].QuestCollect.iQuestID, sizeof( QuestCollectID[0].QuestCollect.iQuestID ) );
		QuestCollectID[0].QuestCollect.iQuestID[0]		= 1;


		ObjectDB->SetData( &( QuestCollectID[0] ) );

		GameObjDbStruct	QuestDetail[1];

		QuestDetail[0].GUID								= Def_ObjectClass_QuestDetail + 1;
		QuestDetail[0].QuestDetail.iQuestID				= 1;

		strcpy( QuestDetail[0].QuestDetail.szQuest_Name,	"該死的豬" );
		strcpy( QuestDetail[0].QuestDetail.szQuest_Desc,	"殺了那隻豬, 然後回來向我回報." );
		strcpy( QuestDetail[0].QuestDetail.szQuest_Accept_Detail,	"那天我到農地工作, 當我回來時, 發現那隻該死的豬, 竟然偷偷跑進我的豬舍. 推倒我家的母豬. 不管!! 不管!! 殺了牠後向我回報." );

		QuestDetail[0].QuestDetail.iReward_Exp			= 50;
		QuestDetail[0].QuestDetail.iReward_Money		= 100;		

		ObjectDB->SetData( &( QuestDetail[0] ) );

		GameObjDbStruct* pObj = ObjectDB->GetObj( Def_ObjectClass_QuestCollect );
*/
		/*
		GameObjectQuestDetail *pQuest = &( ObjectDB->GetObj( 420027 )->QuestDetail );

		pQuest->iRequest_KillID[0]						= 0;
		pQuest->iProcess_KilltoGetItem_ObjID[0]			= 100001;
		pQuest->iProcess_KilltoGetItem_ProcPercent[0]	= 1;
		pQuest->iProcess_KilltoGetItem_GetItem[0]		= 200002;
		pQuest->iProcess_KilltoGetItem_GetItemVal[0]	= 1;
		*/

		//-----------------------------------------------------------------

		/*
		static GameObjDbStruct ItemTest;
		ItemTest.GUID = Def_ObjectClass_Item;
		*/

        //-------------------------------------------------------------------------------------
        //設定RoleDataEx資料
        //-------------------------------------------------------------------------------------
		/*
        //(暫時) 測試資料
        for( int x = 0 ; x < _MAX_RACE_COUNT_ ; x++ )
        {
            for( int y = 0 ; y < _MAX_RACE_COUNT_ ; y++ )
            {
                if( y == x )
                    RoleDataEx::HateTable[y][x] = 50;
                else
                    RoleDataEx::HateTable[y][x] = 100;
            }
        }
		*/
        //-------------------------------------------------------------------------------------
		/*
        ObjectDB->GetDataByOrder( true );
        for( int i = 0 ; i < ObjectDB->TotalInfoCount() ; i++ )
        {
            Obj = ObjectDB->GetDataByOrder();

            if( Obj->MaxHeap == 0 )
                Obj->MaxHeap = 1;
        }*/
    }
}

#pragma unmanaged