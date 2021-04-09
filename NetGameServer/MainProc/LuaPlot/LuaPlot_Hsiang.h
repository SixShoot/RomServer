#pragma		once
#pragma		warning (disable:4786)

#include "..\Global.h"

using namespace std;

//------------------------------------------------------------------------------------
namespace  LuaPlotClass
{
	//��l��
	bool	Init_Hsiang();
    bool	Release_Hsiang();

	//--------------------------------------------------------------------------------
	//��m�O������B�z 
	//--------------------------------------------------------------------------------
	//���m�O���쪫�~�W��
	bool	ItemSavePos( int NewItemID );
	//�⨤��ǰe��Y��m
	bool	ItemTransportPos( );
	//�߬d�O�_�i�H�Ϊ��~�ǰe
	bool	CheckItemTransportPos( );
	//--------------------------------------------------------------------------------	
	//�]�w���󭱦V
	bool	SetDir( int GItemID , float Dir );
	//--------------------------------------------------------------------------------
	//�p�⭱�V
	float	CalDir( float Dx , float Dz );
	//--------------------------------------------------------------------------------
	//���]Coldown
	bool	ResetColdown( int GItemID );
	//--------------------------------------------------------------------------------
	//����
	bool	ChangeZone( int GItemID , int ZoneID , int RoomID , float X , float Y , float Z , float Dir );
	//--------------------------------------------------------------------------------
	//���P�˳�
	bool	GiveBodyItem_EQ( int GItemID , int OrgObjID , int Level , int RuneVolume );
	//--------------------------------------------------------------------------------
	//	�@�� Dialog
	//--------------------------------------------------------------------------------
	//���ͤ@�ӹ�ܮ�
	bool	DialogCreate( int GItemID , int DialogType , const char* Content );
	//�e��ܿﶵ
	bool	DialogSelectStr( int GItemID , const char* SelectStr );
	//�}�_��ܮ�
	bool	DialogSendOpen( int GItemID );
	//���]�w���e
	bool	DialogSetContent( int GItemID , const char* Content );
	//���o�^�����
	int			DialogGetResult				( int iObjID );
	const char* DialogGetInputResult		( int iObjID );
	int			DialogGetInputCheckResult	( int iObjID );
	//�n�D������ܮ�
	bool	DialogClose( int GItemID );
	//�]�w TITLE
	bool	DialogSetTitle( int GItemID , const char* Title );
	//--------------------------------------------------------------------------------
	//�]�w����
	bool	SetLook( int GItemID , int FaceID , int HairID , int HairColor , int BodyColor );
	//--------------------------------------------------------------------------------
	//�]�w�d���}��
	bool	SetCardFlag( int GItemID , int CardID , bool Flag );
	//Ū���O�_���d��
	bool	GetCardFlag( int GItemID , int CardID );
	//--------------------------------------------------------------------------------
	//���|
	//--------------------------------------------------------------------------------
	//�n�D�}�ҫإ߷s���|
	int		OpenCreateNewGuild( );
	//�n�D�}�Ұ^�m����
	int		OpenGuildContribution( );
	//�n�D�}�Ҥ��|�ө�
	int		OpenGuildShop( );
	//��s�ݤ��|�ܦ��������|
	int		SetGuildReady( int GItemID );
	//�W�[���|�� 0 ���ż� 1 ���� 
	//int		AddGuildValue( int GItemID , int Type , int Value , int Score );
	//���o�ۤv�����|���p
	//-1 �䤣�즹�H 0 �S�����| 1 �p�ݤ��| 2 �@�뤽�|
	int		GuildState( int GItemID );

	//���o���|����	-1 �N��S���|
	int		GuildRank( int GItemID );
	//���o���|�H��
	int		GuildMemberCount( int GItemID );

	//���o�Y�H���|���
	int		GuildInfo( int GItemID , int Type );

