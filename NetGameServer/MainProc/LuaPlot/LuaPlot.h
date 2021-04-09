#pragma		once
#pragma		warning (disable:4786)

#include "..\Global.h"
#include "LuaPlot_Hsiang.h"
#include "LuaPlot_PublicEncounter.h"

using namespace std;

//------------------------------------------------------------------------------------s
namespace  LuaPlotClass
{
	//初始化
	bool	Init();
    bool	Release();

	//---------------------------------------------------------------------------------
	//取得系統字串
	const char* GetString( const char* szKeyName );

    //---------------------------------------------------------------------------------
    //取得劇情擁有者的ID   
    int OwnerID( );
    //---------------------------------------------------------------------------------
    //取得劇情目標的ID
    int TargetID();
    //---------------------------------------------------------------------------------
    //取亂數
    // return 小於 Value的數字
    int Random( int Range );
    //---------------------------------------------------------------------------------
    //列印資料
    void SysPrint( const char* );
    //---------------------------------------------------------------------------------
	//移到目標
	int	MoveTarget( int ID , int TargetID );
    //定點 移動
    int Move( int ID , float x , float y , float z );
	//定點 移動不做路逕搜尋
	int MoveDirect( int ID , float x  , float y , float z );	
    //---------------------------------------------------------------------------------
    //設定亂數移動
    void SetRandMove( int ID , int Range , int Freq, int Dis );
    //*********************************************************************************
    //系統指令
    //*********************************************************************************
    //產生旗子
    bool CreateFlag( int ObjID , int FlagID, float X , float Y , float Z , float Dir );
    //---------------------------------------------------------------------------------
    //產生物件
    int CreateObj( int ObjID , float x , float y , float z , float dir , int count );
    //利用旗子座標產生物件 FlagID = -1 代表用亂數
    int CreateObjByFlag( int ObjID , int FlagObjID , int FlagID , int count );
    //---------------------------------------------------------------------------------
    //刪除物件
    bool DelObj( int ItemID );
    //---------------------------------------------------------------------------------
    //加入顯像物件
    bool AddtoPartition( int GItemID , int RoomID );
    //---------------------------------------------------------------------------------
    //加入顯像物件
    bool DelFromPartition( int GItemID );
	//---------------------------------------------------------------------------------
	//加入顯像物件
	bool Show( int GItemID , int RoomID );
	//---------------------------------------------------------------------------------
	//加入顯像物件
	bool Hide( int GItemID );
    //---------------------------------------------------------------------------------
    //執行劇情
    bool RunPlot( int GItemID , const char* PlotName );
    //---------------------------------------------------------------------------------
    //設定NPC死亡
    void  NPCDead( int DeadID , int KillID );
    //---------------------------------------------------------------------------------
    //設定物件模式
    bool SetMode( int GItemID , int Mode );
	int	 GetMode( int GitemID );
    //---------------------------------------------------------------------------------
    //重設NPC資料	
    //回傳需要多久的時間才可以重生
    int ReSetNPCInfo( int ID );

	//*********************************************************************************
	// 地雷處理
	//*********************************************************************************
	//地雷觸發撿查,並且爆炸消失
	//回傳是否觸發地雷
	bool CheckMineEvent( );

    //*********************************************************************************
    //據情 關於物品
    //*********************************************************************************
    //給予物品
    //回傳 0 代表失敗   1 代表成?
    bool GiveBodyItem( int GItemID , int OrgID , int Count );
	bool GiveBodyItemEx( int GItemID , int OrgID , int Count , int RuneVolume , int Inherent1 , int Inherent2 , int Inherent3 , int Inherent4 , int Inherent5 );

