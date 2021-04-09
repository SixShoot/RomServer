#pragma		once
#pragma		warning (disable:4786)

#include "..\Global.h"
#include "LuaPlot_Hsiang.h"
#include "LuaPlot_PublicEncounter.h"

using namespace std;

//------------------------------------------------------------------------------------s
namespace  LuaPlotClass
{
	//��l��
	bool	Init();
    bool	Release();

	//---------------------------------------------------------------------------------
	//���o�t�Φr��
	const char* GetString( const char* szKeyName );

    //---------------------------------------------------------------------------------
    //���o�@���֦��̪�ID   
    int OwnerID( );
    //---------------------------------------------------------------------------------
    //���o�@���ؼЪ�ID
    int TargetID();
    //---------------------------------------------------------------------------------
    //���ü�
    // return �p�� Value���Ʀr
    int Random( int Range );
    //---------------------------------------------------------------------------------
    //�C�L���
    void SysPrint( const char* );
    //---------------------------------------------------------------------------------
	//����ؼ�
	int	MoveTarget( int ID , int TargetID );
    //�w�I ����
    int Move( int ID , float x , float y , float z );
	//�w�I ���ʤ������w�j�M
	int MoveDirect( int ID , float x  , float y , float z );	
    //---------------------------------------------------------------------------------
    //�]�w�üƲ���
    void SetRandMove( int ID , int Range , int Freq, int Dis );
    //*********************************************************************************
    //�t�Ϋ��O
    //*********************************************************************************
    //���ͺX�l
    bool CreateFlag( int ObjID , int FlagID, float X , float Y , float Z , float Dir );
    //---------------------------------------------------------------------------------
    //���ͪ���
    int CreateObj( int ObjID , float x , float y , float z , float dir , int count );
    //�Q�κX�l�y�в��ͪ��� FlagID = -1 �N��ζü�
    int CreateObjByFlag( int ObjID , int FlagObjID , int FlagID , int count );
    //---------------------------------------------------------------------------------
    //�R������
    bool DelObj( int ItemID );
    //---------------------------------------------------------------------------------
    //�[�J�㹳����
    bool AddtoPartition( int GItemID , int RoomID );
    //---------------------------------------------------------------------------------
    //�[�J�㹳����
    bool DelFromPartition( int GItemID );
	//---------------------------------------------------------------------------------
	//�[�J�㹳����
	bool Show( int GItemID , int RoomID );
	//---------------------------------------------------------------------------------
	//�[�J�㹳����
	bool Hide( int GItemID );
    //---------------------------------------------------------------------------------
    //����@��
    bool RunPlot( int GItemID , const char* PlotName );
    //---------------------------------------------------------------------------------
    //�]�wNPC���`
    void  NPCDead( int DeadID , int KillID );
    //---------------------------------------------------------------------------------
    //�]�w����Ҧ�
    bool SetMode( int GItemID , int Mode );
	int	 GetMode( int GitemID );
    //---------------------------------------------------------------------------------
    //���]NPC���	
    //�^�ǻݭn�h�[���ɶ��~�i�H����
    int ReSetNPCInfo( int ID );

	//*********************************************************************************
	// �a�p�B�z
	//*********************************************************************************
	//�a�pĲ�o�߬d,�åB�z������
	//�^�ǬO�_Ĳ�o�a�p
	bool CheckMineEvent( );

    //*********************************************************************************
    //�ڱ� ���󪫫~
    //*********************************************************************************
    //�������~
    //�^�� 0 �N����   1 �N��?
    bool GiveBodyItem( int GItemID , int OrgID , int Count );
	bool GiveBodyItemEx( int GItemID , int OrgID , int Count , int RuneVolume , int Inherent1 , int Inherent2 , int Inherent3 , int Inherent4 , int Inherent5 );

	bool GiveBodyItem_Note( int GItemID , int OrgID , int Count , const char* Note );
	    //---------------------------------------------------------------------------------
    //�R�����W���Y���~    
    //�^�� 0 �N����   1 �N��?
	bool DelBodyItem( int GItemID , int OrgID , int Count );
    //---------------------------------------------------------------------------------
    //�p�⨭�W�Y�@�˪��~�h�֭�
    //�^�� �ƶq
    int CountBodyItem( int GItemID  , int OrgID );
    //---------------------------------------------------------------------------------
    //�p�⦹����@���h�֭� ( Bank Body EQ )
    //�^�� �ƶq
    int CountItem( int GItemID  , int OrgID );
    //---------------------------------------------------------------------------------

    //*********************************************************************************
    //�ڱ� ......
    //*********************************************************************************
    //�T��
    bool    Msg( int GItemID , const char* Str );
    //---------------------------------------------------------------------------------
    //�K�y
    bool    Tell( int SendID , int SayID , const char* Str );
	bool	SayTo( int SendID , int SayID , const char* Str );
	//---------------------------------------------------------------------------------
    //�d�� ����
    bool    Say( int SayID , const char* Str );
	bool    NpcSay( int SayID , const char* Str );
	