	//�ˬd�O�_�b�ۤv�����|����
	bool	CheckInMyGuildHouse( int PlayerGUID );
	//--------------------------------------------------------------------------------
	//�x�s�����I	
	bool	SaveHomePoint( int GItemID );
	//�ǰe�^�����I	
	bool	GoHomePoint( int GItemID );
	//--------------------------------------------------------------------------------
	//�}�Ҽֳz�m�I������
	bool	OpenExchangeLottery( );
	//�}���ʶR�ֳz�m����
	bool	OpenBuyLottery( );
	 
	bool	OpenRare3EqExchangeItem();
	//--------------------------------------------------------------------------------
	//�}��Client�ݤ���
	bool	OpenClientMenu( int GItemID , int Type );
	//--------------------------------------------------------------------------------
	//////////////////////////////////////////////////////////////////////////
	//�ЫΨt��
	//////////////////////////////////////////////////////////////////////////
	//�R�Фl
	bool	BuyHouse( int GItemID , int Type );
	//�W�[�Ыί�q
	bool	AddHouseEnergy( int GItemID , int Energy );
	//////////////////////////////////////////////////////////////////////////
	//�]���O
	//0 �ۤv 1 �ϰ� 2 ���������a
	bool	RunningMsg( int GItemID , int Type , const char* Msg );
	bool	RunningMsgEx( int GItemID , int Type , int EventType , const char* Msg );
	//////////////////////////////////////////////////////////////////////////
	//�x�s�^���I
	bool	SaveReturnPos( int GItemID , int ZoneID , float X , float Y , float Z , float Dir );
	bool	SaveReturnPos_ZoneDef( int GItemID );

	//�Ǧ^�^���I
	bool	GoReturnPos( int GItemID );
	bool	GoReturnPosByNPC( int GItemID , int PlayerID );
	//////////////////////////////////////////////////////////////////////////
	//�إߪ��a�p��
	bool	BuildHouse( int GItemID , int HouseType );

	//�n�D�}�Ҫ��a�p�ΰ��[����
	bool	OpenVisitHouse( bool IsVisitMyHouse );

	//�]�w���з�¾�~���Ÿ˳�
	bool	SetStandardWearEq( int GItemID );

	//�M��Magic Point �õ�TP
	bool	SetStandardClearMagicPointAndSetTP( int GItemID );

	//�]�w�Ы�����
	bool	SetHouseType( int HouseImageObj );
	int		GetHouseType( );

	//////////////////////////////////////////////////////////////////////////
	//�]�w����ϰ�
	bool	SetParalledID( int GItemID , int ParalledID );
	
	//�ˬd�ϰ�O�_�}�_
	bool	CheckZone( int ZoneID );
	int		GetZoneID( int PlayerDBID );
	//////////////////////////////////////////////////////////////////////////
	//���o�Y��쪫�~����T
	int		GetItemInfo( int GItemID , int PGType , int Pos , int ValueType );
	bool	SetItemInfo( int GItemID , int PGType , int Pos , int ValueType , int Value );
	//////////////////////////////////////////////////////////////////////////
	//���o�W�r
	const char*	GetName( int GItemID );
	//////////////////////////////////////////////////////////////////////////
	//�[���U�k�N
	bool	AssistMagic( int GItemID , int MagicBase , int Time );
	//////////////////////////////////////////////////////////////////////////
	//�ײz�Ҧ��˳�
	bool	FixAllEq	( int GItemID , int Rate );
	//////////////////////////////////////////////////////////////////////////
	//����Log
	bool	DesignLog	( int GItemID , int Type , const char* Content  );

