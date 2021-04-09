#pragma		once
#pragma		warning (disable:4786)

#include "..\Global.h"

using namespace std;

//------------------------------------------------------------------------------------
namespace  LuaPlotClass
{
	//初始化
	bool	Init_Hsiang();
    bool	Release_Hsiang();

	//--------------------------------------------------------------------------------
	//位置記錄物件處理 
	//--------------------------------------------------------------------------------
	//把位置記錄到物品上面
	bool	ItemSavePos( int NewItemID );
	//把角色傳送到某位置
	bool	ItemTransportPos( );
	//撿查是否可以用物品傳送
	bool	CheckItemTransportPos( );
	//--------------------------------------------------------------------------------	
	//設定物件面向
	bool	SetDir( int GItemID , float Dir );
	//--------------------------------------------------------------------------------
	//計算面向
	float	CalDir( float Dx , float Dz );
	//--------------------------------------------------------------------------------
	//重設Coldown
	bool	ResetColdown( int GItemID );
	//--------------------------------------------------------------------------------
	//換區
	bool	ChangeZone( int GItemID , int ZoneID , int RoomID , float X , float Y , float Z , float Dir );
	//--------------------------------------------------------------------------------
	//給與裝備
	bool	GiveBodyItem_EQ( int GItemID , int OrgObjID , int Level , int RuneVolume );
	//--------------------------------------------------------------------------------
	//	劇情 Dialog
	//--------------------------------------------------------------------------------
	//產生一個對話框
	bool	DialogCreate( int GItemID , int DialogType , const char* Content );
	//送對話選項
	bool	DialogSelectStr( int GItemID , const char* SelectStr );
	//開起對話框
	bool	DialogSendOpen( int GItemID );
	//重設定內容
	bool	DialogSetContent( int GItemID , const char* Content );
	//取得回應資料
	int			DialogGetResult				( int iObjID );
	const char* DialogGetInputResult		( int iObjID );
	int			DialogGetInputCheckResult	( int iObjID );
	//要求關閉對話框
	bool	DialogClose( int GItemID );
	//設定 TITLE
	bool	DialogSetTitle( int GItemID , const char* Title );
	//--------------------------------------------------------------------------------
	//設定長像
	bool	SetLook( int GItemID , int FaceID , int HairID , int HairColor , int BodyColor );
	//--------------------------------------------------------------------------------
	//設定卡片開關
	bool	SetCardFlag( int GItemID , int CardID , bool Flag );
	//讀取是否有卡片
	bool	GetCardFlag( int GItemID , int CardID );
	//--------------------------------------------------------------------------------
	//公會
	//--------------------------------------------------------------------------------
	//要求開啟建立新公會
	int		OpenCreateNewGuild( );
	//要求開啟貢獻介面
	int		OpenGuildContribution( );
	//要求開啟公會商店
	int		OpenGuildShop( );
	//把連屬公會變成正式公會
	int		SetGuildReady( int GItemID );
	//增加公會的 0 階級數 1 成員 
	//int		AddGuildValue( int GItemID , int Type , int Value , int Score );
	//取得自己的公會狀況
	//-1 找不到此人 0 沒有公會 1 聯屬公會 2 一般公會
	int		GuildState( int GItemID );

	//取得公會階級	-1 代表沒公會
	int		GuildRank( int GItemID );
	//取得公會人數
	int		GuildMemberCount( int GItemID );

	//取得某人公會資料
	int		GuildInfo( int GItemID , int Type );

	//檢查是否在自己的公會城內
	bool	CheckInMyGuildHouse( int PlayerGUID );
	//--------------------------------------------------------------------------------
	//儲存重生點	
	bool	SaveHomePoint( int GItemID );
	//傳送回重生點	
	bool	GoHomePoint( int GItemID );
	//--------------------------------------------------------------------------------
	//開啟樂透彩兌獎介面
	bool	OpenExchangeLottery( );
	//開啟購買樂透彩介面
	bool	OpenBuyLottery( );
	 