	//�d�� �۰ʻ��ܽd��
	bool	Yell( int SayID, const char* Str, int iRange );
	//---------------------------------------------------------------------------------
	void ScriptMessage ( int SayID , int iTarget, int iType, const char* Str, const char* szColor );
    //---------------------------------------------------------------------------------
    //���⪫����Ū��
    //�W�� : RoleName Hp Mp ....
    int     ReadRoleValue( int GItemID , int Type );
	double  ReadRoleValuefloat( int GItemID , int Type );
    //char*	ReadRoleStr( int GItemID , int Type );
    //---------------------------------------------------------------------------------
    //���⪫���Ƽg�J
    bool    WriteRoleValue( int GItemID , int Type , double Value );
	bool    AddRoleValue( int GItemID , int Type , double Value );
    //bool	WriteRoleStr( int GItemID , int Type , const char* Str );
	bool    AddExp( int GItemID , int Value );
	
    //*********************************************************************************
    //�@���]�w���O
    //*********************************************************************************
    //�]�w�@��
    //Plot ==> void	SetPlot( ID , �@��Type , �@���W�� )
    //�Ҧp ==> void SetPlot( ID , "ClientAuto" , "TestPlot" );
    //Type = "ClientAuto" , "Auto" , "Move" , "Dead" , "Range" , "Touch"
    bool SetPlot( int GItemID , const char* TypeName , const char* PlotName , int Range );
	const char* GetPlotName( int GItemID , const char* PlotType  );
    //---------------------------------------------------------------------------------
    //����Y�@��
    bool BeginPlot( int GItemID , const char* PlotName , int DelayTime );
	bool BeginMultiStringPlot( int GItemID , const char* PlotName , int DelayTime );
    //---------------------------------------------------------------------------------
    //�]�wnpc���ݼ@�����O �ΨӤ��᦬�Mnpc����@���� or ��L����
    //Plot ==> int	SetNpcPlotClass( ID , �ۭq�r�� )
    //�Ҧp ==> int  SetNpcPlotClass( 1 , "AAA" );
    // Ret = 0 (���楢��)�L������
    //	   = 1 ��?
    bool SetNpcPlotClass( int GItemID , const char* Str );	
    //---------------------------------------------------------------------------------
	//����t�κX�l�I����( Flag = false ����   ture �i���� )
	bool MoveToFlagEnabled	( int GItemID , bool Flag );
	bool ClickToFaceEnabled	( int GItemID , bool Flag );
	//---------------------------------------------------------------------------------
	//�w�Y�w�쪺���O,��NPC�¦V���a 
	bool FaceObj	( int iSouceID, int iTargetID );
	bool AdjustDir	( int iSouceID, int iAngle );
	//---------------------------------------------------------------------------------
	//�����Y����
	bool KillID		( int KillID , int DeadID );
/*
    //---------------------------------------------------------------------------------
     
    //---------------------------------------------------------------------------------
    //Ū���Y�@�X��
    bool    ReadFlag( int ItemID , int FlagID );	
    //---------------------------------------------------------------------------------
    //�g�J�Y�@�X��
    bool    WriteFlag( int ItemID , int FlagID , int On );	
    //---------------------------------------------------------------------------------
    //(Ū��)��Y�@�X�з�ƭȳB�z
    //�Ҧp ==> int ReadFlagValue( 0 , 10 , 5  )   <== �N��10 - 14 �X�ХN���Ʀr
    //return = �X�ХΥΤG�i��N���Ʀr

    int	    ReadFlagValue( int ItemID , int FlagID , int FlagCount );
    //---------------------------------------------------------------------------------
    //(�g�J)��Y�@�X�з�ƭȳB�z
    //Plot ==> int WriteFlagValue( �}�ϺX�� , �h�ֺX�Ъ�� , �n�s���Ʀr )
    //�Ҧp ==> int WriteFlagValue( ItemID , 10 , 5 , 19 )   <== �N��10 - 14 �X�ХN���Ʀr �s�J19
    //return  =  0 ����
    //		  	 1 ��?
    bool    WriteFlagValue( int ItemID , int FlagID , int FlagCount , int Value );
    */
    //---------------------------------------------------------------------------------
    //�ˬd����O�_�s�b
    //Plot ==> int CheckID( ItemID )
    //�Ҧp ==> int CheckID( 100 )   <== �ˬd100������O�_�s�b
    //return  =  0 �S��
    //		  	 1 ��
    bool	CheckID( int GItemID );
    //---------------------------------------------------------------------------------
    //���o������
    //Plot ==> int GetValue( ID , Type )
//    int     GetValue( int GItemID , int Type );
    //---------------------------------------------------------------------------------
    //�ө�
    //---------------------------------------------------------------------------------
    //�ө��]�w
    void    SetShop( int GItemID , int ShopObjID , const char* ShopClosePlot );
    //�}�_�ө�
    bool    OpenShop( );
	//
	bool	CloseShop( int GItemID );