	//////////////////////////////////////////////////////////////////////////
	//�p�⦳�X�ӪŦ�
	int		EmptyPacketCount( int GItemID );
	//�p��]�qQueue
	int		QueuePacketCount( int GItemID );
	//�ϰ�PK�X��
	void	PKFlag			( int Type );
	//////////////////////////////////////////////////////////////////////////
	//�M���}�筫���I
	void	ClearCampRevicePoint( );
	//�]�w�}�筫���I,�@�w�n�`�ǳ]�w
	bool	SetCampRevicePoint( int CampID , int ZoneID , float X , float Y , float Z , const char* LuaScript );
	//////////////////////////////////////////////////////////////////////////
	//�ˬd�O�c�O�_�پߨ�����
	bool	CheckTreasureDelete	( int GItemID );
	//////////////////////////////////////////////////////////////////////////
	//�e�H
	void	SendMail( const char* Name , const char* Title , const char* Content );
	void	SendMailEx( const char* fromName , const char* toName , const char* Title , const char* Content , int ItemID , int ItemCount , int Money );
	//////////////////////////////////////////////////////////////////////////
	//�]�w��H�����Y
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
	
	//�ˬd�G���K�X
	bool	CheckPassword( int OwnerID , const char* Password );
	//////////////////////////////////////////////////////////////////////////
	int		SysKeyValue( const char* KeyStr );
	int		LanguageType( );
	int		GetMoneyKeyValue( const char* KeyStr , int );
	//////////////////////////////////////////////////////////////////////////
	//���o�ϥΪ��~�����
	//////////////////////////////////////////////////////////////////////////
	//�R���ϥΪ����~
	bool	UseItemDestroy( );
	//���o�ϥΪ��~�o�@�[��
	int		UseItemDurable( );	
	//�ˬd�ϥΪ����~�O�_�٦b
	bool	CheckUseItem( );

	//////////////////////////////////////////////////////////////////////////
	//���|��
	//////////////////////////////////////////////////////////////////////////
	//�إߤ��|
	bool	BuildGuildHouse( int GItemID );
	//�n�D�}�� ���|�ΰ��[����
	bool	OpenVisitGuildHouse( bool IsVisitMyHouse );

	//////////////////////////////////////////////////////////////////////////
	//��������
	//////////////////////////////////////////////////////////////////////////
	bool	AddMoney( int GItemID , int ActionType , int Money );
	//�W�[�S��f��
	bool	AddSpeicalCoin( int GItemID , int ActionType , int MoneyType , int Money );
	bool	CheckSpeicalCoin( int GItemID , int MoneyType , int Money );
	bool	AddMoney_Bonus( int GItemID , int ActionType , int Money );
	bool	AddMoney_Account( int GItemID , int ActionType , int Money );
	bool	AddMoneyEx_Account( int GItemID , int ActionType , int Money , bool LockMoneyProc );
	//////////////////////////////////////////////////////////////////////////
	// AI �B�z
	//////////////////////////////////////////////////////////////////////////
	//���o����C���H�ƶq
	int		HateListCount( int GItemID );
	//���o����C�����Y�X�Ӧ�m�����
	double	HateListInfo( int GItemID , int Pos , int Type );
	//�Ȱ��ζ}��NPC AI�B�z
	bool	EnableNpcAI( int GItemID , bool Flag );
	//�M���T��� TargetID = -1 ��ܥ��M
	bool	ClearHateList( int GItemID , int TargetID );
	//�]�wMode
	bool	SetModeEx( int GItemID , int Type , bool Flag );
	//Ū��Mode
	bool	GetModeEx( int GItemID , int Type );
	//�ˬd�O�_������
	bool	CheckLine( int GItemID , float Tx , float Ty , float Tz );
	//���o�Y�I������
	float	GetHeight( float X , float Y , float Z );
	//�]�w����C�����Y�X�Ӧ�m�����
	bool	SetHateListPoint( int GItemID , int Pos , float Value );

	bool	ModifyRangeHatePoint( int GItemID , int Type , float Point );

	//////////////////////////////////////////////////////////////////////////
	//	Buff�B�z
	//////////////////////////////////////////////////////////////////////////
	//�ˬd�O�_���YBuff
	bool	CheckBuff			( int ItemGUID	, int BuffID );
	bool	CancelBuff			( int iRoleID	, int iMagicID );
	bool	CancelBuff_NoEvent	( int iRoleID	, int iMagicID );
	