	bool GiveBodyItem_Note( int GItemID , int OrgID , int Count , const char* Note );
	    //---------------------------------------------------------------------------------
    //刪除身上的某物品    
    //回傳 0 代表失敗   1 代表成?
	bool DelBodyItem( int GItemID , int OrgID , int Count );
    //---------------------------------------------------------------------------------
    //計算身上某一樣物品多少個
    //回傳 數量
    int CountBodyItem( int GItemID  , int OrgID );
    //---------------------------------------------------------------------------------
    //計算此角色共有多少個 ( Bank Body EQ )
    //回傳 數量
    int CountItem( int GItemID  , int OrgID );
    //---------------------------------------------------------------------------------

    //*********************************************************************************
    //據情 ......
    //*********************************************************************************
    //訊息
    bool    Msg( int GItemID , const char* Str );
    //---------------------------------------------------------------------------------
    //密語
    bool    Tell( int SendID , int SayID , const char* Str );
	bool	SayTo( int SendID , int SayID , const char* Str );
	//---------------------------------------------------------------------------------
    //範圍 說話
    bool    Say( int SayID , const char* Str );
	bool    NpcSay( int SayID , const char* Str );
	
	//範圍 自動說話範圍
	bool	Yell( int SayID, const char* Str, int iRange );
	//---------------------------------------------------------------------------------
	void ScriptMessage ( int SayID , int iTarget, int iType, const char* Str, const char* szColor );
    //---------------------------------------------------------------------------------
    //角色物件資料讀取
    //名稱 : RoleName Hp Mp ....
    int     ReadRoleValue( int GItemID , int Type );
	double  ReadRoleValuefloat( int GItemID , int Type );
    //char*	ReadRoleStr( int GItemID , int Type );
    //---------------------------------------------------------------------------------
    //角色物件資料寫入
    bool    WriteRoleValue( int GItemID , int Type , double Value );
	bool    AddRoleValue( int GItemID , int Type , double Value );
    //bool	WriteRoleStr( int GItemID , int Type , const char* Str );
	bool    AddExp( int GItemID , int Value );
	
    //*********************************************************************************
    //劇情設定指令
    //*********************************************************************************
    //設定劇情
    //Plot ==> void	SetPlot( ID , 劇情Type , 劇情名稱 )
    //例如 ==> void SetPlot( ID , "ClientAuto" , "TestPlot" );
    //Type = "ClientAuto" , "Auto" , "Move" , "Dead" , "Range" , "Touch"
    bool SetPlot( int GItemID , const char* TypeName , const char* PlotName , int Range );
	const char* GetPlotName( int GItemID , const char* PlotType  );
    //---------------------------------------------------------------------------------
    //執行某劇情
    bool BeginPlot( int GItemID , const char* PlotName , int DelayTime );
	bool BeginMultiStringPlot( int GItemID , const char* PlotName , int DelayTime );
    //---------------------------------------------------------------------------------
    //設定npc所屬劇情類別 用來之後收尋npc執行劇情用 or 其他的事
    //Plot ==> int	SetNpcPlotClass( ID , 自訂字串 )
    //例如 ==> int  SetNpcPlotClass( 1 , "AAA" );
    // Ret = 0 (執行失敗)無此物件
    //	   = 1 成?
    bool SetNpcPlotClass( int GItemID , const char* Str );	
    //---------------------------------------------------------------------------------
	//停止系統旗子點移動( Flag = false 停止   ture 可移動 )
	bool MoveToFlagEnabled	( int GItemID , bool Flag );
	bool ClickToFaceEnabled	( int GItemID , bool Flag );
	//---------------------------------------------------------------------------------
	//定即定位的指令,讓NPC朝向玩家 
	bool FaceObj	( int iSouceID, int iTargetID );
	bool AdjustDir	( int iSouceID, int iAngle );
	//---------------------------------------------------------------------------------
	//殺死某物件
	bool KillID		( int KillID , int DeadID );
/*
    //---------------------------------------------------------------------------------
     
    //---------------------------------------------------------------------------------
    //讀取某一旗標
    bool    ReadFlag( int ItemID , int FlagID );	
    //---------------------------------------------------------------------------------
    //寫入某一旗標
    bool    WriteFlag( int ItemID , int FlagID , int On );	
    //---------------------------------------------------------------------------------
    //(讀取)把某一旗標當數值處理
    //例如 ==> int ReadFlagValue( 0 , 10 , 5  )   <== 代表重10 - 14 旗標代表的數字
    //return = 旗標用用二進位代表的數字

    int	    ReadFlagValue( int ItemID , int FlagID , int FlagCount );
    //---------------------------------------------------------------------------------
    //(寫入)把某一旗標當數值處理
    //Plot ==> int WriteFlagValue( 開使旗標 , 多少旗標表示 , 要存的數字 )
    //例如 ==> int WriteFlagValue( ItemID , 10 , 5 , 19 )   <== 代表重10 - 14 旗標代表的數字 存入19
    //return  =  0 失敗
    //		  	 1 成?
    bool    WriteFlagValue( int ItemID , int FlagID , int FlagCount , int Value );
    */
    //---------------------------------------------------------------------------------
    //檢查物件是否存在
    //Plot ==> int CheckID( ItemID )
    //例如 ==> int CheckID( 100 )   <== 檢查100號物件是否存在
    //return  =  0 沒有
    //		  	 1 有
    bool	CheckID( int GItemID );
    //---------------------------------------------------------------------------------
    //取得角色資料
    //Plot ==> int GetValue( ID , Type )
//    int     GetValue( int GItemID , int Type );
    //---------------------------------------------------------------------------------
    //商店
    //---------------------------------------------------------------------------------
    //商店設定
    void    SetShop( int GItemID , int ShopObjID , const char* ShopClosePlot );
    //開起商店
    bool    OpenShop( );
	//
	bool	CloseShop( int GItemID );