	//�}�һȦ�
	bool    OpenBank( );
	bool    OpenBankEx( int TargetID );
	//�}�_ Mail
	bool	OpenMail( );
	bool	OpenMailEx( int TargetID );
	//�}�� AC
	bool	OpenAC( );
	bool	OpenACEx( int TargetID );
	//�}��	����
	void	OpenChangeJob( );
	//�}�_ �Ʀ�]
	void    OpenBillboard( );
	//�}�_���|�v��
	void	OpenGuildWarBid( int iPlayer, int iNpc );
	//�}�� ���X�˳�
	void	OpenCombinEQ( int PlayerGUID , int NpcGUID , int Type );
	//�}�� �@���������~
	void	OpenPlotGive( int GItemID , bool IsSort , int Count );
	//�}�� �b���I�]
	void	OpenAccountBag( );
	//�}�� �˳ƽ��
	bool	OpenEqGamble();
	//�}�ҡ@�M�˨t��
	bool	OpenSuitSkill();
	//---------------------------------------------------------------------------------
    //�԰��@��
    //---------------------------------------------------------------------------------
    //�]�w�M��Y�@�Ӫ���
    //�^�� : �j�M�쪺�ƶq
    int     SetSearchRangeInfo( int GItemID , int Range );
	int     SetSearchAllNPC( int RoomID );
	int		SetSearchAllPlayer( int RoomID );

	//���M�ж����Y�ӥ����s�ժ�����
	int		SetSearchAllNPC_ByGroupID( int RoomID , int GroupID );

    int     GetSearchResult( );

    //�]�w�Y����}�ҧ���
    bool    SetAttack( int GItemID , int TargetID );
    //��������
    bool    SetStopAttack( int GItemID );

    //�]�w�k�]�}�ҩ�����  (�԰�����)
    bool    SetEscape( int GItemID , bool Flag );

    //--------------------------------------------------------------------------------
	// ���o�X�l����m
	//Type 
	//	EM_RoleValue_X
	//	EM_RoleValue_Y
	//	EM_RoleValue_Z
	//	EM_RoleValue_Dir
	int	GetMoveFlagValue( int FlagObjID , int FlagID , int Type );

	// ���o�Y���X�l���ƶq
	int	GetMoveFlagCount( int FlagObjID );

	//�]�w��m
	bool	SetPos( int GItemID , float X , float Y , float Z , float Dir );

	//----------------------------------------------------------------------------------------
	// �}��]�w
	//----------------------------------------------------------------------------------------
	//�]�w�Y������ݰ}��
	bool	SetRoleCamp			( int GUID , const char* CampName );
	//�]�w�Y�ϰ�ҥΪ��}���T
	bool	SetZoneCamp( int OrgObjID );
	//////////////////////////////////////////////////////////////////////////
    //--------------------------------------------------------------------------------
	// �q�� Client �I�磌��e�m�ˬd���G
	void SendQuestClickObjResult( int iRoleGUID, int iResult, int iItemID, int iItemVal );

	// �Ȯɳ]�w LUA �B�z���G��
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

	// ��ܨt�ά���
	void	AddSpeakOption		( int iRoleID, int iNPCID, const char* pszOption, const char* pszLuaFunc, int iIconID );
	void	SetSpeakDetail		( int iRoleID, const char* pszDetail );
	void	LoadQuestOption		( int iRoleID );
	void	CloseSpeak			( int iRoleID );

	// ���Ȭ���
	bool	CheckQuest			( int iRoleID, int iQuestID, int iMode );

	// �@����t����
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

	// ���ʼ@������
	void	SetPosByFlag		( int iRoleID, int iFlagObjID, int iFlagID );
		// iMode = 0 ( �ӤH�ζ��� ), 1 = ( �ӤH ), 2 = ( �O�d EX: �ζ� )

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
	// �S�� ���եΫ��O
	//--------------------------------------------------------------------------------
	//�]�w���a�˳�
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
	// ����@���Ϋ��O
	//--------------------------------------------------------------------------------
	int				GetPartyID		( int, int );

	// �@���ϥ� CastBar
	int				BeginCastBar			( int iObjID, const char* pszString, int iTime, int iMotionID, int iMotionEndID, int iFlag );
	int				BeginCastBarEvent		( int iObjID, int iClickObjID, const char* pszString, int iTime, int iMotionID, int iMotionEndID, int iFlag, const char* pszLuaEvent );
	int				CheckCastBar			( int iObjID );
	void			EndCastBar				( int iObjID, int iResult );
	int				CheckCastBarStatus		( int iObjID );
	
	
	void			AttachCastMotionTool	( int iObjID, int iAttachItemID );	// ���ݭn������L BeginCastBar ����, �~�]�o�禡, �b�ϥ� BeginCast bar ��, ��W�j�����w���~, �b EndCastBar �ɩޱ�
	void			AttachTool				( int iRoleID, int iToolID, const char* cszLinkID );
	void			StartClientCountDown	( int iObjID, int iStartNumber, int iEndNumber, int iOffset,int iMode, int iShowStatusBar, const char* Str);				// �ϥ� Client �����}�l�@�ӭ˼ƭp��
	void            StopClientCountDown     ( int iObjID);																// ���� Client �����˼ƭp��


	bool			AdjustFaceDir	( int iSouceID, int iTargetID, int iAngle );
		// ���V����, �ä��\�վ㨤��
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