	bool	AddBuff			( int ItemGUID , int BuffID , int MagicLv , int EffectTime );
	int		BuffCount		( int ItemGUID );
	int		BuffInfo		( int ItemGUID , int Pos , int Type );

	//�ק�Buff��T
	bool	ModifyBuff		( int GItemID , int BuffID , int Power , int Time );
	bool	CancelBuff_Pos	( int GItemID , int Pos );


	double		GameObjInfo_Double( int OrgObjID , const char* DB_FieldName );
	const char*	GameObjInfo_Str	( int OrgObjID , const char* DB_FieldName );
	bool		WriteGameObjInfo_Double( int OrgObjID , const char* DB_FieldName , double value );


	//--------------------------------------------------------------------------------
	bool	TouchTomb		( );
	void	ClientSkyType	( int Type );
	//--------------------------------------------------------------------------------
	//Ū���o���|�ؿv���
	int		ReadGuildBuilding	( int ItemGUID , int Type );
	bool	WriteGuildBuilding	( int ItemGUID , int Type , int Value );
	bool	AddGuildResource	( int PlayerGUID , int Gold , int BonusGold , int Mine , int Wood , int Herb , int GuildRune , int GuildStone , const char* RetRunplot );
	//--------------------------------------------------------------------------------
	//���Client�ݪ��ɶ�
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
	//�d���@��
	//////////////////////////////////////////////////////////////////////////
	//�����d�����~
	bool	GiveItem_Pet( int GItemID , int ItemOrgID , int Lv , int Range , int MaxTotalPoint , int NpcOrgID );

	//���s�p���d������
	bool	ReCalPetLv	( int GItemID , int Pos );
	//////////////////////////////////////////////////////////////////////////
	//�]�w�y�M�C��
	bool	SetHourseColor( int GItemID , int  Color1 , int Color2 , int Color3 , int Color4 );

	//�洫�˳�
	bool	SwapEQ( int GItemID , int posID );
	//////////////////////////////////////////////////////////////////////////
	// ���|�ξ�
	//////////////////////////////////////////////////////////////////////////
	//�W��|�Կn��
	bool	AddGuildWarScore( int GItemID , int Score );
	int		GetGuildWarScore( int GItemID );

	bool	AddGuildWarEnergy( int GItemID , int Value );
	int		GetGuildWarEnergy( int GItemID );

	//�W�[���|�Գ��ӤH�n�� ( Type = 0 �ˮ`�q 1 ���}�� 2 �ؿv�� 4 �R���� 5 �v���q )
	bool	AddGuildWarPlayerValue( int GItemID , int Type , int Score );
	int		GetGuildWarPlayerValue( int GItemID , int Type );

//	bool	AddGuildWarValue( int GItemID , int Type , int Value );
//	int		GetGuildWarValue( int GItemID , int Type );
	
	int		GetGuildWarScore_ByID( int RoomID , int ID );

	const char* GetGuildName_ByID( int RoomID , int ID );

	

	//bool SetMark( int GItemID , int MarkID , int StrKeyID )
	//////////////////////////////////////////////////////////////////////////
	//�R��¾�~
	bool	DelJob( int GItemID , int Job );
	//////////////////////////////////////////////////////////////////////////
	// �����R���ж�
	bool	CopyRoomMonster	( int FromRoomID , int ToRoomID );
	bool	DelRoomMonster	( int RoomID );
	//////////////////////////////////////////////////////////////////////////
	//Ū���Ҧb��m���a��id
	int		FindMapID( float X , float Y , float Z );
	//�ˬd�Ҧb��m�O�_�b���a��
	bool	CheckMapID( float X , float Y , float Z , int MapID );
	//////////////////////////////////////////////////////////////////////////
	//�W����
	bool	AttachObj( int GItemID , int WagonItemID , int Type , const char* ItemPos , const char* WagonPos );
	//�U����
	bool	DetachObj( int GItemID );