	//開啟銀行
	bool    OpenBank( );
	bool    OpenBankEx( int TargetID );
	//開起 Mail
	bool	OpenMail( );
	bool	OpenMailEx( int TargetID );
	//開啟 AC
	bool	OpenAC( );
	bool	OpenACEx( int TargetID );
	//開啟	換角
	void	OpenChangeJob( );
	//開起 排行榜
	void    OpenBillboard( );
	//開起公會競標
	void	OpenGuildWarBid( int iPlayer, int iNpc );
	//開啟 溶合裝備
	void	OpenCombinEQ( int PlayerGUID , int NpcGUID , int Type );
	//開啟 劇情給予物品
	void	OpenPlotGive( int GItemID , bool IsSort , int Count );
	//開啟 帳號背包
	void	OpenAccountBag( );
	//開啟 裝備賭博
	bool	OpenEqGamble();
	//開啟　套裝系統
	bool	OpenSuitSkill();
	//---------------------------------------------------------------------------------
    //戰鬥劇情
    //---------------------------------------------------------------------------------
    //設定尋找某一個物件
    //回傳 : 搜尋到的數量
    int     SetSearchRangeInfo( int GItemID , int Range );
	int     SetSearchAllNPC( int RoomID );
	int		SetSearchAllPlayer( int RoomID );

	//收尋房間內某個企劃群組的物件
	int		SetSearchAllNPC_ByGroupID( int RoomID , int GroupID );

    int     GetSearchResult( );

    //設定某物件開啟攻擊
    bool    SetAttack( int GItemID , int TargetID );
    //關閉攻擊
    bool    SetStopAttack( int GItemID );

    //設定逃跑開啟或關閉  (戰鬥不關)
    bool    SetEscape( int GItemID , bool Flag );

    //--------------------------------------------------------------------------------
	// 取得旗子的位置
	//Type 
	//	EM_RoleValue_X
	//	EM_RoleValue_Y
	//	EM_RoleValue_Z
	//	EM_RoleValue_Dir
	int	GetMoveFlagValue( int FlagObjID , int FlagID , int Type );

	// 取得某類旗子的數量
	int	GetMoveFlagCount( int FlagObjID );

	//設定位置
	bool	SetPos( int GItemID , float X , float Y , float Z , float Dir );