	bool	OpenRare3EqExchangeItem();
	//--------------------------------------------------------------------------------
	//開啟Client端介面
	bool	OpenClientMenu( int GItemID , int Type );
	//--------------------------------------------------------------------------------
	//////////////////////////////////////////////////////////////////////////
	//房屋系統
	//////////////////////////////////////////////////////////////////////////
	//買房子
	bool	BuyHouse( int GItemID , int Type );
	//增加房屋能量
	bool	AddHouseEnergy( int GItemID , int Energy );
	//////////////////////////////////////////////////////////////////////////
	//跑馬燈
	//0 自己 1 區域 2 全部的玩家
	bool	RunningMsg( int GItemID , int Type , const char* Msg );
	bool	RunningMsgEx( int GItemID , int Type , int EventType , const char* Msg );
	//////////////////////////////////////////////////////////////////////////
	//儲存回傳點
	bool	SaveReturnPos( int GItemID , int ZoneID , float X , float Y , float Z , float Dir );
	bool	SaveReturnPos_ZoneDef( int GItemID );

	//傳回回傳點
	bool	GoReturnPos( int GItemID );
	bool	GoReturnPosByNPC( int GItemID , int PlayerID );
	//////////////////////////////////////////////////////////////////////////
	//建立玩家小屋
	bool	BuildHouse( int GItemID , int HouseType );

	//要求開啟玩家小屋參觀介面
	bool	OpenVisitHouse( bool IsVisitMyHouse );

	//設定為標準職業等級裝備
	bool	SetStandardWearEq( int GItemID );

	//清除Magic Point 並給TP
	bool	SetStandardClearMagicPointAndSetTP( int GItemID );

	//設定房屋類型
	bool	SetHouseType( int HouseImageObj );
	int		GetHouseType( );

	//////////////////////////////////////////////////////////////////////////
	//設定平行區域
	bool	SetParalledID( int GItemID , int ParalledID );
	
	//檢查區域是否開起
	bool	CheckZone( int ZoneID );
	int		GetZoneID( int PlayerDBID );
	//////////////////////////////////////////////////////////////////////////
	//取得某欄位物品的資訊
	int		GetItemInfo( int GItemID , int PGType , int Pos , int ValueType );
	bool	SetItemInfo( int GItemID , int PGType , int Pos , int ValueType , int Value );
	//////////////////////////////////////////////////////////////////////////
	//取得名字
	const char*	GetName( int GItemID );
	//////////////////////////////////////////////////////////////////////////
	//加輔助法術
	bool	AssistMagic( int GItemID , int MagicBase , int Time );
	//////////////////////////////////////////////////////////////////////////
	//修理所有裝備
	bool	FixAllEq	( int GItemID , int Rate );
	//////////////////////////////////////////////////////////////////////////
	//企劃Log
	bool	DesignLog	( int GItemID , int Type , const char* Content  );

	//////////////////////////////////////////////////////////////////////////
	//計算有幾個空位
	int		EmptyPacketCount( int GItemID );
	//計算包裹Queue
	int		QueuePacketCount( int GItemID );
	//區域PK旗標
	void	PKFlag			( int Type );
	//////////////////////////////////////////////////////////////////////////
	//清除陣營重生點
	void	ClearCampRevicePoint( );
	//設定陣營重生點,一定要循序設定
	bool	SetCampRevicePoint( int CampID , int ZoneID , float X , float Y , float Z , const char* LuaScript );
	//////////////////////////////////////////////////////////////////////////
	//檢查保箱是否還撿取完成
	bool	CheckTreasureDelete	( int GItemID );
	//////////////////////////////////////////////////////////////////////////
	//送信
	void	SendMail( const char* Name , const char* Title , const char* Content );
	void	SendMailEx( const char* fromName , const char* toName , const char* Title , const char* Content , int ItemID , int ItemCount , int Money );
	//////////////////////////////////////////////////////////////////////////
	//設定兩人的關係
	bool	SetRelation( int OwnerID , int TargetID , int Relation1 , float Lv1 , int Relation2 , float Lv2 );
	int		GetRelation( int OwnerID , int TargetID );
	float	GetRelationLv( int OwnerID , int TargetID  );
	bool	SetRelationLv( int OwnerID , int TargetID , float Lv );