	bool	CheckAttach( int WagonItemID , int GItemID );

	bool	CheckAttachAble( int WagonItemID );
	//////////////////////////////////////////////////////////////////////////
	//���_�ثe���I�k
	bool	MagicInterrupt( int GItemID );
	//////////////////////////////////////////////////////////////////////////
	//���s�p�⨤��ƭȣ�
	bool	ReCalculate( int GItemID );
	//
	//////////////////////////////////////////////////////////////////////////
	bool	SetSmallGameMenuType	( int GItemID , int MenuID , int Type );
	bool	SetSmallGameMenuValue	( int GItemID , int MenuID , int ID , int Value );
	bool	SetSmallGameMenuStr		( int GItemID , int MenuID , int ID , const char* Content );
	//////////////////////////////////////////////////////////////////////////
	//�j�M�i�˪�����
	int		SearchTreasure			( int GItemID );
	//�˨�����
	bool	GetTreasure				( int GItemID , int PetID , int TreasureID );
	//�}��NPC AI
	bool	DisabledNpcAI			( int GItemID , bool Flag );
	//////////////////////////////////////////////////////////////////////////
	bool	StopMove				( int GItemID , bool Immediately );
	//////////////////////////////////////////////////////////////////////////
	int		PartyInstanceLv			( int GItemID );
	//�x�s�ƥ��i��
	bool	WriteInstanceArg		( int KeyID , int Value );
	//Ū���ƥ��i��
	int		ReadInstanceArg			( int RoomID, int KeyID );
	//////////////////////////////////////////////////////////////////////////
	//Ū���C���ɶ�
	int		GameTime				( );
	//////////////////////////////////////////////////////////////////////////
	//Ū���b���X��
	bool	ReadAccountFlag			( int GItemID , int Pos );
	//�g�J�b���X��
	bool	WriteAccountFlag		( int GItemID , int Pos , bool Value );
	int		ReadAccountFlagValue	( int GItemID , int Pos , int PosCount );
	bool	WriteAccountFlagValue	( int GItemID , int Pos , int PosCount , int Value );
	//////////////////////////////////////////////////////////////////////////
	//�]�wMap icon & tip
	bool	ResetMapMark			( int RoomID );
	bool	SetMapMark				( int RoomID , int MarkID , float X , float Y , float Z , const char* TipStr , int IconID );
	bool	ClsMapMark				( int RoomID , int MarkID );
	//////////////////////////////////////////////////////////////////////////
	//�]�w�ϰ��ܼ�
	bool	ResetRoomValue			( int RoomID );
	bool	SetRoomValue			( int RoomID , int KeyID , int Value );
	int		GetRoomValue			( int RoomID , int KeyID );
	bool	ClsRoomValue			( int RoomID , int KeyID );
	//////////////////////////////////////////////////////////////////////////
	//���󪺰Ѧ��I�W�ت���
	int		CreateObj_ByObjPoint	( int GItemID , int ObjID , const char* Point );
	
	//////////////////////////////////////////////////////////////////////////
	//�ƧǸ��
	void	SortList_Reset			();
	void	SortList_Push			( int KeyID , int SortValue );
	int		SortList_Sort			();
	int		SortList_Get			();
	//////////////////////////////////////////////////////////////////////////
	//�����]�w�Ȯɥi�I�i���k�N
	bool	SetExSkill				( int GItemID , int SpellerID , int SkillID , int SkillLv );
	//�R�������]�w�k�N SpellID = -1 ��ܩ����ˬd�I�k��, SkillID = -1 ��ܩ����ޯ�����
	bool	DelExSkill				( int GItemID , int SpellerID , int SkillID );