	//----------------------------------------------------------------------------------------
	// 陣營設定
	//----------------------------------------------------------------------------------------
	//設定某物件所屬陣營
	bool	SetRoleCamp			( int GUID , const char* CampName );
	//設定某區域所用的陣營資訊
	bool	SetZoneCamp( int OrgObjID );
	//////////////////////////////////////////////////////////////////////////
    //--------------------------------------------------------------------------------
	// 通知 Client 點選物件前置檢查結果
	void SendQuestClickObjResult( int iRoleGUID, int iResult, int iItemID, int iItemVal );

	// 暫時設定 LUA 處理結果用
	void	DisableQuest		( int iObjID, bool bDisableQuest );
	void	SetQuestState		( int iRoleGUID, int iQuest, int iResult );
	int		GetQuestState		( int iRoleGUID, int iQuest );

	void	SetPosture			( int iGUID, int iPostureID, int iMotionID );
	void	PlayMotion			( int iObjID, int iMotionID );
	void	PlayMotion_Self		( int PlayerID , int PlayMotionObjID , int MotionID );
	void	PlayMotionEx		( int iObjID, int iMotionID , int iIdleMotionID );
	void	SetIdleMotion		( int iObjID, int iMotionID );
	void	SetDefIdleMotion	( int iObjID, int iMotionID );

	void	SetFightMode		( int iGUID, int iSearchEnemy, int iMove, int iEscape, int iFight  );
	bool	FaceObj				( int iSourceID, int iTargetID );
	void	FaceFlag			( int iSourceID, int iFlagGroup, int iFlagID );

	// 對話系統相關
	void	AddSpeakOption		( int iRoleID, int iNPCID, const char* pszOption, const char* pszLuaFunc, int iIconID );
	void	SetSpeakDetail		( int iRoleID, const char* pszDetail );
	void	LoadQuestOption		( int iRoleID );
	void	CloseSpeak			( int iRoleID );

	// 任務相關
	bool	CheckQuest			( int iRoleID, int iQuestID, int iMode );

	// 劇情表演相關
	bool	CastSpell			( int iRoleID, int iTargetID, int iMagicID );
	bool	CastSpellLv			( int iRoleID, int iTargetID, int iMagicID , int iMagicLv );
	bool	CastSpellPos		( int iRoleID, float X , float Y , float Z , int iMagicID , int iMagicLv );

	void	SetScriptFlag		( int iRoleID, int iFlagID, int iVal );
	bool	CheckScriptFlag		( int iRoleID, int iFlagID );
	bool	CheckCardFlag		( int iRoleID, int iFlagID );

	bool	CheckCompleteQuest	( int iRoleID, int iQuestID );
	bool	CheckAcceptQuest	( int iRoleID, int iQuestID );

	void	MoveToPathPoint		( int iRoleID, int iPathID );
	void	CallSpeakLua		( int iNPCID, int iPlayerID, const char* pszSpeakLua );
	void	LockObj				( int iObjID, int iLockType, int iLockID );

	// 移動劇情相關
	void	SetPosByFlag		( int iRoleID, int iFlagObjID, int iFlagID );
		// iMode = 0 ( 個人及隊伍 ), 1 = ( 個人 ), 2 = ( 保留 EX: 團隊 )

	void	SetRoleEquip		( int iRoleID, int iPartID, int iItemID, int iCount );

	int		PlaySound			( int iRoleID, const char* pszFile, bool bLoop );
	int		PlaySoundToPlayer	( int iRoleID, const char* pszFile, bool bLoop );

	int		PlayMusic			( int iRoleID, const char* pszFile, bool bLoop );
	int		PlayMusicToPlayer	( int iRoleID, const char* pszFile, bool bLoop );

	int		Play3DSound			( int iRoleID, const char* pszFile, bool bLoop );
	int		Play3DSoundToPlayer	( int iRoleID, const char* pszFile, bool bLoop );


	void	StopSoundToPlayer	( int iRoleID, int iSoundID );
	void	StopSound			( int iRoleID, int iSoundID );
	
	
	const char* GetString		( const char* pszString );
	int		RandRange			( int iMin, int iMax );
	bool	CheckAcceptQuest	( int iRoleID, int iQuestID );