	int			GetRelation_DBID( int OwnerID , int Pos );
	const char*	GetRelation_Name( int OwnerID , int Pos );
	float		GetRelation_Lv( int OwnerID , int Pos );
	int			GetRelation_Relation( int OwnerID , int Pos );
	
	bool		SetRelation_Lv( int OwnerID , int Pos , float Lv );
	bool		SetRelation_Relation( int OwnerID , int Pos , int Relation );

	bool		CallPlayer_DBID( int OwnerID , int TargetDBID );
	bool		GotoPlayer_DBID( int OwnerID , int TargetDBID );
	
	//////////////////////////////////////////////////////////////////////////
	bool	AddSkillValue (int OwnerID , int SkillID , float Value );
	
	//檢查二次密碼
	bool	CheckPassword( int OwnerID , const char* Password );
	//////////////////////////////////////////////////////////////////////////
	int		SysKeyValue( const char* KeyStr );
	int		LanguageType( );
	int		GetMoneyKeyValue( const char* KeyStr , int );
	//////////////////////////////////////////////////////////////////////////
	//取得使用物品的資料
	//////////////////////////////////////////////////////////////////////////
	//刪除使用的物品
	bool	UseItemDestroy( );
	//取得使用物品得耐久度
	int		UseItemDurable( );	
	//檢查使用的物品是否還在
	bool	CheckUseItem( );

	//////////////////////////////////////////////////////////////////////////
	//公會屋
	//////////////////////////////////////////////////////////////////////////
	//建立公會
	bool	BuildGuildHouse( int GItemID );
	//要求開啟 公會屋參觀介面
	bool	OpenVisitGuildHouse( bool IsVisitMyHouse );

	//////////////////////////////////////////////////////////////////////////
	//給予金錢
	//////////////////////////////////////////////////////////////////////////
	bool	AddMoney( int GItemID , int ActionType , int Money );
	//增加特殊貨幣
	bool	AddSpeicalCoin( int GItemID , int ActionType , int MoneyType , int Money );
	bool	CheckSpeicalCoin( int GItemID , int MoneyType , int Money );
	bool	AddMoney_Bonus( int GItemID , int ActionType , int Money );
	bool	AddMoney_Account( int GItemID , int ActionType , int Money );
	bool	AddMoneyEx_Account( int GItemID , int ActionType , int Money , bool LockMoneyProc );
	//////////////////////////////////////////////////////////////////////////
	// AI 處理
	//////////////////////////////////////////////////////////////////////////
	//取得仇恨列表內人數量
	int		HateListCount( int GItemID );
	//取得仇恨列表中的某幾個位置的資料
	double	HateListInfo( int GItemID , int Pos , int Type );
	//暫停或開啟NPC AI處理
	bool	EnableNpcAI( int GItemID , bool Flag );
	//清除愁恨表 TargetID = -1 表示全清
	bool	ClearHateList( int GItemID , int TargetID );
	//設定Mode
	bool	SetModeEx( int GItemID , int Type , bool Flag );
	//讀取Mode
	bool	GetModeEx( int GItemID , int Type );
	//檢查是否有組檔
	bool	CheckLine( int GItemID , float Tx , float Ty , float Tz );
	//取得某點的高度
	float	GetHeight( float X , float Y , float Z );
	//設定仇恨列表中的某幾個位置的資料
	bool	SetHateListPoint( int GItemID , int Pos , float Value );

	bool	ModifyRangeHatePoint( int GItemID , int Type , float Point );

	//////////////////////////////////////////////////////////////////////////
	//	Buff處理
	//////////////////////////////////////////////////////////////////////////
	//檢查是否有某Buff
	bool	CheckBuff			( int ItemGUID	, int BuffID );
	bool	CancelBuff			( int iRoleID	, int iMagicID );
	bool	CancelBuff_NoEvent	( int iRoleID	, int iMagicID );
	