	//////////////////////////////////////////////////////////////////////////
	//�q��Client ���|�Գ̫ᵲ�G
	void	SendGuildWarFinalScore  ( int RoomID , int PlayerDBID , int PrizeType , int	Score , int OrderID );
	//////////////////////////////////////////////////////////////////////////
	//�i�J�@��
	bool	EnterWorld				( int GItemID , int WorldID , int ZoneID , int RoomID , float X , float Y , float Z , float Dir );
	//���}�Y�@��
	bool	LeaveWorld				( int GItemID );

	//////////////////////////////////////////////////////////////////////////
	//�Ʀ�]( top��s )
	bool	Billboard_TopUpdate		( int GItemID , int SortType , int Value );
	//�Ʀ�]( ��s )
	bool	Billboard_Update		( int GItemID , int SortType , int Value );
	//�Ʀ�]( �ץ� )
	bool	Billboard_Add			( int GItemID , int SortType , int Value );
	//////////////////////////////////////////////////////////////////////////
	//�}�ҧI���]�۪�����
	bool	OpenMagicStoneShop		( int MagicStoneID , int Lv , int Money1 , int Money2 );
	//////////////////////////////////////////////////////////////////////////
	//�]�w�Y��j�p
	bool	SetModelScale			( int GItemID , float Scale );
	//////////////////////////////////////////////////////////////////////////
	//lua Ĳ�oClient�ƥ�
	void	ClientEvent				( int SendID , int GItemID , int EventType , const char* Content ); 
	void	ClientEvent_Range		( int GItemID , int EventType , const char* Content ); 
	//////////////////////////////////////////////////////////////////////////
	//�k���ƭ�
	int		ReadServantValue		( int ServantID , int Type );
	bool	WriteServantValue		( int ServantID , int Type , int Value );

	bool	ReadServantFlag			( int ServantID , int Index );
	bool	WriteServantFlag		( int ServantID , int Index , bool Result );
	//////////////////////////////////////////////////////////////////////////
	//���oDB�]�w��keyvalue
	int		GetDBKeyValue			( const char* KeyStr );
	//////////////////////////////////////////////////////////////////////////
	//�ˬd�H���O�_�b�u�W
	bool	CheckOnlinePlayer		( int DBID );
	//////////////////////////////////////////////////////////////////////////
	//���o�ثe�t�ήɶ� 1900�~��ثe�L�F�X��
	int		GetNowTime();

	//DBID �� GUID �䤣��^��-1
	//int		PlayerDBIDtoGUID( int DBID );
	//////////////////////////////////////////////////////////////////////////
	//��w��q 0 �H�U��ܤ���w
	bool	LockHP( int GItemID , int HP , const char* LuaEvent );
	//////////////////////////////////////////////////////////////////////////
	//�S����
	bool	SpecialMove( int GItemID , float X , float Y , float Z , int Type );
	//////////////////////////////////////////////////////////////////////////
	//�]�w�k�N�R���ؼ�
	bool	PushMagicExplodeTarget( int TargetID );
	bool	PushMagicShootTarget( int TargetID );

	//�n�D��L�ϰ�]PCall
	bool	ZonePCall( int zoneID , const char* plotCmd );

	//////////////////////////////////////////////////////////////////////////
	//�W�߹C�� �Գ�
	bool	InitBgIndependence( int GItemID , int mode );

	//���o�̫�������ê��@��
	const char*	GetLastHidePlotStr( int OwnerID );


	//////////////////////////////////////////////////////////////////////////
	//�u�q����
	//////////////////////////////////////////////////////////////////////////
	void	ClearLineBlock( int roomID );
	void	AddLineBlock( int roomID , int id , float x1 , float y1 , float x2 , float y2 );
	bool	DelLineBlock( int roomID , int id );
	bool	CheckLineBlock( int roomID , float x1 , float y1 , float x2 , float y2 );
	
	//////////////////////////////////////////////////////////////////////////
	//���m�ƥ��ǰe����
	bool	WeekInstancesReset(  );
};