	int		GetDistance			( int iOwnerID, int iTargetID );
	bool	CheckDistance		( int iOwnerID, int iTargetID, int iRange );

	void	SetMinimapIcon		( int iRoleID, int iIconID );
	void	SetCursorType		( int iObjID, int iCursorID );
	//--------------------------------------------------------------------------------
	// 特殊 測試用指令
	//--------------------------------------------------------------------------------
	//設定玩家裝備
	bool	SetRoleEq_Player	( int PlayerID , int Part , int ItemID );


	void			AddBorderPage	( int iObjID, const char* sPageText );
	void			ClearBorder		( int iObjID );
	int				GetUseItemGUID	( int iObjID );
	const char*		GetObjNameByGUID( int iGUID );
	void			ShowBorder		( int iObjID, int iQuestID, const char* sBorderTitle, const char* sTexture );
	const char*		GetQuestDesc	( int iQuestID );
	const char*		GetQuestDetail	( int iQuestID, int iType );
	void			SetFollow		( int, int );


	//--------------------------------------------------------------------------------
	// 隊伍劇情用指令
	//--------------------------------------------------------------------------------
	int				GetPartyID		( int, int );

	// 劇情使用 CastBar
	int				BeginCastBar			( int iObjID, const char* pszString, int iTime, int iMotionID, int iMotionEndID, int iFlag );
	int				BeginCastBarEvent		( int iObjID, int iClickObjID, const char* pszString, int iTime, int iMotionID, int iMotionEndID, int iFlag, const char* pszLuaEvent );
	int				CheckCastBar			( int iObjID );
	void			EndCastBar				( int iObjID, int iResult );
	int				CheckCastBarStatus		( int iObjID );
	
	
	void			AttachCastMotionTool	( int iObjID, int iAttachItemID );	// 必需要先執行過 BeginCastBar 之後, 才跑這函式, 在使用 BeginCast bar 時, 手上綁取指定物品, 在 EndCastBar 時拔掉
	void			AttachTool				( int iRoleID, int iToolID, const char* cszLinkID );
	void			StartClientCountDown	( int iObjID, int iStartNumber, int iEndNumber, int iOffset,int iMode, int iShowStatusBar, const char* Str);				// 使用 Client 介面開始一個倒數計時
	void            StopClientCountDown     ( int iObjID);																// 停止 Client 介面倒數計時


	bool			AdjustFaceDir	( int iSouceID, int iTargetID, int iAngle );
		// 面向物件, 並允許調整角度
	void			GenerateMine	( int iRoomID, int iFlagID, int iMineID,	int iMineCount );

	void			SetCrystalID	( int iObjID, int iCrystalID );
	
	int				GetSystime		( int iTimeID );

	bool			DeleteQuest		( int iObjID, int iQuestID );

	int				GetBodyFreeSlot	( int iObjID );

	void			ResetObjDailyQuest	( int iObjID );
	int				GetDailyQuestCount	( int iObjID );
	void			SendBGInfoToClient	( int iBGID, int iClientID );
	void			SetBattleGroundObjClickState( int iObjID, int iTeamID, int iClickAble );

	bool			SetRoleCampID	( int iObjID, int iCampID );
	int				GetRoleCampID	( int iObjID );
	bool			ResetRoleCampID		( int ObjID );
	void			ChangeObjID		( int iObjID, int iID );

	void			SetScore		( int iRoomID, int iTeamID, int iVal );
	int				GetScore		( int iRoomID, int iTeamID );
	
	int				GetTeamID		( int iObjID );
	
	void			SetZoneReviveScript		( const char* pszPVP );
	void			SetZonePVPScript		( const char* pszPVP );
	
	void			EndBattleGround			( int iRoomID, int iTeamID );
	void			CloseBattleGround		( int iRoomID );