	bool	AddBuff			( int ItemGUID , int BuffID , int MagicLv , int EffectTime );
	int		BuffCount		( int ItemGUID );
	int		BuffInfo		( int ItemGUID , int Pos , int Type );

	//修改Buff資訊
	bool	ModifyBuff		( int GItemID , int BuffID , int Power , int Time );
	bool	CancelBuff_Pos	( int GItemID , int Pos );


	double		GameObjInfo_Double( int OrgObjID , const char* DB_FieldName );
	const char*	GameObjInfo_Str	( int OrgObjID , const char* DB_FieldName );
	bool		WriteGameObjInfo_Double( int OrgObjID , const char* DB_FieldName , double value );


	//--------------------------------------------------------------------------------
	bool	TouchTomb		( );
	void	ClientSkyType	( int Type );
	//--------------------------------------------------------------------------------
	//讀取得公會建築資料
	int		ReadGuildBuilding	( int ItemGUID , int Type );
	bool	WriteGuildBuilding	( int ItemGUID , int Type , int Value );
	bool	AddGuildResource	( int PlayerGUID , int Gold , int BonusGold , int Mine , int Wood , int Herb , int GuildRune , int GuildStone , const char* RetRunplot );
	//--------------------------------------------------------------------------------
	//顯示Client端的時間
//	bool	ClientClock( int GItemID , int BeginTime , int NowTime , int EndTime , int Type );
	bool	ClockOpen		( int GItemID , int CheckValue , int BeginTime , int NowTime , int EndTime , const char* ChangeZonePlot , const char* TimeUpPlot );
	bool    ClockStop		( int GItemID );
	bool    ClockClose		( int GItemID );
	int		ClockRead		( int GItemID , int Type );
	//--------------------------------------------------------------------------------
	bool	SysCastSpellLv		( int RoleID, int TargetID, int MagicID , int MagicLv );
	bool	SysCastSpellLv_Pos	( int RoleID, float X , float Y , float Z , int MagicID , int MagicLv );
	bool	InterruptMagic		( int GItemID );
	//--------------------------------------------------------------------------------
	const char* GetGuildName	( int GuildID );
	//////////////////////////////////////////////////////////////////////////
	//寵物劇情
	//////////////////////////////////////////////////////////////////////////
	//產生寵物物品
	bool	GiveItem_Pet( int GItemID , int ItemOrgID , int Lv , int Range , int MaxTotalPoint , int NpcOrgID );

	//重新計算寵物等級
	bool	ReCalPetLv	( int GItemID , int Pos );
	//////////////////////////////////////////////////////////////////////////
	//設定座騎顏色
	bool	SetHourseColor( int GItemID , int  Color1 , int Color2 , int Color3 , int Color4 );

	//交換裝備
	bool	SwapEQ( int GItemID , int posID );
	//////////////////////////////////////////////////////////////////////////
	// 公會屋戰
	//////////////////////////////////////////////////////////////////////////
	//增減公會戰積分
	bool	AddGuildWarScore( int GItemID , int Score );
	int		GetGuildWarScore( int GItemID );

	bool	AddGuildWarEnergy( int GItemID , int Value );
	int		GetGuildWarEnergy( int GItemID );

	//增加公會戰場個人積分 ( Type = 0 傷害量 1 擊破數 2 建築數 4 摧毀數 5 治療量 )
	bool	AddGuildWarPlayerValue( int GItemID , int Type , int Score );
	int		GetGuildWarPlayerValue( int GItemID , int Type );

//	bool	AddGuildWarValue( int GItemID , int Type , int Value );
//	int		GetGuildWarValue( int GItemID , int Type );
	
	int		GetGuildWarScore_ByID( int RoomID , int ID );

	const char* GetGuildName_ByID( int RoomID , int ID );

	

	//bool SetMark( int GItemID , int MarkID , int StrKeyID )
	//////////////////////////////////////////////////////////////////////////
	//刪除職業
	bool	DelJob( int GItemID , int Job );
	//////////////////////////////////////////////////////////////////////////
	// 拷貝刪除房間
	bool	CopyRoomMonster	( int FromRoomID , int ToRoomID );
	bool	DelRoomMonster	( int RoomID );
	//////////////////////////////////////////////////////////////////////////
	//讀取所在位置的地圖id
	int		FindMapID( float X , float Y , float Z );
	//檢查所在位置是否在此地圖
	bool	CheckMapID( float X , float Y , float Z , int MapID );
	//////////////////////////////////////////////////////////////////////////
	//上載具
	bool	AttachObj( int GItemID , int WagonItemID , int Type , const char* ItemPos , const char* WagonPos );
	//下載具
	bool	DetachObj( int GItemID );

	bool	CheckAttach( int WagonItemID , int GItemID );

	bool	CheckAttachAble( int WagonItemID );
	//////////////////////////////////////////////////////////////////////////
	//中斷目前的施法
	bool	MagicInterrupt( int GItemID );
	//////////////////////////////////////////////////////////////////////////
	//重新計算角色數值˙
	bool	ReCalculate( int GItemID );
	//
	//////////////////////////////////////////////////////////////////////////
	bool	SetSmallGameMenuType	( int GItemID , int MenuID , int Type );
	bool	SetSmallGameMenuValue	( int GItemID , int MenuID , int ID , int Value );
	bool	SetSmallGameMenuStr		( int GItemID , int MenuID , int ID , const char* Content );
	//////////////////////////////////////////////////////////////////////////
	//搜尋可檢的屍體
	int		SearchTreasure			( int GItemID );
	//檢取屍體
	bool	GetTreasure				( int GItemID , int PetID , int TreasureID );
	//開關NPC AI
	bool	DisabledNpcAI			( int GItemID , bool Flag );
	//////////////////////////////////////////////////////////////////////////
	bool	StopMove				( int GItemID , bool Immediately );
	//////////////////////////////////////////////////////////////////////////
	int		PartyInstanceLv			( int GItemID );
	//儲存副本進度
	bool	WriteInstanceArg		( int KeyID , int Value );
	//讀取副本進度
	int		ReadInstanceArg			( int RoomID, int KeyID );
	//////////////////////////////////////////////////////////////////////////
	//讀取遊戲時間
	int		GameTime				( );
	//////////////////////////////////////////////////////////////////////////
	//讀取帳號旗標
	bool	ReadAccountFlag			( int GItemID , int Pos );
	//寫入帳號旗標
	bool	WriteAccountFlag		( int GItemID , int Pos , bool Value );
	int		ReadAccountFlagValue	( int GItemID , int Pos , int PosCount );
	bool	WriteAccountFlagValue	( int GItemID , int Pos , int PosCount , int Value );
	//////////////////////////////////////////////////////////////////////////
	//設定Map icon & tip
	bool	ResetMapMark			( int RoomID );
	bool	SetMapMark				( int RoomID , int MarkID , float X , float Y , float Z , const char* TipStr , int IconID );
	bool	ClsMapMark				( int RoomID , int MarkID );
	//////////////////////////////////////////////////////////////////////////
	//設定區域變數
	bool	ResetRoomValue			( int RoomID );
	bool	SetRoomValue			( int RoomID , int KeyID , int Value );
	int		GetRoomValue			( int RoomID , int KeyID );
	bool	ClsRoomValue			( int RoomID , int KeyID );
	//////////////////////////////////////////////////////////////////////////
	//物件的參考點上建物件
	int		CreateObj_ByObjPoint	( int GItemID , int ObjID , const char* Point );
	
	//////////////////////////////////////////////////////////////////////////
	//排序資料
	void	SortList_Reset			();
	void	SortList_Push			( int KeyID , int SortValue );
	int		SortList_Sort			();
	int		SortList_Get			();
	//////////////////////////////////////////////////////////////////////////
	//企劃設定暫時可施展的法術
	bool	SetExSkill				( int GItemID , int SpellerID , int SkillID , int SkillLv );
	//刪除企劃設定法術 SpellID = -1 表示忽略檢查施法者, SkillID = -1 表示忽略技能類型
	bool	DelExSkill				( int GItemID , int SpellerID , int SkillID );