	void			SetLandMarkInfo	( int iRoomID, int iTeamID, int iID, float fX, float fY, float fZ, const char* szName, int iIconID );
	void			SetLandMark		( int iRoomID, int iID, int iVal );
	int				GetLandMark		( int iRoomID, int iID );

	void			SetVar			( int iRoomID, int iID, int iVal );
	int				GetVar			( int iRoomID, int iID );


	void			AddRecipe		( int iObjID, int iRecipeID );
	void			GetRecipeList	( int iClientID, int iObjID );

	bool			SetCustomTitleString( int iObjID, const char* pszTitle );

	
	int				DebugCheck		(lua_State *L);
	int				CallPlot		(lua_State *L);
	bool			CheckBit		( int iValue, int iID );
	int				SetBitValue		( int iValue, int iBitID, int iBitValue   );

	bool			CheckRelation	( int OwnerID , int TargetID , int Type  );
	

	const char*		GetServerStringLanguage();
	const char*		GetServerDataLanguage();

	int				GetNumTeam		();

	int				GetNumQueueMember		( int iLV, int iQueueID );
	int				GetNumTeamMember		( int iRoomID, int iTeamID  );	
	int				GetDisconnectTeamMember	( int iRoomID, int iTeamID  );
	int				GetNumEnterTeamMember	( int iRoomID, int iTeamID  );
	int				GetBattleGroundRoomID	( int iID );


	int				GetMaxTeamMember		();
	int				GetWorldQueueRule		();


	
	void			AddTeamItem				( int iRoomID, int iTeamID, int iItemID, int iItemCount );
	void			DelRoleItemByDBID				( int iDBID, int iItemID, int iItemCount );

	int				GetGUIDByDBID					( int iDBID );



	void			SetBattleGroundObjCampID		( int iDBID, int iCampID );
	void			SetBattleGroundObjForceFlagID	( int iDBID, int iFlagID );
	void			SetBattleGroundObjGUIDForceFlag	( int iGUID, int iFlagID );
	void			MoveRoleToBattleGround			( int iWorldID, int iWorldZoneID, int iClientDBID, int iRoomID, float X, float Y, float Z, float Dir );


	void			SetArenaScore					( int iRoomID, int iTeamID, int iArenaType, int iIndex, float iValue );
	float			GetArenaScore					( int iRoomID, int iTeamID, int iArenaType, int iIndex	);

	void			Revive							( int iOwnerID, int iZoneID, float x, float y, float z, float ExpDecRate );
	void			DebugMsg						( int iTargetID , int iRoomID, const char* Str );
	void			DebugLog						( int iLV, const char* pszString );
	void			SetDelayPatrolTime				( int iObjID, int iTime );

	void			SetMemberVar					( int iRoomID, int iTeamID, int iGUID, int iVarID, int iVar );
	int				GetMemberVar					( int iRoomID, int iTeamID, int iGUID, int iVarID );

	int				GetEnterTeamMemberInfo			( int iRoomID, int iTeamID, int iID );
	void			JoinBattleGround				( int iObjID, int iBattleGroundID, int iPartyID, int iSrvID );
	
	
	void			AddHonor						( int iObjID, int iValue );
	void			AddTeamHonor					( int iRoomID, int iTeamID, int iHonor	);

	void			AddRankPoint					( int iObjID, int iRankPoint );
	void			AddTeamRankPoint				( int iRoomID, int iTeamID, int iRankPoint );

	void			SetCameraFollowUnit				( int iRoleID, int iObjID, int offsetX, int offsetY, int offsetZ);
	int				GetMemory						();

	void			SetWorldVar						( const char* pszVarName, int iVarValue );
	int				GetWorldVar						( const char* pszVarName );
			
	int				GetLocaltime					( int iTimeVal, int iTimeID );
	void			JoinBattleGroundWithTeam		( int iObjID, int iZoneID );
	void			LeaveBattleGround				( int iObjID );

	int				GetNumPlayer					();
	int				GetIniValueSetting				( const char* pszVarName );

};