	//////////////////////////////////////////////////////////////////////////
	//通知Client 公會戰最後結果
	void	SendGuildWarFinalScore  ( int RoomID , int PlayerDBID , int PrizeType , int	Score , int OrderID );
	//////////////////////////////////////////////////////////////////////////
	//進入世界
	bool	EnterWorld				( int GItemID , int WorldID , int ZoneID , int RoomID , float X , float Y , float Z , float Dir );
	//離開某世界
	bool	LeaveWorld				( int GItemID );

	//////////////////////////////////////////////////////////////////////////
	//排行榜( top更新 )
	bool	Billboard_TopUpdate		( int GItemID , int SortType , int Value );
	//排行榜( 更新 )
	bool	Billboard_Update		( int GItemID , int SortType , int Value );
	//排行榜( 修正 )
	bool	Billboard_Add			( int GItemID , int SortType , int Value );
	//////////////////////////////////////////////////////////////////////////
	//開啟兌換魔石的介面
	bool	OpenMagicStoneShop		( int MagicStoneID , int Lv , int Money1 , int Money2 );
	//////////////////////////////////////////////////////////////////////////
	//設定縮放大小
	bool	SetModelScale			( int GItemID , float Scale );
	//////////////////////////////////////////////////////////////////////////
	//lua 觸發Client事件
	void	ClientEvent				( int SendID , int GItemID , int EventType , const char* Content ); 
	void	ClientEvent_Range		( int GItemID , int EventType , const char* Content ); 
	//////////////////////////////////////////////////////////////////////////
	//女僕數值
	int		ReadServantValue		( int ServantID , int Type );
	bool	WriteServantValue		( int ServantID , int Type , int Value );

	bool	ReadServantFlag			( int ServantID , int Index );
	bool	WriteServantFlag		( int ServantID , int Index , bool Result );
	//////////////////////////////////////////////////////////////////////////
	//取得DB設定的keyvalue
	int		GetDBKeyValue			( const char* KeyStr );
	//////////////////////////////////////////////////////////////////////////
	//檢查人物是否在線上
	bool	CheckOnlinePlayer		( int DBID );
	//////////////////////////////////////////////////////////////////////////
	//取得目前系統時間 1900年到目前過了幾秒
	int		GetNowTime();

	//DBID 轉 GUID 找不到回傳-1
	//int		PlayerDBIDtoGUID( int DBID );
	//////////////////////////////////////////////////////////////////////////
	//鎖定血量 0 以下表示不鎖定
	bool	LockHP( int GItemID , int HP , const char* LuaEvent );
	//////////////////////////////////////////////////////////////////////////
	//特殊移動
	bool	SpecialMove( int GItemID , float X , float Y , float Z , int Type );
	//////////////////////////////////////////////////////////////////////////
	//設定法術命中目標
	bool	PushMagicExplodeTarget( int TargetID );
	bool	PushMagicShootTarget( int TargetID );

	//要求其他區域跑PCall
	bool	ZonePCall( int zoneID , const char* plotCmd );

	//////////////////////////////////////////////////////////////////////////
	//獨立遊戲 戰場
	bool	InitBgIndependence( int GItemID , int mode );

	//取得最後執行隱藏的劇情
	const char*	GetLastHidePlotStr( int OwnerID );


	//////////////////////////////////////////////////////////////////////////
	//線段組檔
	//////////////////////////////////////////////////////////////////////////
	void	ClearLineBlock( int roomID );
	void	AddLineBlock( int roomID , int id , float x1 , float y1 , float x2 , float y2 );
	bool	DelLineBlock( int roomID , int id );
	bool	CheckLineBlock( int roomID , float x1 , float y1 , float x2 , float y2 );
	
	//////////////////////////////////////////////////////////////////////////
	//重置副本傳送次數
	bool	WeekInstancesReset(  );
};
