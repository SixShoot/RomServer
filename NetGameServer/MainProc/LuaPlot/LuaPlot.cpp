//----------------------------------------------------------------------------------------
#include <time.h>
#include <Windows.h>
#include <Psapi.h>

#include "LuaPlot.h" 
#include "lua/LuaWrapper/LuaWrap.h"
//----------------------------------------------------------------------------------------
#include "../../netwalker_member/Net_Script/NetSrv_Script.h"
#include "../../netwalker_member/NetWakerGSrvInc.h"
#include "../FlagPosList/FlagPosList.h"
#include "../AIProc/NpcAIBase.h"
#include "../magicproc/MagicProcess.h"
#include "../partyinfolist/PartyInfoList.h"
#include <vector>
#include "../../mainproc/pathmanage/NPCMoveFlagManage.h"
//----------------------------------------------------------------------------------------
namespace  LuaPlotClass
{
	//----------------------------------------------------------------------------------------
	//共用資料
	vector< int >				SearchObjList;
	//----------------------------------------------------------------------------------------

	bool Init( )
	{
		LuaRegisterFunc( "OwnerID"	        , int()	                                , OwnerID               );
		LuaRegisterFunc( "TargetID"			, int()	                                , TargetID	            );

		LuaRegisterFunc( "Rand"			    , int(int)	                            , Random	            );
		LuaRegisterFunc( "RandRange"		, int(int,int)                          , RandRange	            );

		LuaRegisterFunc( "SysPrint"			, void(const char*)                     , SysPrint	            );
		LuaRegisterFunc( "MoveTarget"		, int(int,int)					        , MoveTarget            );
		LuaRegisterFunc( "Move"				, int(int,float,float,float)            , Move		            );
		LuaRegisterFunc( "MoveDirect"		, int(int,float,float,float)            , MoveDirect		    );

		LuaRegisterFunc( "SetRandMove"		, void(int, int, int,int)               , SetRandMove		    );

		LuaRegisterFunc( "CreateFlag"		, bool(int,int,float,float,float,float) , CreateFlag		    );
		LuaRegisterFunc( "CreateObj"		, int(int,float,float,float,float,int)  , CreateObj			    );
		LuaRegisterFunc( "CreateObjByFlag"	, int(int,int,int,int)                  , CreateObjByFlag		);
		LuaRegisterFunc( "DelObj"         	, bool( int )                           , DelObj                );
		LuaRegisterFunc( "AddtoPartition"	, bool( int,int )	                    , AddtoPartition		);
		LuaRegisterFunc( "DelFromPartition"	, bool( int )                           , DelFromPartition		);
		LuaRegisterFunc( "Show"				, bool( int,int )	                    , Show					);
		LuaRegisterFunc( "Hide"				, bool( int )                           , Hide					);

		LuaRegisterFunc( "SetMode"	        , bool( int,int )	                    , SetMode		        );
		LuaRegisterFunc( "GetMode"	        , int ( int )		                    , GetMode		        );
		LuaRegisterFunc( "SetModeEx"		, bool( int , int , bool )				, SetModeEx	);

		LuaRegisterFunc( "SetPos"	        , bool( int,float,float,float,float)	, SetPos		        );

		LuaRegisterFunc( "RunPlot"         	, bool( int , const char* )             , RunPlot       		);
		LuaRegisterFunc( "ZonePCall"       	, bool( int , const char* )             , ZonePCall       		);

		//設定NPC死亡
		LuaRegisterFunc( "NPCDead"         	, void( int , int )                      , NPCDead               );
		LuaRegisterFunc( "ReSetNPCInfo"     , int ( int )                           , ReSetNPCInfo          );


		LuaRegisterFunc( "GiveBodyItem"     , bool( int , int , int )													, GiveBodyItem       	);
		LuaRegisterFunc( "GiveBodyItemEx"   , bool( int , int , int , int , int , int , int , int , int )	            , GiveBodyItemEx       	);
		LuaRegisterFunc( "GiveBodyItem_Note", bool( int , int , int , const char* )										, GiveBodyItem_Note     );

		LuaRegisterFunc( "GiveBodyItem_EQ"  , bool( int , int , int , int )	        , GiveBodyItem_EQ      	);

		LuaRegisterFunc( "DelBodyItem"      , bool( int , int , int )		        , DelBodyItem       	);
		LuaRegisterFunc( "CountBodyItem"    , int( int , int )			            , CountBodyItem       	);
		LuaRegisterFunc( "CountItem"        , int( int , int )			            , CountItem       	    );

		LuaRegisterFunc( "DebugMsg"      	, void( int , int, const char* )		, DebugMsg 		        );
		LuaRegisterFunc( "DebugLog"      	, void( int , const char* )			    , DebugLog 		        );
		LuaRegisterFunc( "DeleteQuest"      , bool( int , int )					    , DeleteQuest	        );


		LuaRegisterFunc( "Msg"         	    , bool( int , const char* )		        , Msg      		        );
		LuaRegisterFunc( "Tell"             , bool( int , int , const char* )       , Tell       	        );
		LuaRegisterFunc( "SayTo"            , bool( int , int , const char* )       , SayTo       	        );
		LuaRegisterFunc( "Say"              , bool( int , const char* )		        , Say                  	);
		LuaRegisterFunc( "NpcSay"           , bool( int , const char* )		        , NpcSay               	);
		LuaRegisterFunc( "Yell"             , bool( int , const char*, int)	        , Yell                  );

		LuaRegisterFunc( "ReadRoleValue"    , int( int , int )			            , ReadRoleValue       	);
		LuaRegisterFunc( "ReadRoleValueFloat"    , double( int , int )			            , ReadRoleValuefloat       	);
		LuaRegisterFunc( "WriteRoleValue"   , bool( int , int , double )		    , WriteRoleValue       	);
		LuaRegisterFunc( "AddRoleValue"		, bool( int , int , double )			, AddRoleValue       	);

		LuaRegisterFunc( "AddExp"			, bool( int , int )						, AddExp		       	);

		LuaRegisterFunc( "SetPlot"          , bool( int , const char* , const char* , int ), SetPlot        );
		LuaRegisterFunc( "GetPlotName"      , const char*( int , const char* )		, GetPlotName			);
		LuaRegisterFunc( "BeginPlot"        , bool( int , const char* , int )       , BeginPlot       	    );
		LuaRegisterFunc( "SetNpcPlotClass"  , bool( int , const char* )			    , SetNpcPlotClass       );


		LuaRegisterFunc( "CheckID"          , bool( int )			                , CheckID       	    );
		//LuaRegisterFunc( "GetValue"         , int( int , int )			            , GetValue              );
		LuaRegisterFunc( "GetString"		, const char* ( const char* )			, GetString	     	    );

		LuaRegisterFunc( "SetShop"          , void( int , int , const char* )		, SetShop       	    );
		LuaRegisterFunc( "CloseShop"        , bool( int )							, CloseShop       	    );
		LuaRegisterFunc( "OpenShop"         , bool(  )			                    , OpenShop              );
		LuaRegisterFunc( "OpenEqGamble"     , bool(  )								, OpenEqGamble          );
		LuaRegisterFunc( "OpenSuitSkill"    , bool(  )								, OpenSuitSkill         );

		LuaRegisterFunc( "OpenBank"         , bool(  )			                    , OpenBank              );		
		LuaRegisterFunc( "OpenMail"         , bool(  )			                    , OpenMail              );
		LuaRegisterFunc( "OpenAC"			, bool(  )			                    , OpenAC	            );
		LuaRegisterFunc( "OpenBankEx"         , bool( int )			                , OpenBankEx            );		
		LuaRegisterFunc( "OpenMailEx"         , bool( int )			                , OpenMailEx            );
		LuaRegisterFunc( "OpenACEx"			, bool( int )			                , OpenACEx	            );

		LuaRegisterFunc( "OpenChangeJob"	, void(  )			                    , OpenChangeJob         );
		LuaRegisterFunc( "OpenBillboard"    , void(  )			                    , OpenBillboard         );
		LuaRegisterFunc( "OpenGuildWarBid"  , void( int, int  )						, OpenGuildWarBid       );
		LuaRegisterFunc( "OpenAccountBag"   , void(  )								, OpenAccountBag        );

		LuaRegisterFunc( "SetMinimapIcon"	, void( int, int )		                , SetMinimapIcon        );
		LuaRegisterFunc( "KillID"			, bool( int , int  )				    , KillID		        );

		LuaRegisterFunc( "SetParalledID"	, bool( int, int )		                , SetParalledID         );
		LuaRegisterFunc( "CheckZone"		, bool( int )						    , CheckZone		        );

		LuaRegisterFunc( "CheckTreasureDelete"	, bool(int)	                        , CheckTreasureDelete	);
		LuaRegisterFunc( "SendMail"			, void(const char*,const char*,const char*)			, SendMail	);
		LuaRegisterFunc( "SendMailEx"		, void(const char*,const char*,const char*,const char*,int,int,int )			, SendMailEx	);
		LuaRegisterFunc( "ClockOpen"		, bool(int,int,int,int,int,const char*,const char* ), ClockOpen				);
		LuaRegisterFunc( "ClockStop"		, bool(int)								, ClockStop				);
		LuaRegisterFunc( "ClockClose"		, bool(int)								, ClockClose			);
		LuaRegisterFunc( "ClockRead"		, int(int,int)							, ClockRead				);

		LuaRegisterFunc( "SysCastSpellLv"	, bool(int,int,int,int)					, SysCastSpellLv		);
		LuaRegisterFunc( "SysCastSpellLv_Pos", bool(int,float,float,float,int,int)	, SysCastSpellLv_Pos	);
		LuaRegisterFunc( "InterruptMagic"	, bool(int)								, InterruptMagic		);
		LuaRegisterFunc( "SwapEQ"			, bool(int,int)							, SwapEQ				);		

		LuaRegisterFunc( "GetNowTime"				, int()							, GetNowTime			);		
		LuaRegisterFunc( "PushMagicExplodeTarget"	, bool(int)						, PushMagicExplodeTarget	);		
		LuaRegisterFunc( "PushMagicShootTarget"		, bool(int)						, PushMagicShootTarget		);		
		LuaRegisterFunc( "InitBgIndependence"		, bool(int,int)					, InitBgIndependence		);		

		//---------------------------------------------------------------------------------
		// 地雷處理
		//---------------------------------------------------------------------------------		
		LuaRegisterFunc( "CheckMineEvent"	, bool( )								, CheckMineEvent        );
		//---------------------------------------------------------------------------------
		//戰鬥相關劇情
		//---------------------------------------------------------------------------------
		LuaRegisterFunc( "GetNumPlayer",		int()								  , GetNumPlayer    );

		LuaRegisterFunc( "SetSearchAllPlayer", int( int )			                , SetSearchAllPlayer    );
		LuaRegisterFunc( "SetSearchRangeInfo", int( int , int )	                    , SetSearchRangeInfo    );
		LuaRegisterFunc( "SetSearchAllNPC"	 , int( int )							, SetSearchAllNPC		);

		LuaRegisterFunc( "SetSearchAllNPC_ByGroupID"	 , int( int , int )			, SetSearchAllNPC_ByGroupID		);

		LuaRegisterFunc( "GetSearchResult"   , int( )	                            , GetSearchResult       );
		LuaRegisterFunc( "SetAttack"         , bool( int , int )	                , SetAttack             );
		LuaRegisterFunc( "SetStopAttack"     , bool( int )	                        , SetStopAttack         );
		//----------------------------------------------------------------------------------------
		// 陣營設定
		//----------------------------------------------------------------------------------------
		LuaRegisterFunc( "SetRoleCamp" 		, bool( int, const char* )				, SetRoleCamp			);
		LuaRegisterFunc( "SetZoneCamp"		, void( int )							, SetZoneCamp			);

		//----------------------------------------------------------------------------------------
		// 劇情相關
		//----------------------------------------------------------------------------------------
		LuaRegisterFunc( "PlayMotion",				void( int, int  ),					PlayMotion				);
		LuaRegisterFunc( "PlayMotion_Self",			void( int, int , int ),				PlayMotion_Self			);
		LuaRegisterFunc( "PlayMotionEx",			void( int, int , int ),				PlayMotionEx			);
		LuaRegisterFunc( "SetPosture",				void( int, int, int ),				SetPosture				);
		LuaRegisterFunc( "SetIdleMotion",			void( int, int ),					SetIdleMotion			);
		LuaRegisterFunc( "SetDefIdleMotion",		void( int, int ),					SetDefIdleMotion		);


		LuaRegisterFunc( "SetFightMode",			void( int, int, int, int, int   ),	SetFightMode			);

		LuaRegisterFunc( "AdjustDir",				bool( int, int   ),					AdjustDir				);
		LuaRegisterFunc( "FaceObj",					bool( int, int   ),					FaceObj					);
		LuaRegisterFunc( "FaceFlag",				void( int, int, int   ),			FaceFlag				);
		LuaRegisterFunc( "MoveToFlagEnabled",		bool( int, bool ),					MoveToFlagEnabled		);
		LuaRegisterFunc( "ClickToFaceEnabled",		bool( int, bool ),					ClickToFaceEnabled		);
		//LuaRegisterFunc( "SetNpcNearPlayer",		bool( int, int ),					SetNpcNearPlayer		);
		LuaRegisterFunc( "CastSpell",				bool( int, int, int ),				CastSpell				);
		LuaRegisterFunc( "CastSpellLv",				bool( int, int, int , int),			CastSpellLv				);
		LuaRegisterFunc( "CastSpellPos",			bool( int , float , float , float , int , int ) ,		CastSpellPos				);
		LuaRegisterFunc( "CancelBuff",				bool( int, int ),					CancelBuff				);
		LuaRegisterFunc( "CancelBuff_NoEvent",		bool( int, int ),					CancelBuff_NoEvent		);

		LuaRegisterFunc( "ModifyBuff",				bool( int, int , int , int ),		ModifyBuff				);
		LuaRegisterFunc( "CancelBuff_Pos",			bool( int, int ),					CancelBuff_Pos			);

		LuaRegisterFunc( "PlaySound",				int( int, const char*, bool ),		PlaySound				);
		LuaRegisterFunc( "PlaySoundToPlayer",		int( int, const char*, bool ),		PlaySoundToPlayer		);

		LuaRegisterFunc( "PlayMusic",				int( int, const char*, bool ),		PlayMusic				);
		LuaRegisterFunc( "PlayMusicToPlayer",		int( int, const char*, bool ),		PlayMusicToPlayer		);

		LuaRegisterFunc( "Play3DSound",				int( int, const char*, bool ),		Play3DSound				);
		LuaRegisterFunc( "Play3DSoundToPlayer",		int( int, const char*, bool ),		Play3DSoundToPlayer		);

		LuaRegisterFunc( "StopSound",				void( int, int ),					StopSound				);

		LuaRegisterFunc( "MoveToPathPoint",			void( int, int ),					MoveToPathPoint			);
		LuaRegisterFunc( "ScriptMessage",			void( int, int, int, const char*, const char* ),	ScriptMessage			);
		LuaRegisterFunc( "LockObj",					void( int, int, int ),				LockObj					);
		LuaRegisterFunc( "SetFollow",				void( int, int ),					SetFollow				);
		LuaRegisterFunc( "SetCursorType",			void( int, int ),					SetCursorType			);
		LuaRegisterFunc( "SetDelayPatrolTime",		void( int, int ),					SetDelayPatrolTime			);

		//----------------------------------------------------------------------------------------
		LuaRegisterFunc( "GetMoveFlagValue"	,		int( int , int , int ),				GetMoveFlagValue      );
		LuaRegisterFunc( "GetMoveFlagCount"	,		int( int ),							GetMoveFlagCount      );
		//----------------------------------------------------------------------------------------
		LuaRegisterFunc( "SendQuestClickObjResult", void( int, int, int, int ),			SendQuestClickObjResult		);
		LuaRegisterFunc( "SetQuestState",			void( int, int, int ),				SetQuestState		);
		LuaRegisterFunc( "GetQuestState",			int( int, int ),					GetQuestState		);
		//----------------------------------------------------------------------------------------
		LuaRegisterFunc( "CallSpeakLua",			void( int, int, const char* ),		CallSpeakLua		);
		//----------------------------------------------------------------------------------------
		//讀寫女僕數值
		LuaRegisterFunc( "ReadServantValue",		int( int, int ),					ReadServantValue	);
		LuaRegisterFunc( "WriteServantValue",		bool( int, int, int ),				WriteServantValue	);

		LuaRegisterFunc( "ReadServantFlag",			bool( int, int ),					ReadServantFlag		);
		LuaRegisterFunc( "WriteServantFlag",		bool( int, int, bool ),				WriteServantFlag	);


		// SCRIPT 時間條
		//----------------------------------------------------------------------------------------
		LuaRegisterFunc( "BeginCastBar",			int( int, const char*, int, int, int, int  ),	BeginCastBar		);
		LuaRegisterFunc( "BeginCastBarEvent",		int( int, int, const char*, int, int, int, int, const char*  ),	BeginCastBarEvent		);
		LuaRegisterFunc( "CheckCastBar",			int( int ),							CheckCastBar		);
		LuaRegisterFunc( "EndCastBar",				void( int, int ),					EndCastBar			);
		LuaRegisterFunc( "CheckCastBarStatus",		int( int ),							CheckCastBarStatus	);

		//----------------------------------------------------------------------------------------
		 
		//----------------------------------------------------------------------------------------
		LuaRegisterFunc( "DisableQuest",			void( int, bool ),					DisableQuest				);		
		LuaRegisterFunc( "SetScriptFlag",			void( int, int, int ),				SetScriptFlag			);
		LuaRegisterFunc( "CheckScriptFlag",			bool( int, int ),					CheckScriptFlag			);
		LuaRegisterFunc( "SetFlag",					void( int, int, int ),				SetScriptFlag			);
		LuaRegisterFunc( "CheckFlag",				bool( int, int ),					CheckScriptFlag			);
		LuaRegisterFunc( "CheckCardFlag",			bool( int, int ),					CheckCardFlag			);

		LuaRegisterFunc( "CheckCompleteQuest",		bool( int, int ),					CheckCompleteQuest		);
		LuaRegisterFunc( "CheckAcceptQuest",		bool( int, int ),					CheckAcceptQuest		);

		LuaRegisterFunc( "GetDistance",				int( int, int ),					GetDistance				);
		LuaRegisterFunc( "CheckDistance",			bool( int, int, int ),				CheckDistance			);
		//----------------------------------------------------------------------------------------
		// 對話系統相關
		//----------------------------------------------------------------------------------------
		LuaRegisterFunc( "AddSpeakOption",			void( int, int, const char*, const char*, int ),	AddSpeakOption	);
		LuaRegisterFunc( "SetSpeakDetail",			void( int, const char* ),							SetSpeakDetail	);
		LuaRegisterFunc( "LoadQuestOption",			void( int ),										LoadQuestOption	);
		LuaRegisterFunc( "CloseSpeak",				void( int ),										CloseSpeak		);
		//----------------------------------------------------------------------------------------
		// 任務系統相關
		//----------------------------------------------------------------------------------------
		LuaRegisterFunc( "CheckQuest",				bool( int, int, int ),				CheckQuest	);	

		//----------------------------------------------------------------------------------------
		// 劇情移動相關
		//----------------------------------------------------------------------------------------
		LuaRegisterFunc( "SetPosByFlag",			void( int, int, int ),				SetPosByFlag	);
		//--------------------------------------------------------------------------------
		// 特殊 測試用指令
		//--------------------------------------------------------------------------------
		LuaRegisterFunc( "SetRoleEq_Player",		bool( int, int, int ),				SetRoleEq_Player	);
		LuaRegisterFunc( "SetRoleEquip",			void( int, int, int, int ),			SetRoleEquip			);
		//--------------------------------------------------------------------------------
		// 石碑介面指令 SciptBorder
		//--------------------------------------------------------------------------------
		LuaRegisterFunc( "GetUseItemGUID",			int( int ),							GetUseItemGUID					);
		LuaRegisterFunc( "GetObjNameByGUID",		const char*( int ),					GetObjNameByGUID				);
		LuaRegisterFunc( "GetQuestDesc",			const char*( int ),					GetQuestDesc					);
		LuaRegisterFunc( "GetQuestDetail",			const char*( int, int ),			GetQuestDetail					);

		LuaRegisterFunc( "ClearBorder",				void( int ),						ClearBorder						);
		LuaRegisterFunc( "AddBorderPage",			void( int, const char* ),			AddBorderPage					);
		LuaRegisterFunc( "ShowBorder",				void( int, int, const char*, const char* ),						ShowBorder					);
		//LuaRegisterFunc( "SetBorderAcceptQuestLua",	void( int ),						SetBorderAcceptQuestLua			);

		//--------------------------------------------------------------------------------
		// 隊伍劇情指令
		//--------------------------------------------------------------------------------
		LuaRegisterFunc( "GetPartyID",				int( int, int ),					GetPartyID					);

		//--------------------------------------------------------------------------------
		// WorldVar
		//--------------------------------------------------------------------------------
		LuaRegisterFunc( "SetWorldVar",				void( const char*, int ),			SetWorldVar					);
		LuaRegisterFunc( "GetWorldVar",				int( const char* ),					GetWorldVar					);

		//--------------------------------------------------------------------------------
		// 劇情表
		//--------------------------------------------------------------------------------
		//		Init_Hsiang();
		LuaRegisterFunc( "ItemSavePos"					, bool( int )												, ItemSavePos			);
		LuaRegisterFunc( "ItemTransportPos"				, bool(  )													, ItemTransportPos		);
		LuaRegisterFunc( "CheckItemTransportPos"		, bool(  )													, CheckItemTransportPos );

		LuaRegisterFunc( "CheckBuff"					, bool( int , int )											, CheckBuff				);

		LuaRegisterFunc( "SetDir"						, bool( int , float )										, SetDir				);
		LuaRegisterFunc( "CalDir"						, float( float , float)										, CalDir				);
		LuaRegisterFunc( "AdjustFaceDir"				, bool( int, int, int )										, AdjustFaceDir			);

		LuaRegisterFunc( "ResetColdown"					, bool( int )												, ResetColdown			);
		LuaRegisterFunc( "ChangeZone"					, bool( int , int , int , float , float , float , float )	, ChangeZone			);


		LuaRegisterFunc( "DialogCreate"					, bool( int,int,const char*)								, DialogCreate			);
		LuaRegisterFunc( "DialogSelectStr"				, bool( int,const char*)									, DialogSelectStr		);
		LuaRegisterFunc( "DialogSendOpen"				, bool( int )												, DialogSendOpen		);
		LuaRegisterFunc( "DialogSetContent"				, bool( int,const char*)									, DialogSetContent		);
		LuaRegisterFunc( "DialogGetResult"				, int( int )												, DialogGetResult		);
		LuaRegisterFunc( "DialogGetInputResult"			, const char* ( int )										, DialogGetInputResult	);
		LuaRegisterFunc( "DialogGetInputCheckResult"	, int ( int )												, DialogGetInputCheckResult	);

		LuaRegisterFunc( "DialogClose"					, bool( int )												, DialogClose			);
		LuaRegisterFunc( "DialogSetTitle"				, void( int,const char*)									, DialogSetTitle		);

		

		//---------------------------------------------------------------------------------
		LuaRegisterFunc( "SetLook"						, bool( int,int,int,int,int )								, SetLook				);

		LuaRegisterFunc( "SetCardFlag"					, bool( int,int,bool )										, SetCardFlag			);
		LuaRegisterFunc( "GetCardFlag"					, bool( int,int )											, GetCardFlag			);
		//---------------------------------------------------------------------------------
		LuaRegisterFunc( "OpenCreateNewGuild"			, int( )													, OpenCreateNewGuild	);
		LuaRegisterFunc( "OpenGuildContribution"		, int( )													, OpenGuildContribution	);
		LuaRegisterFunc( "SetGuildReady"				, int( int )												, SetGuildReady			);
		//		LuaRegisterFunc( "AddGuildValue"				, int( int,int,int,int )									, AddGuildValue			);
		LuaRegisterFunc( "GuildRank"					, int( int )												, GuildRank				);
		LuaRegisterFunc( "GuildMemberCount"				, int( int )												, GuildMemberCount		);
		LuaRegisterFunc( "GuildState"					, int( int )												, GuildState			);
		LuaRegisterFunc( "OpenGuildShop"				, int( )													, OpenGuildShop			);
		LuaRegisterFunc( "GuildInfo"					, int( int , int )											, GuildInfo				);
		LuaRegisterFunc( "GetGuildName"					, const char*( int )										, GetGuildName			);
		LuaRegisterFunc( "AddGuildResource"				, bool( int,int,int,int,int,int,int,int, const char* )		, AddGuildResource		);
		LuaRegisterFunc( "CheckInMyGuildHouse"			, bool(int)													, CheckInMyGuildHouse	);

		LuaRegisterFunc( "ReadGuildBuilding"			, int( int , int )											, ReadGuildBuilding		);
		LuaRegisterFunc( "WriteGuildBuilding"			, int( int , int , int )									, WriteGuildBuilding	);
		//---------------------------------------------------------------------------------
		LuaRegisterFunc( "SaveHomePoint"				, bool( int )												, SaveHomePoint			);
		LuaRegisterFunc( "GoHomePoint"					, bool( int )												, GoHomePoint			);
		//---------------------------------------------------------------------------------
		LuaRegisterFunc( "OpenExchangeLottery"			, bool( )													, OpenExchangeLottery	);
		LuaRegisterFunc( "OpenBuyLottery"				, bool( )													, OpenBuyLottery		);
		LuaRegisterFunc( "OpenRare3EqExchangeItem"		, bool( )													, OpenRare3EqExchangeItem);
		//--------------------------------------------------------------------------------
		LuaRegisterFunc( "OpenClientMenu"				, bool( int , int )											, OpenClientMenu		);
		LuaRegisterFunc( "RunningMsg"					, bool( int , int , const char* )							, RunningMsg			);
		LuaRegisterFunc( "RunningMsgEx"					, bool( int , int , int , const char* )						, RunningMsgEx			);

		LuaRegisterFunc( "SaveReturnPos"				, bool( int , int , float , float , float , float  )		, SaveReturnPos			);
		LuaRegisterFunc( "SaveReturnPos_ZoneDef"		, bool( int )												, SaveReturnPos_ZoneDef			);
		LuaRegisterFunc( "GoReturnPos"					, bool( int )												, GoReturnPos			);
		LuaRegisterFunc( "GoReturnPosByNPC"				, bool( int , int )											, GoReturnPosByNPC			);
		LuaRegisterFunc( "OpenVisitHouse"				, bool( bool )												, OpenVisitHouse		);
		LuaRegisterFunc( "BuildHouse"					, bool( int , int )											, BuildHouse			);
		LuaRegisterFunc( "AddHouseEnergy"				, bool( int , int )											, AddHouseEnergy		);
		LuaRegisterFunc( "SetHouseType"					, bool( int )												, SetHouseType			);
		LuaRegisterFunc( "GetHouseType"					, int( )													, GetHouseType			);
		
		//--------------------------------------------------------------------------------
		LuaRegisterFunc( "GenerateMine"					, void( int, int , int, int )								, GenerateMine			);
		LuaRegisterFunc( "SetCrystalID"					, void( int, int )											, SetCrystalID			);

		LuaRegisterFunc( "SetStandardWearEq"			, bool( int )												, SetStandardWearEq		);
		LuaRegisterFunc( "SetStandardClearMagicPointAndSetTP"	, bool( int )										, SetStandardClearMagicPointAndSetTP		);

		LuaRegisterFunc( "GetItemInfo"					, int(int,int,int,int)										, GetItemInfo			);
		LuaRegisterFunc( "SetItemInfo"					, int(int,int,int,int,int)									, SetItemInfo			);
		LuaRegisterFunc( "OpenCombinEQ"					, void(int,int,int)											, OpenCombinEQ			);
		LuaRegisterFunc( "GetName"						, const char*(int)											, GetName				);
		LuaRegisterFunc( "AssistMagic"					, bool( int , int , int )									, AssistMagic			);
		LuaRegisterFunc( "FixAllEq"						, bool( int , int )											, FixAllEq				);
		LuaRegisterFunc( "DesignLog"					, bool( int,int,const char* )								, DesignLog				);

		LuaRegisterFunc( "GetLocaltime"					, int ( int, int )											, GetLocaltime			);
		LuaRegisterFunc( "GetSystime"					, int ( int )												, GetSystime			);

		LuaRegisterFunc( "EmptyPacketCount"				, int ( int )												, EmptyPacketCount		);
		LuaRegisterFunc( "QueuePacketCount"				, int ( int )												, QueuePacketCount		);

		LuaRegisterFunc( "PKFlag"						, void ( int )												, PKFlag				);

		LuaRegisterFunc( "OpenPlotGive"					, void ( int , bool , int )									, OpenPlotGive			);
		LuaRegisterFunc( "GetBodyFreeSlot"				, int ( int )												, GetBodyFreeSlot		);

		LuaRegisterFunc( "ClearCampRevicePoint"			, void ( void )												, ClearCampRevicePoint	);
		LuaRegisterFunc( "SetCampRevicePoint"			, bool( int,int,float,float,float,const char*) 				, SetCampRevicePoint	);

		LuaRegisterFunc( "ResetObjDailyQuest"			, void( int )												, ResetObjDailyQuest	);
		LuaRegisterFunc( "GetDailyQuestCount"			, int( int )												, GetDailyQuestCount	);


		LuaRegisterFunc( "SetBattleGroundObjClickState"	, void( int, int, int )											, SetBattleGroundObjClickState	);
		LuaRegisterFunc( "SetRoleCampID"				, bool( int, int )											, SetRoleCampID	);
		LuaRegisterFunc( "GetRoleCampID"				, int( int )												, GetRoleCampID	);
		LuaRegisterFunc( "ResetRoleCampID"				, bool( int )												, ResetRoleCampID	);
		LuaRegisterFunc( "BeginMultiStringPlot"			, bool( int, const char*, int )										, BeginMultiStringPlot	);
		LuaRegisterFunc( "ChangeObjID"					, void( int, int )											, ChangeObjID	);

		LuaRegisterFunc( "DelJob"						, bool( int,int)											, DelJob	);
		LuaRegisterFunc( "GetBattleGroundRoomID"		, int( int)													, GetBattleGroundRoomID	);
		// BattleGround
		//-----------------------------------------------------------------------------------------------------------------------
		
		LuaRegisterFunc( "GetScore"						, int( int, int )											, GetScore	);
		LuaRegisterFunc( "SetScore"						, void( int, int, int  )									, SetScore	);
		LuaRegisterFunc( "GetLandMark"					, int( int, int )											, GetLandMark	);
		LuaRegisterFunc( "SetLandMark"					, void( int, int, int  )									, SetLandMark	);		
		LuaRegisterFunc( "GetVar"						, int( int, int )											, GetVar		);
		LuaRegisterFunc( "SetVar"						, void( int, int, int  )									, SetVar		);		
		LuaRegisterFunc( "SetLandMarkInfo"				, void( int, int, int, float, float, float, const char*, int)									, SetLandMarkInfo	);

		LuaRegisterFunc( "GetTeamID"					, int( int )												, GetTeamID	);
		LuaRegisterFunc( "SendBGInfoToClient"			, void( int, int )											, SendBGInfoToClient	);

		LuaRegisterFunc( "GetNumTeam"					, int( void)												, GetNumTeam		);
		LuaRegisterFunc( "GetNumQueueMember"			, int( int, int )											, GetNumQueueMember	);
		

		LuaRegisterFunc( "SetBattleGroundObjCampID"			, void( int, int )										, SetBattleGroundObjCampID		);
		LuaRegisterFunc( "SetBattleGroundObjForceFlagID"	, void( int, int )										, SetBattleGroundObjForceFlagID	);
		LuaRegisterFunc( "SetBattleGroundObjGUIDForceFlag"	, void( int, int )										, SetBattleGroundObjGUIDForceFlag	);
		LuaRegisterFunc( "MoveRoleToBattleGround"			, void( int, int, int, int, float, float, float, float ), MoveRoleToBattleGround	);

		LuaRegisterFunc( "GetNumTeamMember"					, int( int,int )										, GetNumTeamMember	);
		LuaRegisterFunc( "GetDisconnectTeamMember"			, int( int, int )										, GetDisconnectTeamMember	);
		LuaRegisterFunc( "GetNumEnterTeamMember"			, int( int, int )										, GetNumEnterTeamMember	);
		LuaRegisterFunc( "GetMaxTeamMember"					, int( void )											, GetMaxTeamMember	);
	
		LuaRegisterFunc( "Revive"							, void( int, int, float, float, float, float )			, Revive	);

		
		LuaRegisterFunc( "AddTeamItem"						, void( int, int, int, int )							, AddTeamItem	);
		LuaRegisterFunc( "GetGUIDByDBID"					, int( int )											, GetGUIDByDBID	);

		LuaRegisterFunc( "GetEnterTeamMemberInfo"			, int( int, int, int )									, GetEnterTeamMemberInfo	);
		LuaRegisterFunc( "JoinBattleGround"					, void( int, int, int, int )							, JoinBattleGround			);
		LuaRegisterFunc( "JoinBattleGroundWithTeam"			, void( int, int )										, JoinBattleGroundWithTeam			);

		LuaRegisterFunc( "LeaveBattleGround"				, void( int	)											, LeaveBattleGround			);

		LuaRegisterFunc( "AddHonor"							, void( int, int )										, AddHonor			);
		LuaRegisterFunc( "AddTeamHonor"						, void( int, int, int )									, AddTeamHonor		);

		LuaRegisterFunc( "AddRankPoint"						, void( int, int )										, AddRankPoint		);
		LuaRegisterFunc( "AddTeamRankPoint"					, void( int, int, int )									, AddTeamRankPoint	);
		//-----------------------------------------------------------------------------------------------------------------------

		LuaRegisterFunc( "SetZoneReviveScript"				, void( const char*  )									, SetZoneReviveScript	);
		LuaRegisterFunc( "SetZonePVPScript"					, void( const char*  )									, SetZonePVPScript	);
		LuaRegisterFunc( "EndBattleGround"					, void( int, int )										, EndBattleGround );
		LuaRegisterFunc( "CloseBattleGround"				, void( int )											, CloseBattleGround );

		LuaRegisterFunc( "GetZoneID"						, int( int )											, GetZoneID	);
		LuaRegisterFunc( "CheckPassword"					, bool( int , const char* )								, CheckPassword	);
		LuaRegisterFunc( "CheckOnlinePlayer"				, bool( int )											, CheckOnlinePlayer	);
		//---------------------------------------------------------------------------------
		//設定關係
		LuaRegisterFunc( "SetRelation"					, bool( int , int , int , float , int , float )				, SetRelation	);
		LuaRegisterFunc( "GetRelation"					, int( int , int )											, GetRelation	);
		LuaRegisterFunc( "GetRelationLv"				, float( int , int )										, GetRelationLv	);
		LuaRegisterFunc( "SetRelationLv"				, bool( int , int , float )									, SetRelationLv	);

		LuaRegisterFunc( "GetRelation_DBID"				, int( int , int )											, GetRelation_DBID		);
		LuaRegisterFunc( "GetRelation_Name"				, const char*( int , int )									, GetRelation_Name		);
		LuaRegisterFunc( "GetRelation_Lv"				, float( int , int )										, GetRelation_Lv		);
		LuaRegisterFunc( "GetRelation_Relation"			, int( int , int )											, GetRelation_Relation	);

		LuaRegisterFunc( "SetRelation_Lv"				, bool( int , int , float )									, SetRelation_Lv	);
		LuaRegisterFunc( "SetRelation_Relation"			, bool( int , int , int )									, SetRelation_Relation	);
		LuaRegisterFunc( "CallPlayer_DBID"				, bool( int , int )											, CallPlayer_DBID	);
		LuaRegisterFunc( "GotoPlayer_DBID"				, bool( int , int )											, GotoPlayer_DBID	);

		LuaRegisterFunc( "AddSkillValue"				, bool(int,int,float)										, AddSkillValue	);
		LuaRegisterFunc( "AddRecipe"					, void(int,int)												, AddRecipe	);
		LuaRegisterFunc( "GetRecipeList"				, void(int,int)												, GetRecipeList	);
		LuaRegisterFunc( "SetCustomTitleString"			, bool(int,const char*)										, SetCustomTitleString	);

		LuaRegisterFunc( "SysKeyValue"					, int(const char*)											, SysKeyValue	);
		LuaRegisterFunc( "LanguageType"					, int( )													, LanguageType	);
		LuaRegisterFunc( "GetMoneyKeyValue"				, int(const char*,int)										, GetMoneyKeyValue	);

		LuaRegisterFunc( "GetServerDataLanguage"		, const char*()												, GetServerDataLanguage		);
		LuaRegisterFunc( "GetServerStringLanguage"		, const char*()												, GetServerStringLanguage	);

		LuaRegisterFunc( "GetLastHidePlotStr"			, const char*(int)											, GetLastHidePlotStr	);
		//////////////////////////////////////////////////////////////////////////
		//取得使用物品的資料
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "UseItemDestroy"				, bool( )													, UseItemDestroy	);
		LuaRegisterFunc( "UseItemDurable"				, int( )													, UseItemDurable	);
		LuaRegisterFunc( "CheckUseItem"					, bool( )													, CheckUseItem	);
		//////////////////////////////////////////////////////////////////////////
		//公會屋
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "BuildGuildHouse"				, bool( int )												, BuildGuildHouse	);
		LuaRegisterFunc( "OpenVisitGuildHouse"			, bool( bool )												, OpenVisitGuildHouse	);
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "AddBuff"						, bool( int, int, int, int )								, AddBuff	);
		LuaRegisterFunc( "BuffCount"					, int( int )												, BuffCount	);
		LuaRegisterFunc( "BuffInfo"						, int( int,int,int )										, BuffInfo	);

		LuaRegisterFunc( "WriteGameObjInfo_Double"		, bool( int , const char* , double)							, WriteGameObjInfo_Double);
		LuaRegisterFunc( "GameObjInfo_Double"			, double( int , const char* )								, GameObjInfo_Double);
		LuaRegisterFunc( "GameObjInfo_Int"				, double( int , const char* )								, GameObjInfo_Double);
		LuaRegisterFunc( "GameObjInfo_Str"				, const char*( int , const char* )							, GameObjInfo_Str	);
		//////////////////////////////////////////////////////////////////////////
		// AI 處理
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "HateListCount"				, int( int )												, HateListCount		);
		LuaRegisterFunc( "HateListInfo"					, double( int ,int,int)										, HateListInfo		);
		LuaRegisterFunc( "EnableNpcAI"					, bool( int,bool )											, EnableNpcAI		);
		LuaRegisterFunc( "ClearHateList"				, bool( int,int )											, ClearHateList		);
		LuaRegisterFunc( "SetModeEx"					, bool( int,int,bool )										, SetModeEx			);
		LuaRegisterFunc( "GetModeEx"					, bool( int,int )											, GetModeEx			);
		LuaRegisterFunc( "CheckLine"					, bool( int,float,float,float )								, CheckLine			);
		LuaRegisterFunc( "GetHeight"					, float( float,float,float  )								, GetHeight			);
		LuaRegisterFunc( "SetHateListPoint"				, bool( int , int , float )									, SetHateListPoint	);
		LuaRegisterFunc( "ModifyRangeHatePoint"			, bool( int , int , float )									, ModifyRangeHatePoint	);

		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "CheckBit"						, bool( int , int )											, CheckBit			);
		LuaRegisterFunc( "SetBitValue"						, int( int, int, int )										, SetBitValue	);
		LuaRegisterFunc( "CheckRelation"				, bool( int , int , int )									, CheckRelation		);
		LuaRegisterFunc( "TouchTomb"					, bool( )													, TouchTomb			);
		LuaRegisterFunc( "ClientSkyType"				, void( int )												, ClientSkyType		);
		//////////////////////////////////////////////////////////////////////////
		//給予金錢
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "AddMoney"						, bool( int,int,int )										, AddMoney				);
		LuaRegisterFunc( "AddMoney_Bonus"				, bool( int,int,int )										, AddMoney_Bonus		);
		LuaRegisterFunc( "AddMoney_Account"				, bool( int,int,int )										, AddMoney_Account		);
		LuaRegisterFunc( "AddMoneyEx_Account"			, bool( int,int,int,bool )									, AddMoneyEx_Account	);
		LuaRegisterFunc( "AddSpeicalCoin"				, bool( int,int,int,int )									, AddSpeicalCoin		);
		LuaRegisterFunc( "CheckSpeicalCoin"				, bool( int,int,int )										, CheckSpeicalCoin		);

		LuaRegisterFunc( "AttachCastMotionTool"			, void( int,int )											, AttachCastMotionTool	);
		LuaRegisterFunc( "AttachTool"					, void( int,int,const char* )								, AttachTool			);

		LuaRegisterFunc( "StartClientCountDown"			, void( int,int,int,int, int, int, const char*)				, StartClientCountDown	);
		LuaRegisterFunc( "StopClientCountDown"			, void( int )												, StopClientCountDown	);

		LuaRegisterFunc( "SetArenaScore"				, void( int,int,int,int,float )								, SetArenaScore		);
		LuaRegisterFunc( "GetArenaScore"				, float( int,int,int,int )									, GetArenaScore		);

		LuaRegisterFunc( "GetNumTeamMember"				, int( int,int )											, GetNumTeamMember	);
		LuaRegisterFunc( "SetMemberVar"					, void( int, int,int,int, int )								, SetMemberVar		);
		LuaRegisterFunc( "GetMemberVar"					, int( int, int,int, int )									, GetMemberVar		);

		LuaRegisterFunc( "GetWorldQueueRule"			, int()														, GetWorldQueueRule		);
		//////////////////////////////////////////////////////////////////////////
		//寵物劇情
		LuaRegisterFunc( "GiveItem_Pet"					, bool( int,int,int,int,int,int )							, GiveItem_Pet		);
		LuaRegisterFunc( "SetHourseColor"				, bool( int,int,int,int,int )								, SetHourseColor	);

		LuaRegisterFunc( "ReCalPetLv"					, bool( int, int )											, ReCalPetLv	);


		LuaRegisterFunc( "CopyRoomMonster"				, bool( int,int)											, CopyRoomMonster	);
		LuaRegisterFunc( "DelRoomMonster"				, bool( int)												, DelRoomMonster	);

		LuaRegisterFunc( "FindMapID"					, int( float , float , float )								, FindMapID			);
		LuaRegisterFunc( "CheckMapID"					, bool( float , float , float , int )						, CheckMapID		);

		LuaRegisterFunc( "CheckAttachAble"				, bool( int )												, CheckAttachAble			);
		LuaRegisterFunc( "CheckAttach"					, bool( int , int )											, CheckAttach			);
		LuaRegisterFunc( "AttachObj"					, bool( int , int , int , const char* , const char* )		, AttachObj			);
		LuaRegisterFunc( "DetachObj"					, bool( int )												, DetachObj			);

		LuaRegisterFunc( "MagicInterrupt"				, bool( int )												, MagicInterrupt	);
		LuaRegisterFunc( "ReCalculate"					, bool( int )												, ReCalculate		);
//		LuaRegisterFunc( "PlayerDBIDtoGUID"				, int( int )												, PlayerDBIDtoGUID		);
		//////////////////////////////////////////////////////////////////////////
		//公會戰
		LuaRegisterFunc( "AddGuildWarScore"				, bool( int,int )											, AddGuildWarScore		);
		LuaRegisterFunc( "AddGuildWarEnergy"			, bool( int,int )											, AddGuildWarEnergy		);
		LuaRegisterFunc( "AddGuildWarPlayerValue"		, bool( int,int,int )										, AddGuildWarPlayerValue);
		LuaRegisterFunc( "GetGuildWarPlayerValue"		, int( int,int )											, GetGuildWarPlayerValue);
//		LuaRegisterFunc( "AddGuildWarValue"				, bool( int,int,int )										, AddGuildWarValue		);
//		LuaRegisterFunc( "GetGuildWarValue"				, int( int,int )											, GetGuildWarValue		);
		LuaRegisterFunc( "GetGuildWarScore"				, int( int )												, GetGuildWarScore		);
		LuaRegisterFunc( "GetGuildWarEnergy"			, int( int )												, GetGuildWarEnergy		);
		LuaRegisterFunc( "GetGuildWarScore_ByID"		, int( int,int )											, GetGuildWarScore_ByID	);
		LuaRegisterFunc( "GetGuildName_ByID"			, const char*( int,int )									, GetGuildName_ByID		);

		LuaRegisterFunc( "SendGuildWarFinalScore"		, void( int,int,int,int,int )								, SendGuildWarFinalScore	);

		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "SetSmallGameMenuType"			, bool( int,int,int )										, SetSmallGameMenuType	);
		LuaRegisterFunc( "SetSmallGameMenuValue"		, bool( int, int,int,int )									, SetSmallGameMenuValue	);
		LuaRegisterFunc( "SetSmallGameMenuStr"			, bool( int, int,int, const char* )							, SetSmallGameMenuStr	);
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "SearchTreasure"				, int( int)													, SearchTreasure		);
		LuaRegisterFunc( "GetTreasure"					, bool( int, int,int )										, GetTreasure			);

		LuaRegisterFunc( "StopMove"						, bool( int, bool)											, StopMove				);
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "PartyInstanceLv"				, int( int )												, PartyInstanceLv		);
		LuaRegisterFunc( "WriteInstanceArg"				, bool( int , int )											, WriteInstanceArg		);
		LuaRegisterFunc( "ReadInstanceArg"				, int( int, int )											, ReadInstanceArg		);
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "GameTime"						, int(  )													, GameTime				);
		LuaRegisterFunc( "ReadAccountFlag"				, bool( int , int )											, ReadAccountFlag		);
		LuaRegisterFunc( "WriteAccountFlag"				, bool( int , int , bool  )									, WriteAccountFlag		);
		LuaRegisterFunc( "ReadAccountFlagValue"			, int( int , int , int )									, ReadAccountFlagValue	);
		LuaRegisterFunc( "WriteAccountFlagValue"		, bool( int , int , int , int )								, WriteAccountFlagValue	);
		//////////////////////////////////////////////////////////////////////////
		//設定Map icon & tip
		LuaRegisterFunc( "ResetMapMark"					, bool( int )												, ResetMapMark				);
		LuaRegisterFunc( "SetMapMark"					, bool( int , int , float , float , float , const char* , int )	, SetMapMark			);
		LuaRegisterFunc( "ClsMapMark"					, bool( int , int   )										, ClsMapMark				);

		//////////////////////////////////////////////////////////////////////////
		//設定區域變數
		LuaRegisterFunc( "ResetRoomValue"				, bool( int )												, ResetRoomValue			);
		LuaRegisterFunc( "SetRoomValue"					, bool( int , int , int )									, SetRoomValue				);
		LuaRegisterFunc( "GetRoomValue"					, int( int , int   )										, GetRoomValue				);
		LuaRegisterFunc( "ClsRoomValue"					, bool( int , int   )										, ClsRoomValue				);
		//////////////////////////////////////////////////////////////////////////
		//排序處理
		LuaRegisterFunc( "SortList_Reset"				, void( )													, SortList_Reset				);
		LuaRegisterFunc( "SortList_Push"				, void( int , int )											, SortList_Push				);
		LuaRegisterFunc( "SortList_Sort"				, int( )													, SortList_Sort				);
		LuaRegisterFunc( "SortList_Get"					, int( )													, SortList_Get				);
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "SetExSkill"					, bool( int , int , int, int )								, SetExSkill				);
		LuaRegisterFunc( "DelExSkill"					, bool( int , int, int )									, DelExSkill				);

		LuaRegisterFunc( "CreateObj_ByObjPoint"			, int( int , int , const char* )	                        , CreateObj_ByObjPoint      );

		//進出世界
		LuaRegisterFunc( "EnterWorld"					, bool( int , int , int , int , float , float , float , float )		, EnterWorld      );
		LuaRegisterFunc( "LeaveWorld"					, bool( int )						                       , LeaveWorld      );

		//排行榜
		LuaRegisterFunc( "Billboard_Update"				, bool( int ,int ,int)						               , Billboard_Update      );
		LuaRegisterFunc( "Billboard_Add"				, bool( int ,int ,int)						               , Billboard_Add      );
		LuaRegisterFunc( "Billboard_TopUpdate"				, bool( int ,int ,int)						           , Billboard_TopUpdate      );

		//開啟兌換魔石的介面
		LuaRegisterFunc( "OpenMagicStoneShop"			, bool( int ,int ,int,int)								    , OpenMagicStoneShop    );
		//////////////////////////////////////////////////////////////////////////
		//設定縮放大小
		LuaRegisterFunc( "SetModelScale"				, bool( int ,float )										, SetModelScale			);

		LuaRegisterFunc( "ClientEvent"					, void( int , int , int , const char* )						, ClientEvent			);
		LuaRegisterFunc( "ClientEvent_Range"			, void( int , int , const char* )							, ClientEvent_Range		);
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "GetDBKeyValue"				, int( const char* )										, GetDBKeyValue		);
		LuaRegisterFunc( "LockHP"						, bool ( int , int , const char* )							, LockHP			);
		LuaRegisterFunc( "SpecialMove"					, bool ( int , float , float , float , int )				, SpecialMove		);
		//////////////////////////////////////////////////////////////////////////
		//企劃線段組檔
		LuaRegisterFunc( "ClearLineBlock"				, void ( int )												, ClearLineBlock		);
		LuaRegisterFunc( "AddLineBlock"					, void ( int , int , float , float , float , float )		, AddLineBlock			);
		LuaRegisterFunc( "DelLineBlock"					, bool ( int , int )										, DelLineBlock			);
		LuaRegisterFunc( "CheckLineBlock"				, bool ( int , float , float , float , float )				, CheckLineBlock		);
		//////////////////////////////////////////////////////////////////////////
		//註冊 C 的函式
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "GetTickCount"					, int( )							, GetTickCount		);

		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Public Encounter 相關
		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc("PE_Start"								, void(int, int)																, PE_Start			);
		LuaRegisterFunc("PE_AddPE"								, void(const char*, const char*, int, int)										, PE_AddPE			);
		LuaRegisterFunc("PE_AddRegion"							, void(int, int)																, PE_AddRegion		);	
		LuaRegisterFunc("PE_AddPhase"							, void(int, int, const char*, const char*, int, int, int, int)					, PE_AddPhase		);
		LuaRegisterFunc("PE_JumpToPhase"						, void(int, int)																, PE_JumpToPhase	);
		LuaRegisterFunc("PE_SetCallBackOnSuccess"				, void(int, const char*)														, PE_SetCallBackOnSuccess	);
		LuaRegisterFunc("PE_SetCallBackOnFail"					, void(int, const char*)														, PE_SetCallBackOnFail	);
		LuaRegisterFunc("PE_SetCallBackOnBalance"				, void(int, const char*)														, PE_SetCallBackOnBalance	);
		LuaRegisterFunc("PE_SetCallBackOnPlayerEnter"			, void(int, const char*)														, PE_SetCallBackOnPlayerEnter	);
		LuaRegisterFunc("PE_SetCallBackOnPlayerLeave"			, void(int, const char*)														, PE_SetCallBackOnPlayerLeave	);
		
		LuaRegisterFunc("PE_SetPhaseBonusScore"					, void(int, float, float)														, PE_SetPhaseBonusScore	);
		LuaRegisterFunc("PE_GivePlayerScore"					, void(int, int, float)															, PE_GivePlayerScore	);
		LuaRegisterFunc("PE_GivePartyScore"						, void(int, int, float)															, PE_GivePartyScore	);
		LuaRegisterFunc("PE_GiveAllPlayersScore"				, void(int, float)																, PE_GiveAllPlayersScore	);
		LuaRegisterFunc("PE_GetPlayerScore"						, float(int, int)																, PE_GetPlayerScore	);
		LuaRegisterFunc("PE_GetPartyScore"						, void(int, int, const char*)													, PE_GetPartyScore	);
		LuaRegisterFunc("PE_GetAllPlayersScore"					, void(int, const char*)														, PE_GetAllPlayersScore	);
		LuaRegisterFunc("PE_GetTopNPlayersScore"				, void(int, int, const char*)													, PE_GetTopNPlayersScore	);
		LuaRegisterFunc("PE_SetPlayerScore"						, void(int, int, float)															, PE_SetPlayerScore	);
		LuaRegisterFunc("PE_CountHatePoint"						, void(int, int)																, PE_CountHatePoint	);
		LuaRegisterFunc("PE_GetActivePhaseIndex"				, int(int)																		, PE_GetActivePhaseIndex	);
		LuaRegisterFunc("PE_GetActivePlayerGUIDCount"			, int(int)																		, PE_GetActivePlayerGUIDCount	);
		LuaRegisterFunc("PE_GetActivePlayerGUID"				, int(int)																		, PE_GetActivePlayerGUID	);
		LuaRegisterFunc("PE_SetScoreVisible"					, void(int, bool)																, PE_SetScoreVisible	);
		LuaRegisterFunc("PE_SetUIVisible"						, void(int, bool)																, PE_SetUIVisible	);
		LuaRegisterFunc("PE_RefreshConfig"						, void( )																		, PE_RefreshConfig	);
		
		LuaRegisterFunc("PE_PH_AddObjective_Greater"			, void(int, int, int, const char*, const char*, const char*, int, int, bool)	, PE_PH_AddObjective_Greater);
		LuaRegisterFunc("PE_PH_AddObjective_Smaller"			, void(int, int, int, const char*, const char*, const char*, int, int, bool)	, PE_PH_AddObjective_Smaller);
		LuaRegisterFunc("PE_PH_AddObjective_Equal"				, void(int, int, int, const char*, const char*, const char*, int, int, bool)	, PE_PH_AddObjective_Equal);
		LuaRegisterFunc("PE_PH_AddObjective_NotEqual"			, void(int, int, int, const char*, const char*, const char*, int, int, bool)	, PE_PH_AddObjective_NotEqual);
		LuaRegisterFunc("PE_PH_AddObjective_EqualAndGreater"	, void(int, int, int, const char*, const char*, const char*, int, int, bool)	, PE_PH_AddObjective_EqualAndGreater);
		LuaRegisterFunc("PE_PH_AddObjective_EqualAndSmaller"	, void(int, int, int, const char*, const char*, const char*, int, int, bool)	, PE_PH_AddObjective_EqualAndSmaller);
		LuaRegisterFunc("PE_PH_AddObjective_TimeLimit"			, void(int, int, int, const char*, const char*, const char*, int, int, bool)	, PE_PH_AddObjective_TimeLimit);
		LuaRegisterFunc("PE_PH_SetCallBackOnSuccess"			, void(int, const char*)														, PE_PH_SetCallBackOnSuccess);
		LuaRegisterFunc("PE_PH_SetCallBackOnFail"				, void(int, const char*)														, PE_PH_SetCallBackOnFail	);
		LuaRegisterFunc("PE_PH_SetBonusScore"					, void(int, int, float, float)													, PE_PH_SetBonusScore	);
		LuaRegisterFunc("PE_PH_SetNextPhase"					, void(int, int, int)															, PE_PH_SetNextPhase	);

		LuaRegisterFunc("PE_OB_SetCallBackOnAchieve"			, void(int, const char*)														, PE_OB_SetCallBackOnAchieve	);
		LuaRegisterFunc("PE_OB_SetNextPhase"					, void(int, int, int, int)														, PE_OB_SetNextPhase	);
		LuaRegisterFunc("PE_OB_SetVisible"						, void(int, int, int, bool)														, PE_OB_SetVisible	);

		LuaRegisterFunc("PE_GetVar"								, int(const char*)																, PE_VAR_GetVar	);
		LuaRegisterFunc("PE_SetVar"								, void(const char*, int)														, PE_VAR_SetVar	);
		LuaRegisterFunc("PE_AddVar"								, void(const char*, int)														, PE_VAR_AddVar	);		

		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc("GetIniValueSetting"							, int( const char* )													, GetIniValueSetting	);	

		//////////////////////////////////////////////////////////////////////////
		LuaRegisterFunc( "WeekInstancesReset"					, bool( )																		, WeekInstancesReset		);

		LuaRegisterFunc("GetMemory"								, int()																			, GetMemory	);		
		LuaRegisterFunc("SetCameraFollowUnit"					, void(int, int, int, int, int)													, SetCameraFollowUnit );
		
		LUA_VMClass::RegFunction( "CallPlot",		CallPlot	);
		LUA_VMClass::RegFunction( "DebugCheck",		DebugCheck	);


		return true;
	}
	//---------------------------------------------------------------------------------
	int				GetSystemTime	( int iTimeID );
	bool	Release()
	{
		Release_Hsiang();
		return true;
	};
	//---------------------------------------------------------------------------------
	//取得劇情擁有者的ID   
	int	OwnerID (  )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		if( T == NULL )
			return 0;
		return T->OwnerID;
	}
	//---------------------------------------------------------------------------------
	//取得劇情目標的ID
	int	TargetID( )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		if( T == NULL )
			return 0;
		return T->TargetID;
	}
	//----------------------------------------------------------------------------------------
	//取亂數
	int	Random( int Range )
	{
		if( Range <= 0 )
			return 0;

		if( Range >= 0x8000)
		{
			unsigned RandValue = unsigned( rand() * 0x8000 + rand() ) % Range;
			return RandValue;
		}

		return rand() % Range;
	}
	//----------------------------------------------------------------------------------------
	// Plot ==> void SysPrint( Str 內容 )
	void SysPrint( const char* Str )
	{
		if( Str == NULL )
			return;
		printf( Str);
	}
	//----------------------------------------------------------------------------------------
	int	MoveTarget( int ID , int TargetID )
	{
		//暫時
		BaseItemObject *OtherClass	= BaseItemObject::GetObj( ID );	
		if( OtherClass == NULL  )
			return 0;
		RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );
		if( Target == NULL )
			return 0;

		RoleDataEx  *Other = OtherClass->Role();	
		if( Other->IsAttack() )
			return 20;

		float NX , NZ , NY;
		Global::SearchAttackNode( Other , Target , NX , NY , NZ );

		OtherClass->Path()->SetPos( Other->X() , Other->Y() , Other->Z() );
		OtherClass->Path()->Target( NX , NY , NZ , Other->X() , Other->Y() , Other->Z() );

		int Time = (int)( OtherClass->Path()->TotalLength()*10 / Other->TempData.Attr.Fin.MoveSpeed );

		return Time;
	}
	//----------------------------------------------------------------------------------------
	//定點 移動
	int  Move( int ID , float x  , float y , float z )
	{
		//暫時
		BaseItemObject *OtherClass	= BaseItemObject::GetObj( ID );	
		if( OtherClass == NULL  )
			return 0;

		RoleDataEx  *Other = OtherClass->Role();	
		if( Other->IsAttack() )
			return 20;

		OtherClass->Path()->SetPos( Other->X() , Other->Y() , Other->Z() );
		OtherClass->Path()->Target( x , y , z , Other->X() , Other->Y() , Other->Z() );
		int Time = (int)( OtherClass->Path()->TotalLength()*10 / Other->TempData.Attr.Fin.MoveSpeed );
		return Time;
	}
	//---------------------------------------------------------------------------------
	//定點直線移動
	int  MoveDirect( int ID , float x  , float y , float z )
	{
		//暫時
		BaseItemObject *OtherClass	= BaseItemObject::GetObj( ID );	
		if( OtherClass == NULL  )
			return 0;

		RoleDataEx  *Other = OtherClass->Role();	
		if( Other->IsAttack() )
			return 20;
		
		if( Other->BaseData.Mode.Gravity == false )
			OtherClass->Path()->SetPos_NoCheckHeight( Other->X() , Other->Y() , Other->Z() );
		else
			OtherClass->Path()->SetPos( Other->X() , Other->Y() , Other->Z() );
		OtherClass->Path()->Target( x , y , z, false );
		int Time = 10;
		if( Other->BaseData.Mode.Gravity != false )
			Time = (int)( OtherClass->Path()->TotalLength()*10 / Other->TempData.Attr.Fin.MoveSpeed );
		else
			Time = (int)( OtherClass->Path()->TotalLength3D()*10 / Other->TempData.Attr.Fin.MoveSpeed );
		return Time;
	}
	//---------------------------------------------------------------------------------
	//設定亂數移動
	void SetRandMove( int ID , int Range , int Freq, int Dist )
	{
		BaseItemObject *OtherClass	= BaseItemObject::GetObj( ID );	
		if( OtherClass == NULL  )
			return;

		NPC_RandomMoveStruct& RandomMove = OtherClass->Role()->TempData.RandomMove;

		RandomMove.Active = __max( 1 , Freq );
		RandomMove.CenterX = OtherClass->Role()->Pos()->X;
		RandomMove.CenterZ = OtherClass->Role()->Pos()->Z;

		RandomMove.CountDown = 0;
		RandomMove.MaxRange  = (float)Range;
		RandomMove.Dist      = (float)Dist;

		RandomMove.Enable    = true;

		return;
	}
	//---------------------------------------------------------------------------------
	//產生物件
	int  CreateObj( int OrgObjID , float x , float y , float z , float dir , int count )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();

		int ObjID = Global::CreateObj(  OrgObjID , x , y , z , dir , count );
		
		RoleDataEx* Role = Global::GetRoleDataByGUID( ObjID );
		if( Role != NULL )
		{
			RoleDataEx* Owner = Global::GetRoleDataByGUID( T->OwnerID );
			if( Owner != NULL )
				Role->BaseData.RoomID = Owner->RoomID();

			Print( LV2 , "CreateObj" , "GUID = %d OrgObjID=%d roomid=%d , Lua=%s" , Role->GUID(), OrgObjID , Role->BaseData.RoomID , T->FuncName );
		}

		return ObjID;//Global::CreateObj(  ObjID , x , y , z , dir , count );
	}
	//利用旗子座標產生物件 FlagID = -1 代表用亂數
	int CreateObjByFlag( int OrgObjID , int FlagObjID , int FlagID , int count )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		FlagPosInfo* FlagObj = FlagPosClass::GetFlag( FlagObjID , FlagID );
		if( FlagObj == NULL )
			return -1;

		int ObjID = Global::CreateObj(  OrgObjID , FlagObj->X , FlagObj->Y , FlagObj->Z , FlagObj->Dir , count );

		RoleDataEx* Role = Global::GetRoleDataByGUID( ObjID );
		if( Role != NULL )
		{
			RoleDataEx* Owner = Global::GetRoleDataByGUID( T->OwnerID );
			if( Owner != NULL )
				Role->BaseData.RoomID = Owner->RoomID();
			Print( LV2 , "CreateObj" , "GUID = %d OrgObjID=%d roomID=%d , Lua=%s" , Role->GUID() , OrgObjID , Role->BaseData.RoomID , T->FuncName );
		}

		return ObjID;
	}
	//---------------------------------------------------------------------------------
	// Type 
	//	EM_RoleValue_X
	//	EM_RoleValue_Y
	//	EM_RoleValue_Z
	int	GetMoveFlagValue( int FlagObjID , int FlagID , int Type )
	{
		FlagPosInfo* FlagObj = FlagPosClass::GetFlag( FlagObjID , FlagID );
		if( FlagObj == NULL )
			return 0;
		switch( Type )
		{
		case EM_RoleValue_X:
			return int( FlagObj->X );
			break;
		case EM_RoleValue_Y:
			return int( FlagObj->Y );
			break;
		case EM_RoleValue_Z:
			return int( FlagObj->Z );
			break;
		case EM_RoleValue_Dir:
			return int( FlagObj->Dir );
			break;
		}
		return 0;
	}
	//---------------------------------------------------------------------------------
	//取得這類旗子有幾個
	int	GetMoveFlagCount( int FlagObjID )
	{
		return FlagPosClass::Count( FlagObjID );	
	}
	//---------------------------------------------------------------------------------
	//加入顯像物件
	bool AddtoPartition( int GItemID , int RoomID )
	{  
		BaseItemObject*		OtherObj = Global::GetObj( GItemID );
		if( OtherObj == NULL )
			return false;
		if( OtherObj->Role()->IsDestroy() != false )
			return false;

		OtherObj->Role()->TempData.HideInfo.Init();

		if( OtherObj->Role()->SelfData.AutoPlot.Size() != 0 )
			OtherObj->PlotVM()->CallLuaFunc( OtherObj->Role()->SelfData.AutoPlot.Begin() , OtherObj->GUID() );

		GameObjDbStructEx* pObj = Global::GetObjDB( OtherObj->Role()->BaseData.ItemInfo.OrgObjID );

		if( pObj && ( pObj->IsNPC() || pObj->IsQuestNPC() ) )
		{
			if( strlen( pObj->NPC.szLuaInitScript ) != 0 )
			{
				OtherObj->PlotVM()->CallLuaFunc( pObj->NPC.szLuaInitScript , OtherObj->GUID() );
			}
		}

		return Global::CheckClientLoading_AddToPartition( GItemID , RoomID );
	}
	//---------------------------------------------------------------------------------
	//加入顯像物件
	bool DelFromPartition( int GItemID )
	{
		BaseItemObject*		OtherObj = Global::GetObj( GItemID );
		if( OtherObj == NULL )
			return false;
		RoleDataEx* other = OtherObj->Role();


		PlotRegStruct* T = LUA_VMClass::ProcReg();
		LuaHideInfoStruct& HideInfo = other->TempData.HideInfo;
		HideInfo.Init();
		RoleDataEx* owner = Global::GetRoleDataByGUID( T->OwnerID );
		if( owner )
		{
			HideInfo.OwnerDBID = owner->DBID();
			HideInfo.OwnerOrgID = owner->OrgObjID();
		}
		HideInfo.PlotStr = T->FuncName;

		OtherObj->PlotVM()->EndAllLuaFunc();
		T->Type = PlotRun;

		return Global::DelFromPartition( GItemID );
	}
	//---------------------------------------------------------------------------------
	//加入顯像物件
	bool Show( int GItemID , int RoomID )
	{  
		BaseItemObject*		OtherObj = Global::GetObj( GItemID );
		if( OtherObj == NULL )
			return false;
		OtherObj->Role()->TempData.HideInfo.Init();
		return Global::CheckClientLoading_AddToPartition( GItemID , RoomID );
	}
	//---------------------------------------------------------------------------------
	//加入顯像物件
	bool Hide( int GItemID )
	{
		BaseItemObject*		OtherObj = Global::GetObj( GItemID );
		if( OtherObj == NULL )
			return false;
		RoleDataEx* other = OtherObj->Role();
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		LuaHideInfoStruct& HideInfo = other->TempData.HideInfo;
		HideInfo.Init();
		RoleDataEx* owner = Global::GetRoleDataByGUID( T->OwnerID );
		if( owner )
		{
			HideInfo.OwnerDBID = owner->DBID();
			HideInfo.OwnerOrgID = owner->OrgObjID();
		}
		HideInfo.PlotStr = T->FuncName;

		return Global::DelFromPartition( GItemID );
	}
	//---------------------------------------------------------------------------------
	//執行劇情
	bool RunPlot( int GItemID , const char* PlotName )
	{
		BaseItemObject*		OtherObj = Global::GetObj( GItemID );
		if( OtherObj == NULL )
			return false;

		PlotRegStruct* T = LUA_VMClass::ProcReg();
		OtherObj->PlotVM()->CallLuaFunc( PlotName , T->OwnerID , 0 );

		return true;
	}
	//---------------------------------------------------------------------------------
	//設定NPC死亡
	void NPCDead( int DeadID , int KillID )
	{
		BaseItemObject  *DeadObj		= BaseItemObject::GetObj( DeadID );	
		BaseItemObject  *KillObj		= BaseItemObject::GetObj( KillID );	

		if( DeadObj == NULL || KillObj == NULL )
		{
			Print( LV3 , "NPCDead" , "Dead==NULL or Kill==NULL DeadID=%d KillID=%d",DeadID , KillID );
			return;
		}


		DeadObj->Role()->Dead( true );
		//----------------------------------------------------------------------
		//掉落物品設定
		//		for( int i = 0 ; i < KillObj->Role()->TempData.Sys.KillRate ; i++ )
//		if( DeadObj->Role()->OwnerGUID() == -1 )
		Global::DeadDropItem( DeadID , KillID );
		//----------------------------------------------------------------------
	}
	//---------------------------------------------------------------------------------
	//刪除物件
	bool DelObj( int ItemID )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		BaseItemObject*	Obj = BaseItemObject::GetObj( ItemID );
		if( Obj == NULL || Obj->Role()->IsMirror() )
		{
			return false;
		}

		if( Obj->Role()->IsPlayer() != false )
		{
			return false;
		}

		//據情刪除物件，不刪除資料庫的
		Obj->ReleaseDBID( );
		Obj->Role()->Base.DBID =  -1 ;

		Obj->Destroy( T->FuncName );
/*
		//除錯資訊
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		if( T != NULL )
		{
			Print( Def_PrintLV2 , "DelObj" , "LiveTime(0)Plot Delete NPC Name=%s DBID =%d GUID=%d OrgID=%d Function=%s" 
				,  Global::GetRoleName_ASCII( Obj->Role() ).c_str()  
				, Obj->Role()->DBID() 
				, Obj->Role()->GUID()
				, Obj->Role()->BaseData.ItemInfo.OrgObjID
				, T->FuncName );
		}
*/
		return true;
	}
	//---------------------------------------------------------------------------------
	//重設NPC資料	
	//回傳需要多久的時間才可以重生
	int ReSetNPCInfo( int OwnerID )
	{
		//        RoleDataEx  *Owner	= Global::GetDataByGUID( OwnerID );	
		BaseItemObject*	OwnerObj = BaseItemObject::GetObj( OwnerID );

		if( OwnerObj == NULL )
		{
			Print( LV3 , "ReSetNPCInfo" , "Owner == NULL OwnerID=%d",OwnerID );
			return -1;
		}
		RoleDataEx*         Owner	= OwnerObj->Role();
		GameObjDbStructEx*  OrgDB	= Global::GetObjDB( Owner->BaseData.ItemInfo.OrgObjID );


		if( OrgDB->IsEmpty() )
			return -1;

		if( Owner->BaseData.Mode.Revive == false )
			return -1;



		if( strlen( Owner->RoleName() ) == 0 )
		{
			for( int i = 0 ; i < 2 ; i++ )
			{
				if( Random( 100000 ) < OrgDB->NPC.RaiseChangeRate[i] )
				{				
					GameObjDbStructEx*  NewNpcDB = Global::GetObjDB( OrgDB->NPC.RaiseChangeNPCID[i] );
					if( NewNpcDB->IsNPC() != false )
					{
						Owner->BaseData.ItemInfo.OrgObjID = OrgDB->NPC.RaiseChangeNPCID[i];
						//Owner->BaseData.RoleName = NewNpcDB->Name;
						break;
					}
				}
			}
		}


		OwnerObj->Path()->ClearPath();

		Owner->InitNPCData(); 
		Owner->StopMove();

		if( Owner->IsNPC() )
		{
			//取得重生位置(如果有很多移動旗子選一隻重生 )
			vector< NPC_MoveBaseStruct >& MoveListInfo = *(OwnerObj->NPCMoveInfo()->MoveInfo());

			if( MoveListInfo.size() != 0 && OrgDB->NPC.NoRandReborn == false )
			{
				int FlagID = int( rand() % MoveListInfo.size() );
				NPC_MoveBaseStruct& MoveInfo = MoveListInfo[ FlagID ];
				Owner->BaseData.Pos.X = MoveInfo.X;
				Owner->BaseData.Pos.Y = MoveInfo.Y;
				Owner->BaseData.Pos.Z = MoveInfo.Z;
				(static_cast< NpcAIBaseClass* >(OwnerObj->AI()))->SetNpcFlagPos( FlagID-1 );
				(static_cast< NpcAIBaseClass* >(OwnerObj->AI()))->GetMoveStatus()->Type = EM_NPCMoveFlagAIType_MoveToNext;			
			}
			else
			{
				Owner->BaseData.Pos	= Owner->SelfData.RevPos;
				(static_cast< NpcAIBaseClass* >(OwnerObj->AI()))->SetNpcFlagPos( -1 );
				(static_cast< NpcAIBaseClass* >(OwnerObj->AI()))->GetMoveStatus()->Type = EM_NPCMoveFlagAIType_MoveToNext;
			}
		}
		
		//計算重生時間
		Owner->InitCal();

		if( Global::St()->IsDisableNPCReviveTime != false )
			return 5;

		return OrgDB->ReviveTime;
	}

	//---------------------------------------------------------------------------------
	/*
	bool GetFlag( int GItemID , int FlagID )
	{
	BaseItemObject*		OtherObj = Global::GetObj( GItemID );
	if( OtherObj == NULL )
	return false;
	RoleDataEx* Role = OtherObj->Role();


	return Role->PlayerBaseData->Flag.GetFlag( FlagID );
	}*/

	bool GiveBodyItem_Note( int GItemID , int OrgID , int Count , const char* Note )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		RoleDataEx* NPC  = NULL;
		if( T != NULL )
		{
			NPC = Global::GetRoleDataByGUID( T->OwnerID );
			if( NPC == NULL || NPC->IsPlayer() )
			{
				NPC = Global::GetRoleDataByGUID( T->TargetID );
			}
		}

		RoleDataEx* ItemRole  = Global::GetRoleDataByGUID( GItemID );
		if( ItemRole == NULL )
		{
			Print( LV3 , "GiveBodyItem" , "Item==NULL ItemID=%d OrgID=%d Count=%d",GItemID, OrgID ,Count );
			return false;
		}

		GameObjDbStructEx*  GiveItemOrgDB = Global::GetObjDB( OrgID );
		if( GiveItemOrgDB->IsTreasure() )
		{
			for( int i = 0 ; i < Count ; i++ )
				ItemRole->GiveTreasure_Gamble( OrgID , NULL );
		}
		else if (GiveItemOrgDB->IsItem() == false )
		{
			return ItemRole->GiveItem( OrgID , Count , EM_ActionType_PlotGive , NPC , Note );
		}
		else
		{
			ItemFieldStruct	Item;
			if( Global::NewItemInit( Item , OrgID , 0 ) == false )
			{
				return false;
			}
			Item.Count	  = Count;
			Item.OrgObjID = OrgID;


			GameObjDbStructEx* OrgDB = Global::GetObjDB( OrgID );

			if( OrgDB == NULL )
				return false;

			if( OrgDB->IsArmor() || ( OrgDB->IsWeapon() && OrgDB->Item.WeaponPos != EM_WeaponPos_Ammo ) )
			{
				Item.Count	  = 1;
			}

			if( ItemRole->GiveItem( Item , EM_ActionType_PlotGive , NPC , Note ) == false )
				return false;

			if( OrgDB->Mode.DepartmentStore != false )
				Global::Log_DepartmentStore	( Item , EM_ActionType_PlotGive , Item.Count , 0 , 0 , 0 , ItemRole->DBID() );
		}

		return true;

	}
	bool GiveBodyItemEx( int GItemID , int OrgID , int Count , int RuneVolume , int Inherent1 , int Inherent2 , int Inherent3 , int Inherent4 , int Inherent5 )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		RoleDataEx* NPC  = NULL;
		if( T != NULL )
		{
			NPC = Global::GetRoleDataByGUID( T->OwnerID );
			if( NPC == NULL || NPC->IsPlayer() )
			{
				NPC = Global::GetRoleDataByGUID( T->TargetID );
			}
		}


		RoleDataEx* Target  = Global::GetRoleDataByGUID( GItemID );
		if( Target == NULL )
		{
			Print( LV3 , "GiveBodyItem" , "Item==NULL ItemID=%d OrgID=%d Count=%d",GItemID, OrgID ,Count );
			return false;
		}
		int Inherent[5] = { Inherent1 , Inherent2 , Inherent3 , Inherent4 , Inherent5 };

		for( int i = 0 ; i < 5 ; i++ )
		{
			GameObjDbStructEx* OrgDB = Global::GetObjDB( Inherent[0] );
			if( !OrgDB->IsAttribute() )
			{
				Inherent[i] = 0;
				break;
			}

		}

		if( Count == 0 )
			Count = 1;

		ItemFieldStruct	Item;
		if( Global::NewItemInit( Item , OrgID , 0 ) == false )
		{
			return false;
		}
		Item.Count	  = Count;
		Item.OrgObjID = OrgID;


		GameObjDbStructEx* OrgDB = Global::GetObjDB( OrgID );

		if( OrgDB == NULL )
			return false;

		if( OrgDB->IsArmor() || ( OrgDB->IsWeapon() && OrgDB->Item.WeaponPos != EM_WeaponPos_Ammo ) )
		{
			Item.Count	  = 1;
			Item.RuneVolume = RuneVolume;

			for( int i = 0 ; i < 5 ; i++ )
			{
				if( Inherent[i] == 0 )
					break;
				Item.Inherent( i , Inherent[i] );
			}
		}


		if( Target->GiveItem( Item , EM_ActionType_PlotGive , NPC , "" ) == false )
			return false;

		if( OrgDB->Mode.DepartmentStore != false )
			Global::Log_DepartmentStore	( Item , EM_ActionType_PlotGive , Item.Count , 0 , 0 , 0 , Target->DBID() );

		return true;
	}
	//---------------------------------------------------------------------------------
	//給予物?
	//回傳 0 代表失敗   1 代表成?
	bool GiveBodyItem( int GItemID , int OrgID , int Count )
	{
		return GiveBodyItem_Note( GItemID , OrgID , Count , "" );

	}
	//---------------------------------------------------------------------------------
	//刪除身上的某物品    
	//回傳 0 代表失敗   1 代表成?
	bool DelBodyItem( int GItemID , int OrgID , int Count  ) 
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
		{
			Print( LV3 , "DelBodyItem" , "Item==NULL ItemID=%d OrgID=%d Count=%d",GItemID, OrgID ,Count);
			return false;
		}

		if( Item->DelBodyItem( OrgID , Count , EM_ActionType_PlotDestory ) == false )
		{
			return false;
		}
		return true;
	}
	//---------------------------------------------------------------------------------
	//計算身上某一樣物品多少個 ( Body, Queue )
	//回傳 數量
	int CountBodyItem( int GItemID  , int OrgID )
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
		{
			Print( LV3 , "CountBodyItem" , "Item==NULL GItemID=%d OrgID=%d ",GItemID, OrgID );

			return false;
		}
		int Count = Item->CalBodyItemCount( OrgID ) + Item->CountItemFromQueue( OrgID );
		return Count;
	}
	//---------------------------------------------------------------------------------
	//計算此角色共有多少個 ( Bank Body EQ Queue )
	//回傳 數量
	int CountItem( int GItemID  , int OrgID )
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
		{
			Print( LV3 , "CountItem" , "Item==NULL GItemID=%d OrgID=%d ",GItemID, OrgID );

			return false;
		}
		int Count = Item->CalBodyItemCount( OrgID ) + Item->CalBankItemCount( OrgID ) + Item->CalEqItemCount( OrgID ) + Item->CountItemFromQueue( OrgID );
		return Count;
	}
	//---------------------------------------------------------------------------------

	//訊息
	bool Msg( int GItemID , const char* Str )
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
			return false;

		Item->Msg( CharToWchar( Str ).c_str() );
		return true;
	}

	//---------------------------------------------------------------------------------
	//密語
	bool Tell( int SendID , int SayID , const char* Str )
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( SendID );
		RoleDataEx* SayItem  = Global::GetRoleDataByGUID( SayID );
		if( Item == NULL || SayItem == NULL )
			return false;

		char szName[1024];

		if( SayItem->IsPlayer() == false )
		{
			//Sys111280_name
			sprintf_s( szName , sizeof(szName), "Sys%d_name", SayItem->BaseData.ItemInfo.OrgObjID );
		}
		else
		{
			MyStrcpy( szName, Global::GetRoleName( SayItem ) , sizeof(szName) );
		}

		Item->Tell( szName , Str );
		return true;
	}
	//---------------------------------------------------------------------------------
	//密語
	bool SayTo( int SendID , int SayID , const char* Str )
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( SendID );
		RoleDataEx* SayItem  = Global::GetRoleDataByGUID( SayID );
		if( Item == NULL || SayItem == NULL )
			return false;

		NetSrv_Talk::SayTo( SendID , SayID , Str );
		return true;
	}
	//---------------------------------------------------------------------------------
	//範圍 說話
	bool Say( int GItemID , const char* Str )
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
			return false;

		Item->Say( Str );

		

		return true;
	}

	//NPC 範圍 說話 廢話
	bool NpcSay( int GItemID , const char* Str )
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
			return false;

		NetSrv_Talk::Range( GItemID , true , Str, true );

		return true;
	}
	//---------------------------------------------------------------------------------
	//範圍 說話
	bool Yell( int GItemID, const char* Str, int iRange )
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
			return false;

		NetSrv_Talk::Yell( GItemID, true, Str, iRange );
		return true;
	}
	//---------------------------------------------------------------------------------
	//角色物件資料讀取
	//名稱 : RoleName Hp Mp ....
	int	    ReadRoleValue( int GItemID , int Type )
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
		{
			Print( LV1 , "ReadRoleValue" , "Item==NULL" );
			return 0;
		}
		return Item->GetValue_Int( (RoleValueName_ENUM)Type );
	}
	//---------------------------------------------------------------------------------
	double    ReadRoleValuefloat( int GItemID , int Type )
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
		{
			Print( LV1 , "ReadRoleValue" , "Item==NULL" );
			return 0;
		}
		return Item->GetValue( (RoleValueName_ENUM)Type );
	}    
	//---------------------------------------------------------------------------------
	//角色物件資料寫入
	bool    WriteRoleValue( int GItemID , int Type , double Value )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		if( T == NULL )
			return false;

		RoleDataEx* NPC = Global::GetRoleDataByGUID( T->OwnerID );
		if( NPC == NULL || NPC->IsNPC() == false )
		{
			NPC = Global::GetRoleDataByGUID( T->TargetID );
			if( NPC == NULL || NPC->IsNPC() == false )
				NPC = NULL;
		}

		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
		{
			Print( LV1 , "ReadRoleValue", "Item==NULL" );
			return false;
		}
		Item->SetValue( (RoleValueName_ENUM)Type , Value , NPC );

		if( EM_RoleValue_GoodEvil == Type )
		{
			Global::GoodEvilProc( Item , 0 );
			NetSrv_OtherChild::SendRange_GoodEvilValue( Item );
		}

		return true;

	}
	//---------------------------------------------------------------------------------
	//角色物件資料寫入
	bool    AddRoleValue( int GItemID , int Type , double Value )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		if( T == NULL )
			return false;

		RoleDataEx* NPC = Global::GetRoleDataByGUID( T->OwnerID );
		if( NPC == NULL || NPC->IsNPC() == false )
		{
			NPC = Global::GetRoleDataByGUID( T->TargetID );
			if( NPC == NULL || NPC->IsNPC() == false )
				NPC = NULL;
		}

		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
		{
			Print( LV3 , "ReadRoleValue", "Item==NULL" );
			return false;
		}
		Item->AddValue( (RoleValueName_ENUM)Type , Value );

		if( EM_RoleValue_GoodEvil == Type )
		{
			NetSrv_OtherChild::SendRange_GoodEvilValue( Item );
			Global::GoodEvilProc( Item , 0 );
		}

		return true;

	}
	//---------------------------------------------------------------------------------
	//角色物件資料寫入
	bool    AddExp( int GItemID , int Value )
	{
		RoleDataEx* Item  = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
		{
			Print( LV3 , "AddExp", "Item==NULL" );
			return false;
		}
		Item->Addexp( Value );
		return true;

	}
	//*********************************************************************************
	//劇情設定指令
	//*********************************************************************************
	//設定劇情
	//Plot ==> void	SetPlot( ID , 劇情Type , 劇情名稱 )
	//例如 ==> void SetPlot( ID , "ClientAuto" , "TestPlot" , Range);
	//Type = "ClientAuto" , "Auto" , "Move" , "Dead" , "Range" , "Touch"
	bool SetPlot( int GItemID , const char* PlotType, const char* PlotName , int Range )
	{       
		BaseItemObject *OtherClass	= BaseItemObject::GetObj( GItemID );

		if( OtherClass == NULL )
			return false;

		RoleDataEx* Other = OtherClass->Role();



		if( stricmp( PlotType , "range" ) == 0 )
		{
			if( strlen( PlotName ) == 0 )
			{
				Range = 0;
			}

			Other->TempData.RangePlot =  PlotName ;
			Other->TempData.RangePlot_Range = Range;

			NetSrv_Other::SC_Range_BaseObjValueChange( Other->GUID() , EM_ObjValueChange_Plot_Range , PlotName , Range );
		}
		else if( stricmp( PlotType , "dead" ) == 0 )
		{
			Other->TempData.DeadPlot =  PlotName ;
		}
		else if( stricmp( PlotType , "auto" ) == 0 )
		{
			Other->SelfData.AutoPlot =  PlotName ;
			if( Other->SelfData.AutoPlot.Size() != 0 )
				OtherClass->PlotVM()->CallLuaFunc( PlotName , GItemID , 0 );
		}	
		else if( stricmp( PlotType , "touch" ) == 0 )
		{
			if( strlen( PlotName ) == 0 )
			{
				Range = 0;
			}

			Other->TempData.TouchPlot =  PlotName ;
			Other->TempData.TouchPlot_Range = Range;

			NetSrv_Other::SC_Range_BaseObjValueChange( Other->GUID() , EM_ObjValueChange_Plot_Touch , PlotName , Range );
		}
		else if( stricmp( PlotType , "say" ) == 0 )
		{
			Other->TempData.SayPlot =  PlotName ;
		}		
		else if( stricmp( PlotType , "give" ) == 0 )
		{
			Other->TempData.GiveItemPlot =  PlotName ;
		}		
		else if( stricmp( PlotType , "clientauto" ) == 0 )
		{
			Other->TempData.C_AutoPlot =  PlotName ;
			NetSrv_Other::SC_Range_BaseObjValueChange( Other->GUID() , EM_ObjValueChange_Plot_ClientAuto , PlotName , 0 );
		}
		else if( stricmp( PlotType , "collision" ) == 0 )
		{
			Other->TempData.CollisionPlot		= PlotName;
			Other->BaseData.Mode.CollisionPlot	= true;

			NetSrv_Other::SC_Range_BaseObjValueChange( Other->GUID() , EM_ObjValueChange_Plot_Collision , PlotName , 0 );
		}
		else if( stricmp( PlotType , "checkpetcommand" ) == 0 )
		{
			Other->TempData.CheckPetCommand		= PlotName;
		}
		return true;
	}

	const char* GetPlotName( int GItemID , const char* PlotType  )
	{       
		BaseItemObject *OtherClass	= BaseItemObject::GetObj( GItemID );

		if( OtherClass == NULL )
			return "";

		RoleDataEx* Other = OtherClass->Role();

		if( stricmp( PlotType , "range" ) == 0 )
		{
			return Other->TempData.RangePlot.Begin();
		}
		else if( stricmp( PlotType , "dead" ) == 0 )
		{
			return Other->TempData.DeadPlot.Begin();
		}
		else if( stricmp( PlotType , "auto" ) == 0 )
		{
			return Other->SelfData.AutoPlot.Begin();
		}	
		else if( stricmp( PlotType , "touch" ) == 0 )
		{
			return	Other->TempData.TouchPlot.Begin();
		}
		else if( stricmp( PlotType , "say" ) == 0 )
		{
			return Other->TempData.SayPlot.Begin();
		}		
		else if( stricmp( PlotType , "give" ) == 0 )
		{
			return Other->TempData.GiveItemPlot.Begin();
		}		
		else if( stricmp( PlotType , "clientauto" ) == 0 )
		{
			return Other->TempData.C_AutoPlot.Begin();
		}
		else if( stricmp( PlotType , "collision" ) == 0 )
		{
			return Other->TempData.CollisionPlot.Begin();
		}
		else if( stricmp( PlotType , "checkpetcommand" ) == 0 )
		{
			return Other->TempData.CheckPetCommand.Begin();
		}

		return "";
	}
	//---------------------------------------------------------------------------------
	//設定位置
	bool	SetPos( int GItemID , float X , float Y , float Z , float Dir )
	{
		BaseItemObject*	Obj = BaseItemObject::GetObj( GItemID );

		if( Obj == NULL )
			return false;

/*		if(		Obj->Role()->BaseData.Mode.Gravity != false )
		{
			if(	Obj->Path()->Height( X , Y , Z  , Y , 30 ) == false )
				return false;
		}*/

		//NetSrv_MoveChild::SetObjPos( Obj , X , Y , Z , Dir );
		Global::CheckClientLoading_SetPos( GItemID , X , Y , Z , Dir );
		return true;
	}
	//---------------------------------------------------------------------------------
	//執行某劇情
	bool BeginPlot( int GItemID , const char* PlotName , int DelayTime )
	{
		PlotRegStruct*	T = LUA_VMClass::ProcReg();
		BaseItemObject*		ItemObj = BaseItemObject::GetObj( GItemID );
		if( ItemObj == NULL )
		{
			return false;
		}


		ArgTransferStruct* Arg = ItemObj->PlotVM()->CallLuaFunc( PlotName , T->OwnerID , DelayTime );

		if( Arg == NULL )
			return false;

		return true;
	}
	//---------------------------------------------------------------------------------
	bool BeginMultiStringPlot( int GItemID , const char* PlotName , int DelayTime )
	{
		PlotRegStruct*	T = LUA_VMClass::ProcReg();
		BaseItemObject*		ItemObj = BaseItemObject::GetObj( GItemID );
		if( ItemObj == NULL )
		{
			return false;
		}

		ArgTransferStruct* Arg = ItemObj->PlotVM()->CallLuaFunc( PlotName , T->OwnerID , DelayTime );

		if( Arg == NULL )
			return false;

		return true;
	}
	//---------------------------------------------------------------------------------
	//設定npc所屬劇情類別 用來之後收尋npc執行劇情用 or 其他的事
	//Plot ==> int	SetNpcPlotClass( ID , 自訂字串 )
	//例如 ==> int  SetNpcPlotClass( 1 , "AAA" );
	// Ret = 0 (執行失敗)無此物件
	//	   = 1 成?
	bool SetNpcPlotClass( int GItemID , const char* Str )
	{

		RoleDataEx*		ItemVal = Global::GetRoleDataByGUID( GItemID );
		if( ItemVal == NULL )
			return false;

		ItemVal->SelfData.PlotClassName = Str;

		return true;
	}
	//---------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------
	 
	//---------------------------------------------------------------------------------
	//讀取某一旗標
	/*
	bool    ReadFlag( int GItemID , int FlagID )
	{
	RoleDataEx*		Item = Global::GetRoleDataByGUID( GItemID );
	if( Item == NULL )
	return false;
	return Item->PlayerBaseData->Flag.GetFlag( FlagID );
	}
	*/
	//---------------------------------------------------------------------------------
	//寫入某一旗標
	/*
	bool    WriteFlag( int GItemID , int FlagID , int On )
	{
	RoleDataEx*		Item = Global::GetRoleDataByGUID( GItemID );
	if( Item == NULL )
	return false;
	if( On == 1 )
	Item->PlayerBaseData->Flag.SetFlagOn( FlagID );
	else if( On == 0 )
	Item->PlayerBaseData->Flag.SetFlagOff( FlagID );
	else
	return false;

	return true;
	}*/
	//---------------------------------------------------------------------------------
	//(讀取)把某一旗標當數值處理
	//例如 ==> int ReadFlagValue( 0 , 10 , 5  )   <== 代表重10 - 14 旗標代表的數字
	//return = 旗標用用二進位代表的數字
	/*
	int	    ReadFlagValue( int GItemID , int FlagID , int FlagCount )
	{
	RoleDataEx*		Item = Global::GetRoleDataByGUID( GItemID );
	if( Item == NULL )
	return 0;

	return Item->PlayerBaseData->Flag.GetFlagValue( FlagID , FlagCount );
	}
	*/
	//---------------------------------------------------------------------------------
	//(寫入)把某一旗標當數值處理
	//Plot ==> int WriteFlagValue( 開使旗標 , 多少旗標表示 , 要存的數字 )
	//例如 ==> int WriteFlagValue( ItemID , 10 , 5 , 19 )   <== 代表重10 - 14 旗標代表的數字 存入19
	//return  =  0 失敗
	//		  	 1 成?
	/*
	bool    WriteFlagValue( int GItemID , int FlagID , int FlagCount , int Value )
	{
	RoleDataEx*		Item = Global::GetRoleDataByGUID( GItemID );
	if( Item == NULL )
	return false;

	Item->PlayerBaseData->Flag.SetFlagValue( FlagID , FlagCount , Value );
	return true;
	}*/
	//---------------------------------------------------------------------------------
	//檢查物件是否存在
	//Plot ==> int CheckID( ItemID )
	//例如 ==> int CheckID( 100 )   <== 檢查100號物件是否存在
	//return  =  0 沒有
	//		  	 1 有
	bool CheckID( int GItemID )
	{
		RoleDataEx*		Item = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL )
			return false;
		return true;
	}
	/*
	int GetValue( int GItemID , int Type )
	{
	RoleDataEx*		Item = Global::GetRoleDataByGUID( GItemID );
	if( Item == NULL )
	return 0;
	int  Ret = Item->GetValue_Int( (RoleValueName_ENUM) Type );
	return Ret;
	}
	*/
	//---------------------------------------------------------------------------------
	//商店
	//---------------------------------------------------------------------------------
	//商店設定
	void    SetShop( int GItemID , int ShopObjID , const char* ShopClosePlot )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();
		RoleDataEx*		Shop = Global::GetRoleDataByGUID( GItemID );
		if( Shop == NULL /*|| Shop->IsPlayer()*/ )
		{
			Print( LV2 , "SetShop" , "Shop == NULL || Shop->IsPlayer() Func = %d" , T->FuncName );
			return;
		}
		GameObjDbStructEx*  OrgDB	= Global::GetObjDB( ShopObjID );
		if( OrgDB == NULL || !OrgDB->IsShop() )
		{
			Print( LV2 , "SetShop" , " OrgDB == NULL || !OrgDB->IsShop() ShopObjID = %d Func = %s" , ShopObjID , T->FuncName );
			return;
		}
		ShopStateStruct &ShopInfo = Shop->TempData.ShopInfo;
		ShopInfo.Init();
		ShopInfo.ShopClosePlot = ShopClosePlot;
		ShopInfo.ShopObjID = ShopObjID;
		ShopInfo.LastUpdateTime = RoleDataEx::G_SysTime;
		for( int i = 0 ; i < 30 ; i++ )
		{
			ShopInfo.ItemCount[i] =  OrgDB->Shop.SellItemMax[i] / 2; 
		}

		return;
	}

	//
	bool	CloseShop( int GItemID )
	{
		RoleDataEx*		Shop = Global::GetRoleDataByGUID( GItemID );
		if( Shop == NULL  )
		{
			return false;
		}

		PlayIDInfo**	Block = Global::PartSearch( Shop , _Def_View_Block_Range_  );

		int				i , j;
		PlayID*			TopID;

		for( i = 0 ; Block[i] != NULL ; i++ )
		{
			TopID = Block[i]->Begin;
			for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
			{
				BaseItemObject* Obj		= Global::GetObj( TopID->ID );
				if( Obj == NULL )
					continue;

				RoleDataEx*		Other	= Obj->Role();

				if( Other->IsPlayer() == false )
					continue;
				//SendToCli_ByProxyID( Other->TempData.Sys.SockID , Other->TempData.Sys.ProxyID , Size , Data );
				if( Other->TempData.ShopInfo.TargetID != GItemID )
					continue;
				NetSrv_Shop::S_ShopClose( Other->GUID() );

			}
		}

		return true;
	}

	//開啟 劇情給予物品
	void	OpenPlotGive( int GItemID , bool IsSort , int Count )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		NPC  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || NPC == NULL )
		{
			Print( LV2 , "OpenPlotGive" , "NPC == NULL Func = %d" , T->FuncName );
			return;
		}

		if( Player->CheckOpenSomething() != false )
			return;

		Player->TempData.ShopInfo.TargetID = NPC->GUID();
		Player->TempData.Attr.Action.OpenType  = EM_RoleOpenMenuType_PlotGiveItem;

		NetSrv_Other::S_OpenGiveItemPlot( Player->GUID() , NPC->GUID() , IsSort , Count );

	}

	//開起Bank
	bool    OpenBank( )
	{
		return OpenBankEx( TargetID() );
	}
	bool	OpenBankEx( int TargetID )
	{		
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		Bank  = Global::GetRoleDataByGUID( TargetID );
		if( Player == NULL || Bank == NULL )
		{
			Print( LV2 , "OpenBank" , "Bank == NULL || Bank->IsPlayer() Func = %d" , T->FuncName );
			return false ;
		}

		if( Player->CheckOpenSomething() != false )
			return false;

		Player->TempData.ShopInfo.TargetID = Bank->GUID();
		Player->TempData.Attr.Action.OpenType  = EM_RoleOpenMenuType_Bank;

		NetSrv_Item::OpenBank( Player->GUID() , Bank->GUID() );
		return true;
	}

	//開起商店
	bool    OpenShop( )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		Shop  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || Shop == NULL /* || Shop->IsPlayer()*/ )
		{
			Print( LV2 , "OpenShop" , "Shop == NULL || Shop->IsPlayer() Func = %d" , T->FuncName );
			return false;
		}

		if(		Player->TempData.ShopInfo.TargetID		== Shop->GUID() 
			&&	Player->TempData.Attr.Action.OpenType	== EM_RoleOpenMenuType_Shop )
			return false;

		if( Player->BaseData.SysFlag.DisableTrade != false )
		{
			Player->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
			return false;
		}

		if( Player->TempData.Attr.Action.OpenType != EM_RoleOpenMenuType_Shop && Player->CheckOpenSomething() != false )
			return false;

		ShopStateStruct &ShopInfo = Shop->TempData.ShopInfo;
		GameObjDbStructEx*  OrgDB	= Global::GetObjDB( ShopInfo.ShopObjID );
		if( OrgDB == NULL || !OrgDB->IsShop() )
		{
			Print( LV2 , "SetShop" , " OrgDB == NULL || !OrgDB->IsShop() ShopObjID = %d Func = %d" , ShopInfo.ShopObjID , T->FuncName );
			return false;
		}
		Player->TempData.ShopInfo.TargetID = Shop->GUID();
		Player->TempData.Attr.Action.OpenType	 = EM_RoleOpenMenuType_Shop;
		Player->PlayerTempData->Log.ActionMode.Shop	 = true;

		while( 1 )
		{
			if( RoleDataEx::G_SysTime - ShopInfo.LastUpdateTime <= 60*60*1000 )
				break;
			ShopInfo.LastUpdateTime += 60*60*1000;

			for( int i = 0 ; i < 30 ; i++ )
			{
				ShopInfo.ItemCount[i] =  ShopInfo.ItemCount[i] + OrgDB->Shop.SellItemPorduce[i]; 
				if( ShopInfo.ItemCount[i] > OrgDB->Shop.SellItemMax[i] )
					ShopInfo.ItemCount[i] = OrgDB->Shop.SellItemMax[i];
			}
		}


		NetSrv_Shop::S_ShopOpen( Player->GUID() , Shop->GUID() , ShopInfo.ShopObjID , ShopInfo.ItemCount );
		return true;
	}

	//開啟 帳號背包
	void	OpenAccountBag(  )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		BaseItemObject* PlayerObj = BaseItemObject::GetObj( T->OwnerID );
		RoleDataEx*		AccountBag  = Global::GetRoleDataByGUID( T->TargetID );
		if( PlayerObj == NULL || AccountBag == NULL )
		{
			Print( LV2 , "OpenAccountBag" , "AccountBag == NULL || Player == NULL Func = %d" , T->FuncName );
			return;
		}

		RoleDataEx*     Player = PlayerObj->Role();

		if( Player->BaseData.SysFlag.DisableTrade != false )
		{
			Player->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
			return;
		}

		if( Player->CheckOpenSomething() != false )
			return;

		PlayerObj->AccountBag()->clear();


		Player->TempData.ShopInfo.TargetID = AccountBag->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_AccountBag;
		NetSrv_ItemChild::SD_AccountBagInfoRequest	( Player->DBID() , Player->Account_ID() );	
	}
	//開啟 裝備賭博
	bool	OpenEqGamble()
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		BaseItemObject* PlayerObj = BaseItemObject::GetObj( T->OwnerID );
		RoleDataEx*		Target  = Global::GetRoleDataByGUID( T->TargetID );
		if( PlayerObj == NULL || Target == NULL )
		{
			Print( LV2 , "OpenEqGamble" , "PlayerObj == NULL || Traget == NULLL Func = %d" , T->FuncName );
			return false;
		}

		RoleDataEx*     Player = PlayerObj->Role();

		if( Player->CheckOpenSomething() != false )
			return false;


		Player->TempData.ShopInfo.TargetID = Target->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_EqGamble;
		NetSrv_ItemChild::SC_OpenGamebleItemState( Player->GUID() , Target->GUID() );	
		return true;
	}

	bool	OpenSuitSkill()
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		BaseItemObject* PlayerObj = BaseItemObject::GetObj( T->OwnerID );
		RoleDataEx*		Target  = Global::GetRoleDataByGUID( T->TargetID );
		if( PlayerObj == NULL || Target == NULL )
		{
			Print( LV2 , "OpenSuitSkill" , "PlayerObj == NULL || Traget == NULLL Func = %d" , T->FuncName );
			return false;
		}

		RoleDataEx*     Player = PlayerObj->Role();

		if( Player->CheckOpenSomething() != false )
			return false;


		Player->TempData.ShopInfo.TargetID = Target->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_SuitSkill;
		NetSrv_EQChild::S_LearnSuitSkill_Open( Player->GUID() , Target->GUID() );	
		return true;
	}


	bool    OpenMailEx( int TargetID )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		Mail  = Global::GetRoleDataByGUID( TargetID );
		if( Player == NULL || Mail == NULL )
		{
			Print( LV2 , "OpenMail" , "Mail == NULL || Player == NULL Func = %d" , T->FuncName );
			return false;
		}

		if( Global::Ini()->IsBattleWorld == true )
		{			
			return false;
		}

		if( Player->BaseData.SysFlag.DisableTrade != false )
		{
			Player->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
			return false;
		}

		if( Player->CheckOpenSomething() != false )
			return false;

		//信任玩機制
		if( Player->PlayerBaseData->VipMember.UntrustFlag.DisableMail )
		{
			//Player->Msg( "信任玩家機制測試_無法使用信箱" );
			Player->Net_GameMsgEvent( EM_GameMessageEvent_Role_Untrust);
			return false;
		}


		Player->TempData.ShopInfo.TargetID = Mail->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_Mail;
		Player->PlayerTempData->Log.ActionMode.Mail = true;

		NetSrv_Mail::SC_OpenMail( Player->GUID() , Mail->GUID() );
		return true;
	}
	bool    OpenMail( )
	{		
		return OpenMailEx( TargetID() );
	}

	bool    OpenAC( )
	{
		return OpenACEx( TargetID() );
	}

	bool    OpenACEx( int TargetID )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		AC  = Global::GetRoleDataByGUID( TargetID );
		if( Player == NULL || AC == NULL  )
		{
			Print( LV2 , "OpenAC" , "AC == NULL || Player == NULL Func = %d" , T->FuncName );
			return false;
		}

		if( Player->BaseData.SysFlag.DisableTrade != false )
		{
			Player->Net_GameMsgEvent( EM_GameMessageEvent_Item_DisableTrade );
			return false;
		}

		if( Player->TempData.Attr.Action.OpenType == EM_RoleOpenMenuType_AC )
		{
			NetSrv_AC::SC_AcAlreadyOpen( Player->GUID() , T->TargetID );
			return false;
		}


		if( Player->CheckOpenSomething() != false )
			return false;
		
		//信任玩機制
		if( Player->PlayerBaseData->VipMember.UntrustFlag.DisableOpenAC )
		{
			//Player->Msg( "信任玩家機制測試_無法使用拍賣所" );
			Player->Net_GameMsgEvent( EM_GameMessageEvent_Role_Untrust);
			return false;
		}

		Player->TempData.ShopInfo.TargetID = AC->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_AC;
		Player->PlayerTempData->Log.ActionMode.AC = true;

		NetSrv_AC::SDC_OpenAC( Player->DBID() , AC->GUID() );
		return true;
	}

	void    OpenChangeJob( )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		NPC  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || NPC == NULL  )
		{
			Print( LV2 , "OpenChangeJob" , "NPC == NULL || Player == NULL Func = %d" , T->FuncName );
			return;
		}

		if( Player->CheckOpenSomething() != false )
			return;


		Player->TempData.ShopInfo.TargetID = NPC->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_ChangeJob;


		NetSrv_RoleValue::S_OpenChangeJob( Player->GUID() , NPC->GUID() );
	}

	void    OpenBillboard( )			
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( T->OwnerID );
		RoleDataEx*		NPC  = Global::GetRoleDataByGUID( T->TargetID );
		if( Player == NULL || NPC == NULL  )
		{
			Print( LV2 , "OpenBillboard" , "NPC == NULL || Player == NULL Func = %d" , T->FuncName );
			return;
		}

		if( Player->CheckOpenSomething() != false )
			return;


		Player->TempData.ShopInfo.TargetID = NPC->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_Billboard;


		NetSrv_Billboard::SC_OpenBillboard( Player->GUID() , NPC->GUID() );
	}

	//開啟 溶合裝備
	void	OpenCombinEQ( int PlayerGUID , int NpcGUID , int Type )
	{
		PlotRegStruct*  T = LUA_VMClass::ProcReg();
		RoleDataEx*     Player = Global::GetRoleDataByGUID( PlayerGUID );
		RoleDataEx*		NPC  = Global::GetRoleDataByGUID( NpcGUID );
		if( Player == NULL || NPC == NULL  )
		{
			Print( LV2 , "OpenCombinEQ" , "NPC == NULL || Player == NULL Func = %d" , T->FuncName );
			return;
		}

		if( Player->CheckOpenSomething() != false )
			return;


		Player->TempData.ShopInfo.TargetID = NPC->GUID();
		Player->TempData.Attr.Action.OpenType = EM_RoleOpenMenuType_EQCombin;

		Player->TempData.EQCombin.UseItemTempID = Player->TempData.Magic.UseItem;	//暫存使用的物品
		Player->TempData.EQCombin.Type = Type;

		NetSrv_Item::S_EQCombinOpen( Player->GUID() , NPC->GUID() , Player->TempData.Magic.UseItemPos , Type );
	}

	bool CreateFlag( int ObjID , int FlagID, float X , float Y , float Z , float Dir )
	{
		int Ret = FlagPosClass::AddFlag( NULL , ObjID , FlagID , X , Y , Z , Dir );
		//        bool Ret = Global::CreateFlag( ObjID , FlagID , X , Y , Z );
		return ( Ret != -1 ) ;
	}

	//設定物件模式
	bool SetMode( int GItemID , int Mode )
	{
		RoleDataEx*		Item = Global::GetRoleDataByGUID( GItemID );
		if( Item == NULL || Item->IsPlayer() )
			return false;
		Item->BaseData.Mode._Mode = Mode;

		return true;
	}

	int	 GetMode( int GitemID )
	{
		RoleDataEx*		Item = Global::GetRoleDataByGUID( GitemID );
		if( Item == NULL )
			return 0;

		return Item->BaseData.Mode._Mode;
	}
	//---------------------------------------------------------------------------------
	//戰鬥劇情
	//---------------------------------------------------------------------------------
	//設定尋找某一個物件
	//回傳 : 搜尋到的數量
	int     SetSearchRangeInfo( int GItemID , int Range )
	{
		SearchObjList.clear();

		RoleDataEx* Owner;
		RoleDataEx* Other;

		Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return 0;

		Global::ResetRange( Owner , _Def_View_Block_Range_ );
		while( 1 )
		{
			Other = Global::GetRangeRole( );
			if( Other == NULL )
				break;
			if( Other->RoomID() != Owner->RoomID() )
				continue;

			if( Other->Length( Owner ) <= Range )
				SearchObjList.push_back( Other->GUID() );
		}

		return (int)SearchObjList.size();
	}

	int GetNumPlayer()
	{
		int iSize = (int)BaseItemObject::PlayerObjSet()->size();
		return iSize;
	}

	int		SetSearchAllPlayer( int RoomID )
	{
		SearchObjList.clear();

		set<BaseItemObject* >&	PlayerObjSet = *(BaseItemObject::PlayerObjSet());
		set< BaseItemObject*>::iterator   PlayerObjIter;
		for( PlayerObjIter = PlayerObjSet.begin() ; PlayerObjIter != PlayerObjSet.end() ; PlayerObjIter++ )
		{
			BaseItemObject* Obj = *PlayerObjIter;
			if( Obj == NULL )
				continue;

			if( Obj->Role()->RoomID() != RoomID )
				continue;
			if(		Obj->Role()->TempData.Sys.OnChangeZone
				||	Obj->Role()->TempData.Sys.OnChangeWorld )
				continue;

			SearchObjList.push_back( Obj->GUID() );
		}

		return (int)SearchObjList.size();
	}
	//收尋房間內某個企劃群組的物件
	int		SetSearchAllNPC_ByGroupID( int RoomID , int GroupID )
	{
		SearchObjList.clear();

		set<BaseItemObject* >&	NpcObjSet = *(BaseItemObject::NPCObjSet());
		set< BaseItemObject*>::iterator   NpcObjIter;
		for( NpcObjIter = NpcObjSet.begin() ; NpcObjIter != NpcObjSet.end() ; NpcObjIter++ )
		{
			BaseItemObject* Obj = *NpcObjIter;
			if( Obj == NULL )
				continue;

			if( Obj->Role()->RoomID() != RoomID )
				continue;
			GameObjDbStructEx* ObjDB = Global::GetObjDB( Obj->Role()->BaseData.ItemInfo.OrgObjID );
			if( ObjDB == NULL || ObjDB->NPC.SearchGroupID != GroupID )
				continue;

			SearchObjList.push_back( Obj->GUID() );
		}

		return (int)SearchObjList.size();
	}

	int     SetSearchAllNPC( int RoomID )
	{
		SearchObjList.clear();

		set<BaseItemObject* >&	NpcObjSet = *(BaseItemObject::NPCObjSet());
		set< BaseItemObject*>::iterator   NpcObjIter;
		for( NpcObjIter = NpcObjSet.begin() ; NpcObjIter != NpcObjSet.end() ; NpcObjIter++ )
		{
			BaseItemObject* Obj = *NpcObjIter;
			if( Obj == NULL )
				continue;

			if( Obj->Role()->RoomID() != RoomID )
				continue;

			SearchObjList.push_back( Obj->GUID() );
		}

		return (int)SearchObjList.size();
	}
	int     GetSearchResult( )
	{
		if( SearchObjList.size() == 0 )
			return -1;

		int Ret =  SearchObjList.back();
		SearchObjList.pop_back();
		return Ret;
	}
	//設定某物件開啟攻擊
	bool    SetAttack( int GItemID , int TargetID )
	{
		RoleDataEx* Owner  = Global::GetRoleDataByGUID( GItemID );
		RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );
		if( Owner == NULL || Target == NULL )
			return false;
		return Owner->SetAttack( Target );        
	}
	//關閉攻擊
	bool    SetStopAttack( int GItemID )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		return Owner->StopAttack();
	}

	//設定逃跑開啟或關閉  (戰鬥不關)
	bool    SetEscape( int GItemID , bool Flag )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GItemID );
		if( Owner == NULL )
			return false;

		if( Flag == false )
		{
			if( Owner->IsAttack() == false )
			{
				Owner->TempData.AI.ActionType = EM_ActionType_NORMAL;
			}
			else
			{
				Owner->TempData.AI.ActionType = EM_ActionType_ATTACK;
			}
		}
		else
		{
			if( Owner->IsAttack() == false )
				return false;

			Owner->TempData.AI.ActionType = EM_ActionType_ESCAPE;
		}

		return true;
	}

	// 設定任務點選物件後, 檢查結果
	void SendQuestClickObjResult( int iRoleGUID, int iResult, int iItemID, int iItemVal )
	{
		CNetSrv_Script::ClickQuestObjResult( iRoleGUID, iResult, iItemID, iItemVal );
	}

	int		GetQuestState		( int iRoleGUID, int iQuest )
	{
		RoleDataEx* pRole = Global::GetRoleDataByGUID( iRoleGUID );

		if( pRole == NULL )
			return -1;


		int iQuestField = pRole->CheckQuestList_IsGotQuest( iQuest );

		if( iQuestField != -1 )
			return pRole->PlayerBaseData->Quest.QuestLog[ iQuestField ].iMode;

		return -1;
	}

	// 暫時設定 LUA 處理結果用
	void SetQuestState( int iRoleGUID, int iQuest, int iResult )
	{
		RoleDataEx* pRole = Global::GetRoleDataByGUID( iRoleGUID );
		//pRole->TempData.
		if( pRole == NULL )
		{
			Print( LV2 , "SetQuestState" , "pRole == NULL" );
			return;
		}

		int iQuestField = pRole->CheckQuestList_IsGotQuest( iQuest );

		if( iQuestField != -1 )
		{
			pRole->PlayerBaseData->Quest.QuestLog[ iQuestField ].iMode = iResult;
		}

		// 設定
		CNetSrv_Script::SetQuestState( iRoleGUID, iQuest, iResult );

		//pRole->TempData.QuestTempData.iQuestResult = iResult;		
	}

	//設定某物件所屬陣營
	bool	SetRoleCamp( int GUID , const char* CampName )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( GUID );
		if( Owner == NULL )
			return false;

		CampID_ENUM CampID = RoleDataEx::Camp.GetCampID( CampName );
		if( CampID == EM_CampID_Unknow )
			return false;

		if( CampID == Owner->TempData.AI.CampID )
			return true;

		if( (unsigned)CampID >= _MAX_CAMP_COUNT_ )
			return false;

		Owner->TempData.AI.CampID = CampID;

		int iRoomID = Owner->RoomID();

		//DelFromPartition( GUID );
		//AddtoPartition( GUID, -1 );
		NetSrv_OtherChild::SendRangeCampID( Owner , Owner->TempData.AI.CampID );
		return true;
	}
	//設定某區域所用的陣營資訊
	bool	SetZoneCamp( int OrgObjID )
	{
		GameObjDbStructEx*	ObjDB = Global::GetObjDB( OrgObjID );
		if( ObjDB == NULL )
			return false;

		RoleDataEx::Camp.Init( ObjDB );
		return true;
	}

	//播放特定動作
	void PlayMotion( int iGUID, int iMotionID )
	{
		BaseItemObject* pObj = BaseItemObject::GetObj( iGUID );
		if( pObj == NULL )
			return;

		NetSrv_Move::S_SetNPCMotion( pObj, iMotionID , 0 );

		return;
	}

	//播放特定動作
	void PlayMotion_Self( int PlayerID , int PlayMotionObjID , int MotionID )
	{
		BaseItemObject* pObj = BaseItemObject::GetObj( PlayMotionObjID );
		if( pObj == NULL )
			return;

		NetSrv_Move::S_PlayerMotion( PlayerID , PlayMotionObjID , MotionID );
		return;
	}


	//播放特定動作
	void PlayMotionEx( int iGUID, int iMotionID , int iIdleMotionID )
	{
		BaseItemObject* pObj = BaseItemObject::GetObj( iGUID );
		if( pObj == NULL )
			return;

		NetSrv_Move::S_SetNPCMotion( pObj, iMotionID , iIdleMotionID );

		return;
	}

	void SetIdleMotion( int iGUID, int iIdleMotionID )
	{
		BaseItemObject* pObj = BaseItemObject::GetObj( iGUID );
		if( pObj == NULL )
			return;

		NetSrv_Move::S_SetNPCMotion( pObj, iIdleMotionID , iIdleMotionID );

		return;
	}

	void SetDefIdleMotion ( int iObjID, int iMotionID )
	{
		BaseItemObject* pObj = BaseItemObject::GetObj( iObjID );
		if( pObj == NULL )
			return;
		
		NetSrv_Move::S_SetNPCIdleMotion( pObj, iMotionID );
	}

	//播放特定動作
	void SetPosture( int iGUID, int iPostureID, int iMotionID )
	{
		RoleSpriteStatus Val;

		if( iPostureID & 1 ) Val.isUnholster	= true;
		if( iPostureID & 2 ) Val.isWalk			= true;
		if( iPostureID & 4 ) Val.isCombat		= true;
		if( iPostureID & 8 ) Val.isDead			= true;
		if( iPostureID & 16 ) Val.isSpell		= true;
		if( iPostureID & 32 ) Val.isSit			= true;


		BaseItemObject* pObj = BaseItemObject::GetObj( iGUID );
		if( pObj == NULL )
			return;

		NetSrv_MoveChild::SetPosture( pObj, iGUID, Val.value, iMotionID );

		//CNetSrv_Script::PlayMotion( iGUID, pszMotion );
		//NetSrv_Move::S_SetNPCMotion( pObj, iMotionID );

		return;
	}


	void SetFightMode ( int iGUID, int iSearchEnemy, int iMove, int iEscape, int iFight  )
	{
		RoleDataEx* Owner = Global::GetRoleDataByGUID( iGUID );
		if( Owner == NULL )
			return;

		Owner->BaseData.Mode.Searchenemy	= ( 0 != iSearchEnemy );
		Owner->BaseData.Mode.Move			= ( 0 != iMove );
		Owner->BaseData.Mode.Escape			= ( 0 != iEscape );
		Owner->BaseData.Mode.Fight			= ( 0 != iFight );

	}


	void FaceFlag ( int iSourceID, int iFlagGroup, int iFlagID )
	{
		RoleDataEx* Source	= Global::GetRoleDataByGUID( iSourceID );

		if( Source == NULL )
			return;

		FlagPosInfo* FlagObj = FlagPosClass::GetFlag( iFlagGroup , iFlagID );

		if( FlagObj == NULL )
			return;

		float dir = Source->CalDir( float( FlagObj->X - Source->BaseData.Pos.X ), float( FlagObj->Z - Source->BaseData.Pos.Z ) );

		Source->SetValue( EM_RoleValue_Dir, dir , NULL );
	}

	bool ClickToFaceEnabled	( int GItemID , bool Flag )
	{
		BaseItemObject* OwnerObj = BaseItemObject::GetObj( GItemID );
		if( OwnerObj == NULL )
			return false;

		if( OwnerObj->Role()->IsNPC() == false )
			return false;

		NpcAIBaseClass* NpcAI = (NpcAIBaseClass*)OwnerObj->AI();

		//NpcAI->SetMoveToFlagEnabled( Flag );
		//NpcAI->SetClickToFaceEnable( Flag );
		return true;
	}

	//停止系統旗子點移動( Flag = true 停止   false 可移動 )
	bool MoveToFlagEnabled( int GItemID , bool Flag )
	{
		BaseItemObject* OwnerObj = BaseItemObject::GetObj( GItemID );
		if( OwnerObj == NULL )
			return false;

		if( OwnerObj->Role()->IsNPC() == false )
			return false;

		NpcAIBaseClass* NpcAI = (NpcAIBaseClass*)OwnerObj->AI();

		NpcAI->SetMoveToFlagEnabled( Flag );
		return true;
	}
	//定即定位的指令,讓NPC朝向玩家 

	/*
	void FaceObj ( int iSourceID, int iTargetID )
	{
	RoleDataEx* Source	= Global::GetRoleDataByGUID( iSourceID );
	RoleDataEx* Target	= Global::GetRoleDataByGUID( iTargetID );

	if( Source == NULL || Target )
	return;

	float dir = Source->CalDir( Target );

	Source->SetValue( EM_RoleValue_Dir, dir );
	}
	*/
	bool AdjustFaceDir ( int iSouceID, int iTargetID, int iAngle )
	{
		BaseItemObject* SourceObj = Global::GetObj( iSouceID );
		if( SourceObj == NULL )
			return false;

		RoleDataEx* Source = SourceObj->Role();
		RoleDataEx* Target = Global::GetRoleDataByGUID( iTargetID );
		if( Target == NULL )
			return false;

		Source->Pos()->Dir = Source->CalDir( Target );

		float	fDir	= ( Source->Pos()->Dir + (float) iAngle );
		int		Val		= (int) ( fDir / 360 );

		if( Val != 0 )
		{
			fDir = fDir - ( 360 * Val );
		}

		Source->TempData.Move.Tx		= Source->BaseData.Pos.X;
		Source->TempData.Move.Ty		= Source->BaseData.Pos.Y;
		Source->TempData.Move.Tz		= Source->BaseData.Pos.Z;
		Source->TempData.Move.ProcDelay = 0;
		Source->TempData.Move.AddeDelay = 0;
		Source->TempData.Move.Dx 		= 0;
		Source->TempData.Move.Dy 		= 0;
		Source->TempData.Move.Dz 		= 0;

		Source->Pos()->Dir				= fDir;

		NetSrv_MoveChild::MoveObj( Source );
		return true;
	}	


	bool AdjustDir	( int iSouceID, int iAngle )
	{
		BaseItemObject* SourceObj = Global::GetObj( iSouceID );
		if( SourceObj == NULL )
			return false;

		RoleDataEx* Source = SourceObj->Role();

		float	fDir	= ( Source->Pos()->Dir + (float) iAngle );
		int		Val		= (int) ( fDir / 360 );

		if( Val != 0 )
		{
			fDir = fDir - ( 360 * Val );
		}

		Source->TempData.Move.Tx		= Source->BaseData.Pos.X;
		Source->TempData.Move.Ty		= Source->BaseData.Pos.Y;
		Source->TempData.Move.Tz		= Source->BaseData.Pos.Z;
		Source->TempData.Move.ProcDelay = 0;
		Source->TempData.Move.AddeDelay = 0;
		Source->TempData.Move.Dx 		= 0;
		Source->TempData.Move.Dy 		= 0;
		Source->TempData.Move.Dz 		= 0;
		Source->Pos()->Dir				= fDir;

		NetSrv_MoveChild::MoveObj( Source );
		return true;
	}	
	//---------------------------------------------------------------------------------
	bool FaceObj ( int iSouceID, int iTargetID )
	{
		BaseItemObject* SourceObj = Global::GetObj( iSouceID );
		if( SourceObj == NULL )
			return false;

		RoleDataEx* Source = SourceObj->Role();
		RoleDataEx* Target = Global::GetRoleDataByGUID( iTargetID );
		if( Target == NULL )
			return false;

		Source->StopMove();
		//		SourceObj->Path()->ClearPath();

		float NX , NZ , NY;
		Global::SearchAttackNode( Source ,  Target , NX , NY , NZ  );

		//面向計算
		Source->TempData.Move.Tx = NX;
		Source->TempData.Move.Ty = NY;
		Source->TempData.Move.Tz = NZ;
		Source->TempData.Move.ProcDelay = 0;
		Source->TempData.Move.AddeDelay = 0;
		Source->TempData.Move.Dx 		= 0;
		Source->TempData.Move.Dy 		= 0;
		Source->TempData.Move.Dz 		= 0;

		Source->Pos()->Dir = Source->CalDir( Target );

		NetSrv_MoveChild::MoveObj( Source );
		Source->BaseData.Pos.X = NX;
		Source->BaseData.Pos.Y = NY;
		Source->BaseData.Pos.Z = NZ;

		return true;
	}
	//---------------------------------------------------------------------------------
	void AddSpeakOption ( int iRoleID, int iNPCID, const char* pszOption, const char* pszLuaFunc, int iIconID )
	{
		RoleDataEx* pRole = Global::GetRoleDataByGUID( iRoleID );

		if( pRole == NULL )
		{
			Print( LV2 , "AddSpeakOption" , "pRole == NULL" );
			return;
		}

		SpeakOptionStruct option;

		//strcpy( option.szLuaFunc, pszLuaFunc );
		//strcpy( option.szOption, pszOption );

		option.szLuaFunc	= pszLuaFunc;
		option.szOption		= pszOption;
		option.iIconID		= iIconID;

		pRole->PlayerTempData->SpeakInfo.SpeakOption.Push_Back( option );
	}

	void	SetSpeakDetail ( int iRoleID, const char* pszDetail )
	{
		RoleDataEx* pRole = Global::GetRoleDataByGUID( iRoleID );
		if( pRole )
			pRole->PlayerTempData->SpeakInfo.SpeakDetail = pszDetail;
	}

	void LoadQuestOption ( int iRoleID )
	{
		RoleDataEx* pRole = Global::GetRoleDataByGUID( iRoleID );
		if( pRole )
			pRole->PlayerTempData->SpeakInfo.iLoadQuest = 1;
	}

	bool CheckMineEvent( )
	{
		PlotRegStruct* T = LUA_VMClass::ProcReg();

		int	MagicID = T->Arg.GetNumber( "MagicObjID" );
		int MagicLv = T->Arg.GetNumber( "MagicLv" );
		GameObjDbStructEx*  MagicDB	= Global::GetObjDB( MagicID );

		//表示要結束處理
		if( MagicDB == NULL )
			return true;

		MagicItemRunPlot& ItemRunPlot = MagicDB->MagicBase.ItemRunPlot;
		RoleDataEx* Role = Global::GetRoleDataByGUID( T->OwnerID );

		if( Role == NULL )
			return false;

		vector<BaseSortStruct>*	SearchList;
		switch( MagicDB->MagicBase.ItemRunPlot.TargetType )
		{
		case EM_MagicItemRunPlotTargetType_Enemy:
			SearchList = Global::SearchRangeObject(  Role , Role->X()  , Role->Y(), Role->Z() ,  EM_SearchRange_Enemy , 25 );
			break;
		case EM_MagicItemRunPlotTargetType_Friend:
		case EM_MagicItemRunPlotTargetType_Friend_NoSelf:
			SearchList = Global::SearchRangeObject(  Role , Role->X()  , Role->Y(), Role->Z() ,  EM_SearchRange_Friend , 25 );
			break;
		case EM_MagicItemRunPlotTargetType_Player:
		case EM_MagicItemRunPlotTargetType_Player_NoSelf:
			SearchList = Global::SearchRangeObject(  Role , Role->X()  , Role->Y(), Role->Z() ,  EM_SearchRange_Player , 25 );
			break;
		case EM_MagicItemRunPlotTargetType_NPC:
			SearchList = Global::SearchRangeObject(  Role , Role->X()  , Role->Y(), Role->Z() ,  EM_SearchRange_NPC , 25 );
			break;
		case EM_MagicItemRunPlotTargetType_All:
		case EM_MagicItemRunPlotTargetType_All_NoSelf:
			SearchList = Global::SearchRangeObject(  Role , Role->X()  , Role->Y(), Role->Z() ,  EM_SearchRange_All , 25 );
			break;		
		}

		if( SearchList->size() == 0 )
			return false;

		switch( MagicDB->MagicBase.ItemRunPlot.TargetType )
		{
		case EM_MagicItemRunPlotTargetType_Friend_NoSelf:
		case EM_MagicItemRunPlotTargetType_Player_NoSelf:
		case EM_MagicItemRunPlotTargetType_All_NoSelf:
			{
				RoleDataEx* CheckRole = (RoleDataEx* )(*SearchList)[0].Data;

				if( CheckRole->GUID() == T->TargetID )
				{
					SearchList->erase( SearchList->begin() );
				}
			}
			break;
		}

		if( SearchList->size() == 0 )
			return false;

		RoleDataEx* Emeny = (RoleDataEx* )(*SearchList)[0].Data;

		MagicProcInfo  MagicInfo;
		MagicInfo.Init();
		MagicInfo.State = EM_MAGIC_PROC_STATE_PERPARE;
		MagicInfo.TargetID			= Emeny->GUID();				//目標
		MagicInfo.MagicCollectID	= ItemRunPlot.UseMagic;			//法術
		MagicInfo.MagicLv			= MagicLv;
		bool Ret = MagicProcessClass::SysSpellMagic( Role , &MagicInfo );

		if( Ret == false )
			return false;

		Role->LiveTime( 1000 , "CheckMineEvent" );

		//除錯資訊
		Print( Def_PrintLV2 , "CheckMineEvent" , "LivTime(1000) CheckMineEvent   Name=%s DBID =%d Function=%s" ,  Global::GetRoleName_ASCII( Role ).c_str() , Role->DBID() , T->FuncName );


		return true;
	}
	//---------------------------------------------------------------------------------
	int	GetPartyID		( int iRoleID, int iIndex )
	{
		RoleDataEx*	pMember = NULL;
		RoleDataEx*	pRole	= Global::GetRoleDataByGUID( iRoleID );

		if( pRole == NULL )
			return 0;

		PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( pRole->BaseData.PartyID );	

		if( Party != NULL )
		{
			if( iIndex != -1 )
			{
				int iID = iIndex - 1;
				if( (int)Party->Member.size() > iID && iID >= 0 )
				{
					pMember = Global::GetRoleDataByGUID( Party->Member[ iID ].GItemID );

					if( pMember )
						return pMember->GUID();
					else
						return 0;
				}
				else
					return 0;
			}
			else
				return int( Party->Member.size() );
		}
		else
			return 0;		
	}
	//---------------------------------------------------------------------------------
	bool CheckQuest			( int iRoleID, int iQuestID, int iMode )
	{
		bool		bResult = false;
		RoleDataEx*	pMember = NULL;
		RoleDataEx*	pRole = Global::GetRoleDataByGUID( iRoleID );

		if( pRole == NULL )
		{
			Print( LV2 , "CheckQuest" , "pRole == NULL" );
			return false;
		}

		// 檢查指定人物

		if( iMode == 0 ) // 個人和隊伍
		{


			if( pRole->CheckQuestList_IsGotQuest( iQuestID ) != -1 )
			{
				bResult = true;
			}

			PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( pRole->BaseData.PartyID );	

			if( Party != NULL )
			{
				for( std::vector< PartyMemberInfoStruct >::iterator it = Party->Member.begin(); it != Party->Member.end(); it++ )
				{

					pMember = Global::GetRoleDataByGUID(  (*it).ItemID );

					if( pMember != NULL && pMember->CheckQuestList_IsGotQuest( iQuestID ) != -1 )
					{
						// 檢查和主要檢查者的距離
						if( pRole->Length( pMember ) > _MAX_PARTY_SHARE_RANGE_ )
						{
							bResult = true;
						}
					}
				}
			}
		}
		else
			if( iMode == 1 )
			{

			}

			return bResult;
	}

	//---------------------------------------------------------------------------------
	const char* GetString( const char* szKeyName )
	{
		static char buff[2048];
		sprintf_s( buff , sizeof(buff) , "[%s]", szKeyName );
		return buff;
		//return g_ObjectData->GetString( szKeyName );
	}
	//---------------------------------------------------------------------------------
	void CloseSpeak( int iRoleID )
	{
		CNetSrv_Script::CloseSpeak( iRoleID );
	}
	//---------------------------------------------------------------------------------
	bool CastSpell( int iRoleID, int iTargetID, int iMagicID )
	{
		RoleDataEx*		pTarget = Global::GetRoleDataByGUID( iTargetID );
		if( pTarget == NULL )
			return false;
		
		RoleDataEx* pOwner = Global::GetRoleDataByGUID( iRoleID );
		if( pOwner == NULL )
			return false;
		if( pOwner != pTarget )
			pOwner->BaseData.Pos.Dir = pOwner->CalDir( pTarget );

		return MagicProcessClass::SpellMagic( iRoleID, iTargetID, pTarget->Pos()->X, pTarget->Pos()->Y, pTarget->Pos()->Z, iMagicID, 0 );
	}
	//---------------------------------------------------------------------------------
	bool CastSpellLv( int iRoleID, int iTargetID, int iMagicID , int iMagicLv )
	{
		RoleDataEx*		pTarget = Global::GetRoleDataByGUID( iTargetID );
		if( pTarget == NULL )
			return false;

		RoleDataEx* pOwner = Global::GetRoleDataByGUID( iRoleID );
		if( pOwner == NULL )
			return false;
		if( pOwner != pTarget )
			pOwner->BaseData.Pos.Dir = pOwner->CalDir( pTarget );

		return MagicProcessClass::SpellMagic( iRoleID, iTargetID, pTarget->Pos()->X, pTarget->Pos()->Y, pTarget->Pos()->Z, iMagicID, iMagicLv );
	}
	bool	CastSpellPos		( int iRoleID, float X , float Y , float Z , int iMagicID , int iMagicLv )
	{
		return MagicProcessClass::SpellMagic( iRoleID, iRoleID, X, Y, Z, iMagicID, iMagicLv );
	}

	//---------------------------------------------------------------------------------
	void	SetScriptFlag		( int iRoleID, int iFlagID, int iVal )
	{
		RoleDataEx*		pRole = Global::GetRoleDataByGUID( iRoleID );
		if( pRole == NULL )
			return ;
		if( iVal == 0 )
		{
			pRole->DelKeyItem( iFlagID );
		}
		else
		{
			pRole->AddKeyItem( iFlagID );
			//pRole->BaseData.KeyItem.SetFlagOn( iFlagID );
		}
	}
	//---------------------------------------------------------------------------------
	bool	CheckScriptFlag		( int iRoleID, int iFlagID )
	{
		RoleDataEx*		pRole = Global::GetRoleDataByGUID( iRoleID );
		if( pRole == NULL )
			return false;
		return pRole->BaseData.KeyItem.GetFlag( RoleDataEx::KeyItemObjToID( iFlagID ) );
	}
	//---------------------------------------------------------------------------------
	bool	CheckCardFlag		( int iRoleID, int iFlagID )
	{
		RoleDataEx*		pRole = Global::GetRoleDataByGUID( iRoleID );
		if( pRole == NULL )
			return false;

		return pRole->PlayerBaseData->Card.GetFlag( iFlagID - Def_ObjectClass_Card );
	}
	//---------------------------------------------------------------------------------
	bool	CheckCompleteQuest		( int iRoleID, int iQuestID )
	{
		RoleDataEx*		pRole = Global::GetRoleDataByGUID( iRoleID );
		if( pRole == NULL )
			return false;
		return pRole->CheckQuestHistory_IsComplete( iQuestID );
	}
	//---------------------------------------------------------------------------------
	void SetPosByFlag( int iRoleID, int iFlagObjID, int iFlagID )
	{
		//RoleDataEx*		pObj		= Global::GetRoleDataByGUID( iRoleID );
		BaseItemObject*		pObj	= BaseItemObject::GetObj( iRoleID );
		FlagPosInfo*	pFlagObj	= FlagPosClass::GetFlag( iFlagObjID , iFlagID );
		bool			IsLiving	= false;

		if( pObj == NULL || pFlagObj == NULL )
			return ;

		if( pObj->Role()->TempData.AttachObjID > 0 )
			return;

		//NetSrv_MoveChild::SetObjPos( pObj , pFlagObj->X , pFlagObj->Y , pFlagObj->Z, pFlagObj->Dir );
		Global::CheckClientLoading_SetPos( pObj->GUID() , pFlagObj->X , pFlagObj->Y , pFlagObj->Z, pFlagObj->Dir );
		/*
		if( pObj->SecRoomID() != -1 )	// 要是該物件仍存在於 Zone 的話先刪除再搬移
		{
		DelFromPartition( iRoleID );
		IsLiving = true;
		}

		pObj->BaseData.Pos.X	= pFlagObj->X;
		pObj->BaseData.Pos.Y	= pFlagObj->Y;
		pObj->BaseData.Pos.Z	= pFlagObj->Z;
		pObj->BaseData.Pos.Dir = pFlagObj->Dir;


		if( IsLiving != false )
		{
		AddtoPartition( iRoleID, pObj->BaseData.RoomID );
		}
		*/
	}
	//---------------------------------------------------------------------------------
	void StopSoundToPlayer( int iRoleID, int iSoundID )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iRoleID );	
		if( pObj == NULL )	return;

		PG_Script_LtoC_StopSound	Packet;

		Packet.iObjID		=	pObj->GUID();
		Packet.iSoundID		=	iSoundID;

		Global::SendToCli_ByGUID( iRoleID, sizeof( Packet ) , &Packet );
	}
	//---------------------------------------------------------------------------------
	void StopSound( int iRoleID, int iSoundID )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iRoleID );	
		if( pObj == NULL )	return;

		Global::m_iSoundID++;

		PG_Script_LtoC_StopSound	Packet;

		Packet.iObjID		=	pObj->GUID();
		Packet.iSoundID		=	iSoundID;

		RoleDataEx* Owner = pObj->Role();

		PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

		int			i , j;
		PlayID*		TopID;

		for( i = 0 ; Block[i] != NULL ; i++ )
		{
			TopID = Block[i]->Begin;

			for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
			{
				BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
				if( OtherObj == NULL )
					continue;
				if( OtherObj->Role()->IsPlayer() == false )
					continue;

				Global::SendToCli_ByGUID( OtherObj->GUID() , sizeof( Packet ) , &Packet );
			}
		} 
	}
	//---------------------------------------------------------------------------------
	int PlaySoundToPlayer ( int iRoleID, const char* pszFile, bool bLoop )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iRoleID );	
		if( pObj == NULL )	return 0;

		PG_Script_LtoC_PlaySound Packet;

		Global::m_iSoundID++;

		Packet.iObjID			= pObj->GUID();
		Packet.sSoundPath		= pszFile;
		Packet.bLoop			= bLoop;
		Packet.iSoundID			= Global::m_iSoundID;
		
		Global::SendToCli_ByGUID( iRoleID, sizeof( Packet ) , &Packet );

		return Global::m_iSoundID;
	}
	//---------------------------------------------------------------------------------
	int PlaySound ( int iRoleID, const char* pszFile, bool bLoop )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iRoleID );	
		if( pObj == NULL )	return 0;

		Global::m_iSoundID++;

		PG_Script_LtoC_PlaySound Packet;

		Packet.iObjID			= pObj->GUID();
		Packet.sSoundPath		= pszFile;
		Packet.bLoop			= bLoop;
		Packet.iSoundID			= Global::m_iSoundID;

		RoleDataEx* Owner = pObj->Role();

		PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

		int			i , j;
		PlayID*		TopID;

		for( i = 0 ; Block[i] != NULL ; i++ )
		{
			TopID = Block[i]->Begin;

			for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
			{
				BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
				if( OtherObj == NULL )
					continue;
				if( OtherObj->Role()->IsPlayer() == false )
					continue;

				Global::SendToCli_ByGUID( OtherObj->GUID() , sizeof( Packet ) , &Packet );
			}
		}   
		return Global::m_iSoundID;
	}
	//---------------------------------------------------------------------------------
	int	PlayMusic( int iRoleID, const char* pszFile, bool bLoop )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iRoleID );	
		if( pObj == NULL )	return 0;

		Global::m_iSoundID++;

		PG_Script_LtoC_PlayMusic Packet;

		Packet.iObjID			= pObj->GUID();
		Packet.sSoundPath		= pszFile;
		Packet.bLoop			= bLoop;
		Packet.iSoundID			= Global::m_iSoundID;

		RoleDataEx* Owner = pObj->Role();

		PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

		int			i , j;
		PlayID*		TopID;

		for( i = 0 ; Block[i] != NULL ; i++ )
		{
			TopID = Block[i]->Begin;

			for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
			{
				BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
				if( OtherObj == NULL )
					continue;
				if( OtherObj->Role()->IsPlayer() == false )
					continue;

				Global::SendToCli_ByGUID( OtherObj->GUID() , sizeof( Packet ) , &Packet );
			}
		}   
		return Global::m_iSoundID;;
	}
	//---------------------------------------------------------------------------------
	int	PlayMusicToPlayer( int iRoleID, const char* pszFile, bool bLoop )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iRoleID );	
		if( pObj == NULL )	return 0;

		PG_Script_LtoC_PlayMusic Packet;

		Global::m_iSoundID++;

		Packet.iObjID			= pObj->GUID();
		Packet.sSoundPath		= pszFile;
		Packet.bLoop			= bLoop;
		Packet.iSoundID			= Global::m_iSoundID;

		Global::SendToCli_ByGUID( iRoleID, sizeof( Packet ) , &Packet );

		return Global::m_iSoundID;
	}
	//---------------------------------------------------------------------------------
	int	Play3DSound	( int iRoleID, const char* pszFile, bool bLoop )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iRoleID );	
		if( pObj == NULL )	return 0;

		Global::m_iSoundID++;

		PG_Script_LtoC_Play3DSound Packet;

		Packet.iObjID			= pObj->GUID();
		Packet.sSoundPath		= pszFile;
		Packet.bLoop			= bLoop;
		Packet.iSoundID			= Global::m_iSoundID;

		RoleDataEx* Owner = pObj->Role();

		PlayIDInfo**	Block = Global::PartSearch( Owner , _Def_View_Block_Range_ );

		int			i , j;
		PlayID*		TopID;

		for( i = 0 ; Block[i] != NULL ; i++ )
		{
			TopID = Block[i]->Begin;

			for( j = 0 ; TopID != NULL ; TopID = TopID->Next )
			{
				BaseItemObject* OtherObj = BaseItemObject::GetObj( TopID->ID );
				if( OtherObj == NULL )
					continue;
				if( OtherObj->Role()->IsPlayer() == false )
					continue;

				Global::SendToCli_ByGUID( OtherObj->GUID() , sizeof( Packet ) , &Packet );
			}
		}   
		return Global::m_iSoundID;
	}
	//---------------------------------------------------------------------------------
	int	Play3DSoundToPlayer	( int iRoleID, const char* pszFile, bool bLoop )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iRoleID );	
		if( pObj == NULL )	return 0;

		PG_Script_LtoC_Play3DSound Packet;

		Global::m_iSoundID++;

		Packet.iObjID			= pObj->GUID();
		Packet.sSoundPath		= pszFile;
		Packet.bLoop			= bLoop;
		Packet.iSoundID			= Global::m_iSoundID;

		Global::SendToCli_ByGUID( iRoleID, sizeof( Packet ) , &Packet );

		return Global::m_iSoundID;
	}
	//---------------------------------------------------------------------------------
	//設定玩家裝備
	bool	SetRoleEq_Player	( int PlayerID , int Part , int ItemID )
	{
		RoleDataEx*			Owner	= Global::GetRoleDataByGUID( PlayerID );
		GameObjDbStructEx*  ItemDB	= Global::GetObjDB( ItemID );

		if( Owner == NULL  )
			return false;

		
		EQWearPosENUM EquipPos = (EQWearPosENUM)Part;
		//Owner->BaseData.EQ.Item[ EquipPos ].Init();

		ItemFieldStruct* EQItem = Owner->GetEqItem(EquipPos);
		if( EQItem == NULL )
			return false;

		EQItem->Init();

		if( ItemDB != NULL )
		{
			Global::NewItemInit( *EQItem , ItemID , 0 ) ;

			EQItem->OrgObjID		= ItemID;
			EQItem->Count			= ItemDB->MaxHeap;
			EQItem->ImageObjectID	= ItemID;
		}		

		Owner->Net_FixItemInfo_EQ( EquipPos );
		Owner->TempData.UpdateInfo.Eq = true;

		NetSrv_MoveChild::Send_LookEq( Owner );
		return true;
	}
	//---------------------------------------------------------------------------------
	// 設定 NPC 裝備
	void SetRoleEquip( int iRoleID, int iPartID, int iItemID, int iCount )
	{
		RoleDataEx*			pObj	= Global::GetRoleDataByGUID( iRoleID );
		GameObjDbStructEx*  pItemDB	= Global::GetObjDB( iItemID );

		if( pObj == NULL || pItemDB == NULL )
			return;

		if( pObj->IsPlayer() )
			return;

		EQWearPosENUM emEquipPos = (EQWearPosENUM)iPartID;

		Global::NewItemInit( pObj->BaseData.EQ.Item[ emEquipPos ] , iItemID , 0 ) ;

		pObj->BaseData.EQ.Item[ emEquipPos ].OrgObjID	= iItemID;
		pObj->BaseData.EQ.Item[ emEquipPos ].Count		= iCount;
		pObj->BaseData.EQ.Item[ emEquipPos ].ImageObjectID = iItemID;

		pObj->TempData.UpdateInfo.Eq = true;


		NetSrv_MoveChild::Send_LookEq( pObj );
	}

	//---------------------------------------------------------------------------------

	void	MoveToPathPoint ( int iObjID , int PathID )
	{
		BaseItemObject* pObj	= BaseItemObject::GetObj( iObjID );


		if( pObj == NULL || pObj->Role()->IsNPC() == false )
			return;

		NpcAIBaseClass*					pAI			= (NpcAIBaseClass*)pObj->AI();		
		NPCMoveFlagAIStruct*			pMoveStatus = pAI->GetMoveStatus();
		vector< NPC_MoveBaseStruct >*	pMoveList	= pObj->NPCMoveInfo()->MoveInfo();
		RoleDataEx*						Owner = pObj->Role();
		if( PathID >= (int)pMoveList->size() )
			return;

		pObj->Role()->StopMove();
		//		pObj->Path()->ClearPath();

		NPC_MoveBaseStruct				MoveInfo	= (*pMoveList)[ PathID ];

		pMoveStatus->Type		= EM_NPCMoveFlagAIType_Moving;
		pMoveStatus->Index		= PathID;
		pMoveStatus->TargetX	= MoveInfo.X;
		pMoveStatus->TargetY	= MoveInfo.Y;
		pMoveStatus->TargetZ	= MoveInfo.Z;
		pMoveStatus->TargetDir	= MoveInfo.Dir;
		pMoveStatus->CountDown	= RoleDataEx::G_SysTime + 100;

		pObj->Path()->Target( pMoveStatus->TargetX , pMoveStatus->TargetY , pMoveStatus->TargetZ , Owner->X() , Owner->Y() , Owner->Z() );

	}
	//---------------------------------------------------------------------------------
	void ScriptMessage ( int SayID , int iTarget, int iType, const char* Str, const char* szColor )
	{
		int iColor = HexStrToUInt( szColor );
		NetSrv_Talk::ScriptMessage ( SayID, iTarget, iType, Str, iColor );
	}
	//---------------------------------------------------------------------------------
	int RandRange ( int iMin, int iMax )
	{
		if( iMin <= 0 || iMin > iMax )
			return 0;

		return iMin + ( rand() % ( ( iMax - iMin ) + 1 ) );
	}
	//---------------------------------------------------------------------------------
	bool	CheckAcceptQuest	( int iRoleID, int iQuestID )
	{
		RoleDataEx*		pRole = Global::GetRoleDataByGUID( iRoleID );
		if( pRole == NULL )
			return false;

		if( pRole->CheckQuestList_IsGotQuest( iQuestID ) == -1 )
			return false;
		else
			return true;
	}
	//---------------------------------------------------------------------------------
	int GetDistance( int iOwnerID, int iTargetID )
	{
		RoleDataEx*		pOwner	= Global::GetRoleDataByGUID( iOwnerID );
		RoleDataEx*		pTarget = Global::GetRoleDataByGUID( iTargetID );
		if( pOwner == NULL || pTarget == NULL )
			return -1;

		int iDistance = (int)( pOwner->Length( pTarget ) );

		return iDistance;
	}
	//---------------------------------------------------------------------------------
	bool CheckDistance( int iOwnerID, int iTargetID, int iRange )
	{
		RoleDataEx*		pOwner	= Global::GetRoleDataByGUID( iOwnerID );
		RoleDataEx*		pTarget = Global::GetRoleDataByGUID( iTargetID );
		if( pOwner == NULL || pTarget == NULL )
			return false;

		int iDistance = (int)( pOwner->Length( pTarget ) );

		if( iDistance <= iRange && iDistance >= 0 )
			return true;
		else
			return false;
	}
	//---------------------------------------------------------------------------------
	void CallSpeakLua( int iNPCID, int iPlayerID, const char* pszSpeakLua )
	{
		RoleDataEx*		pNPC	= Global::GetRoleDataByGUID( iNPCID );
		RoleDataEx*		pPlayer	= Global::GetRoleDataByGUID( iPlayerID );

		if( pNPC && pPlayer && strlen( pszSpeakLua ) != 0 )
		{
			pPlayer->PlayerTempData->SpeakInfo.Reset();
			pPlayer->PlayerTempData->SpeakInfo.iSpeakNPCID = iNPCID;

			vector< MyVMValueStruct > vecArg;
			LUA_VMClass::PCallByStrArg( pszSpeakLua, iPlayerID, iNPCID, &vecArg );

			CNetSrv_Script_Child::SpeakOption( iPlayerID );
		}

	}
	//---------------------------------------------------------------------------------
	void LockObj( int iObjID, int iLockType, int iLockID )
	{
		RoleDataEx*		pObj	= Global::GetRoleDataByGUID( iObjID );
		RoleDataEx*		pLocker	= NULL;

		pLocker	= Global::GetRoleDataByGUID( iLockID );

		if( pLocker == NULL || pObj == NULL )
			return;

		if( !pLocker->IsPlayer() )
			return;

		//		if( pObj )
		{
			pObj->PlayerTempData->ScriptInfo.emLockType	= (ENUM_SCRIPTSTATUS_LOCKTYPE) iLockType;
			pObj->PlayerTempData->ScriptInfo.iLockDBID		= pLocker->DBID();
		}		

		if( pLocker )
		{
			NetSrv_MoveChild::_Send_Relation( pLocker, pObj );
		}

		//static void _Send_Relation( RoleDataEx* SendObj , RoleDataEx* Target );


	}
	//---------------------------------------------------------------------------------
	void DisableQuest( int iObjID, bool bDisableQuest )
	{
		RoleDataEx*		pObj	= Global::GetRoleDataByGUID( iObjID );
		if( pObj == NULL )
			return;
		//pObj->TempData.Attr.Action.DisableQuest = bDisableQuest;
		pObj->SetDisableQuest( bDisableQuest );
	}
	//---------------------------------------------------------------------------------
	void SetMinimapIcon( int iObjID, int iIconID )
	{
		RoleDataEx*		pObj	= Global::GetRoleDataByGUID( iObjID );
		if( pObj == NULL )
			return;
		pObj->TempData.iMinimapIcon = iIconID;
		NetSrv_Move::SendObjMinimapIcon( iObjID, iIconID );
	}
	//---------------------------------------------------------------------------------
	void AddBorderPage( int iObjID, const char* sPageText )
	{
		//RoleDataEx*		pObj	= Global::GetRoleDataByGUID( iObjID );
		//return pObj->TempData.Magic.UseItem;

		//if( pObj->TempData.ScriptBorder.iPageCount < _MAX_SPEAKOPTION_COUNT_ )
		//{
		//pObj->TempData.ScriptBorder.sBorderPage[ pObj->TempData.ScriptBorder.iPageCount ] = sPageText;
		//pObj->TempData.ScriptBorder.sBorderPage += sPageText;
		//pObj->TempData.ScriptBorder.sBorderPage += 0;
		//pObj->TempData.ScriptBorder.iPageCount++;
		CNetSrv_Script::AddBorderPage( iObjID, sPageText );
		//
	}
	//---------------------------------------------------------------------------------
	void ClearBorder( int iObjID )
	{
		RoleDataEx*		pObj	= Global::GetRoleDataByGUID( iObjID );
		if( pObj == NULL )
			return;
		pObj->TempData.ScriptBorder.Init();
		CNetSrv_Script::ClearBorder( iObjID );
	}
	//---------------------------------------------------------------------------------
	int GetUseItemGUID( int iObjID )
	{
		RoleDataEx*		pObj	= Global::GetRoleDataByGUID( iObjID );
		if( pObj == NULL )
			return 0;
		return pObj->TempData.Magic.UseItem;
	}
	//---------------------------------------------------------------------------------
	const char* GetObjNameByGUID( int iGUID )
	{
		//RoleDataEx*		pObj = Global::GetRoleDataByGUID( iObjID );

		GameObjDbStructEx*  pItemDB	= Global::GetObjDB( iGUID );
		if( pItemDB == NULL )
			return "";

		return pItemDB->Name;
	}
	//---------------------------------------------------------------------------------
	void ShowBorder( int iObjID, int iQuestID, const char* sBorderTitle, const char* sTexture )
	{
		RoleDataEx*		pObj = Global::GetRoleDataByGUID( iObjID );
		if( pObj == NULL )
			return;
		pObj->TempData.ScriptBorder.iQuestID = iQuestID;
		CNetSrv_Script::ShowBorder( iObjID, iQuestID, sBorderTitle, sTexture );
	}
	//---------------------------------------------------------------------------------
	const char* GetQuestDesc( int iQuestID )
	{
		static string sKey;

		GameObjDbStructEx*  pItemDB	= Global::GetObjDB( iQuestID );
		if( pItemDB == NULL )
			return "";

		char szKey[256];
		sprintf_s( szKey , sizeof(szKey), "[Sys%d_szquest_desc]", iQuestID );

		sKey = szKey;				
		return sKey.c_str();
	}
	//---------------------------------------------------------------------------------
	const char* GetQuestDetail( int iQuestID, int iType )
	{
		static string sKey;

		GameObjDbStructEx*  pItemDB	= Global::GetObjDB( iQuestID );

		if( pItemDB == NULL )
			return "";

		char szKey[256];

		switch( iType )
		{
		case 1: sprintf_s( szKey,sizeof(szKey), "[Sys%d_szquest_accept_detail]", iQuestID );		break;
		case 2: sprintf_s( szKey,sizeof(szKey), "[Sys%d_szquest_uncomplete_detail]", iQuestID );	break;
		case 3: sprintf_s( szKey,sizeof(szKey), "[Sys%d_szquest_complete_detail]", iQuestID );		break;
		}

		sKey = szKey;

		return sKey.c_str();
	}
	//---------------------------------------------------------------------------------
	void SetFollow( int iOwner, int iTarget )
	{
		RoleDataEx*		pOwener = Global::GetRoleDataByGUID( iOwner );
		RoleDataEx*		pTarget = Global::GetRoleDataByGUID( iTarget );

		if( pOwener == NULL )
			return;

		pOwener->SetFollow( pTarget );
	}
	//---------------------------------------------------------------------------------
	//殺死某物件
	bool KillID		( int KillerID , int DeadID )
	{
		RoleDataEx* Dead = Global::GetRoleDataByGUID( DeadID );
		RoleDataEx* Kill = Global::GetRoleDataByGUID( KillerID );
		if( Dead == NULL || Dead->IsDead() || Kill == NULL )
			return false;

		Dead->AddHP( Kill , int( -1*Dead->MaxHP() - 1000 ) );
		return true;
	}
	//---------------------------------------------------------------------------------
	int BeginCastBarEvent ( int iObjID, int iClickObjID, const char* pszString, int iTime, int iMotionID, int iMotionEndID, int iFlag, const char* pszLuaEvent )
	{
		RoleDataEx*		pOwner = Global::GetRoleDataByGUID( iObjID );

		if( pOwner == NULL )
			return 0;

		int				iTick	= iTime * 100;

		if( CNetSrv_Gather_Child::SetCast( pOwner, iClickObjID, iTick, CNetSrv_Script_Child::OnCast_ScriptCast, pszLuaEvent ) != false )
		{
			pOwner->TempData.IdleMotionID	= iMotionID;
			CNetSrv_Script::ScriptCast( iObjID, pszString, iTick, iMotionID, iMotionEndID );
			return 1;
		}

		return 0;
	}
	//---------------------------------------------------------------------------------
	int BeginCastBar ( int iObjID, const char* pszString, int iTime, int iMotionID, int iMotionEndID, int iFlag )
	{
		RoleDataEx*		pOwner = Global::GetRoleDataByGUID( iObjID );

		if( pOwner == NULL )
			return 0;

		int				iTick	= iTime * 100;

		if( CNetSrv_Gather_Child::SetCast( pOwner, 0, iTick, CNetSrv_Script_Child::OnCast_ScriptCast, "" ) != false )
		{
			pOwner->TempData.IdleMotionID	= iMotionID;
			CNetSrv_Script::ScriptCast( iObjID, pszString, iTick, iMotionID, iMotionEndID );
			return 1;
		}

		return 0;	
	}
	//---------------------------------------------------------------------------------
	int CheckCastBar ( int iObjID )
	{
		RoleDataEx*		pOwner = Global::GetRoleDataByGUID( iObjID );

		if( pOwner == NULL )
			return 0;

		return 	pOwner->PlayerTempData->CastData.iContext;
	}
	//---------------------------------------------------------------------------------
	int CheckCastBarStatus ( int iObjID )
	{
		RoleDataEx*		pOwner = Global::GetRoleDataByGUID( iObjID );

		if( pOwner == NULL )
			return 0;

		return 	pOwner->PlayerTempData->CastData.iEventHandle;
	}
	//---------------------------------------------------------------------------------
	void EndCastBar ( int iObjID, int iResult )
	{
		RoleDataEx*		pOwner = Global::GetRoleDataByGUID( iObjID );

		if( pOwner == NULL )
			return;

		CNetSrv_Script::ScriptCastResult( iObjID, iResult );
		pOwner->PlayerTempData->CastData.Clear();
	}
	//---------------------------------------------------------------------------------
	void SetCursorType( int iObjID, int iCursorID )
	{
		RoleDataEx*		pOwner	= Global::GetRoleDataByGUID( iObjID );
		if( pOwner != NULL )
		{
			pOwner->TempData.CursorType	= iCursorID;
			NetSrv_Other::SC_Range_BaseObjValueChange( iObjID , EM_ObjValueChange_CursorType , "" , iCursorID );
		}
	}
	//---------------------------------------------------------------------------------
	void GenerateMine	( int iRoomID, int iFlagID, int iMineID,	int iMineCount )
	{
		CNetSrv_Gather_Child::GenerateMine( iRoomID, iFlagID, iMineID, iMineCount );
	}
	//---------------------------------------------------------------------------------
	void OpenGuildWarBid( int iPlayer, int iNPC )
	{
		RoleDataEx*		pNpc	= Global::GetRoleDataByGUID( iNPC );
		RoleDataEx*		pRole	= Global::GetRoleDataByGUID( iPlayer );
		if( pNpc->TempData.CrystalID != 0 )
		{
			NetSrv_Guild::SD_GuildFightInfoRequest( pRole->DBID(), pNpc->TempData.CrystalID  );
		}		
	}

	void SetCrystalID( int iObjID, int iCrystalID )
	{
		RoleDataEx*		pRole	= Global::GetRoleDataByGUID( iObjID );
		pRole->TempData.CrystalID = iCrystalID;
	}

	int GetLocaltime( int iTimeVal, int iTimeID )
	{
		__time32_t				NowTime;
		struct	tm*				gmt;

		if( iTimeID != 0 )
		{
			iTimeVal = (int)iTimeVal - RoleDataEx::G_TimeZone*60*60;	// 先換算成 UTC
			gmt = _localtime32( (__time32_t*)&iTimeVal );
		}
		else
		{
			gmt = _localtime32( (__time32_t*)&iTimeVal );
		}

		switch( iTimeID )
		{
		case 0: 
			{
				_time32( &NowTime );
				gmt = _localtime32( &NowTime );
				NowTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;
				return (int)NowTime;
			} break;


		case 1:	return gmt->tm_hour;	break; /* hours since midnight - [0,23] */
		case 2:	return gmt->tm_min;		break; /* minutes after the hour - [0,59] */
		case 3:	return gmt->tm_sec;		break; // sec
		case 4:	return gmt->tm_mon;		break; /* months since January - [0,11] */
		case 5:	return gmt->tm_mday;	break; /* day of the month - [1,31] */
		case 6:	return gmt->tm_wday;	break; /* days since Sunday - [0,6] */
		
			
		}

		return 0;
	}

	int GetSystime( int iTimeID )
	{
		__time32_t				NowTime;
		struct	tm*				gmt;

		_time32( &NowTime );
		gmt = _localtime32( &NowTime );

		switch( iTimeID )
		{
		case 0: 
			{
				_time32( &NowTime );
				gmt = _localtime32( &NowTime );
				NowTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;
				return (int)NowTime;
			} break;
			
		case 1:	return gmt->tm_hour;	break; /* hours since midnight - [0,23] */
		case 2:	return gmt->tm_min;		break; /* minutes after the hour - [0,59] */
		case 3:	return gmt->tm_mon;		break; /* months since January - [0,11] */
		case 4:	return gmt->tm_mday;	break; /* day of the month - [1,31] */
		case 5:	return gmt->tm_wday;	break; /* days since Sunday - [0,6] */
		}

		return 0;
	}
	//-----------------------------------------------------------------------------------
	bool DeleteQuest ( int iObjID, int iQuestID )
	{
		BaseItemObject	*pObj	= BaseItemObject::GetObj( iObjID );	
		RoleDataEx*		pRole	= Global::GetRoleDataByGUID( iObjID );

		int iQuestField = -1;

		if( pRole == NULL || pRole->IsPlayer() == false )
			return false;
		
		for( int index = 0; index < _MAX_QUEST_COUNT_; index++ )
		{
			if( pRole->PlayerBaseData->Quest.QuestLog[ index ].iQuestID == iQuestID )
			{
				iQuestField = index;
				break;
			}
		}

		CNetSrv_Script::m_pThis->On_QuestDelete( pObj, iQuestField, iQuestID );
		
		return true;
	}
	//-----------------------------------------------------------------------------------
	int	GetBodyFreeSlot	( int iObjID )
	{
		RoleDataEx*		pRole	= Global::GetRoleDataByGUID( iObjID );

		if( pRole != NULL )
		{
			return pRole->GetBodyFreeSlot();
		}
		return 0;
	}
	//-----------------------------------------------------------------------------------
	void ResetObjDailyQuest	( int iObjID )
	{
		RoleDataEx*		pRole	= Global::GetRoleDataByGUID( iObjID );
		if( pRole->IsPlayer() )
		{
			// 重置每日任務
			pRole->PlayerBaseData->Quest.QuestDaily.ReSet();

			// 重置群組任務
			for( int i=0; i<DF_QUESTDAILY_MAXGROUP; i++ )
			{
				pRole->PlayerBaseData->Quest.iQuestGroup[i] = 0;
			}

			{
				__time32_t NowTime;
				struct	tm*				gmt;
				_time32( &NowTime );
				gmt = _localtime32( &NowTime );

				pRole->PlayerBaseData->Quest.LastCompleteTime = (int)_mktime32( gmt ) + RoleDataEx::G_TimeZone*60*60;;
			}

			// 發送封包通知 Client 重置任務
			CNetSrv_Script::ResetDailyQuest( iObjID, 1 );
		}
	}

	int GetDailyQuestCount( int iObjID )
	{
		RoleDataEx*		pRole	= Global::GetRoleDataByGUID( iObjID );

		if( pRole->IsPlayer() )
		{
			int iCount = 0;
			int iTotal = pRole->PlayerBaseData->Quest.QuestDaily.Size();

			for( int i=0; i<iTotal;i++ )
			{
				if( pRole->PlayerBaseData->Quest.QuestDaily.GetFlag( i ) == true )
				{
					iCount++;
				}
			}

			return iCount;
		}
		

		return 0;
	}
	//-----------------------------------------------------------------------------------
	void SendBGInfoToClient	( int iBGID, int iClientID )
	{
		RoleDataEx*		pRole	= Global::GetRoleDataByGUID( iClientID );

		if( pRole != NULL )
			CNetSrv_BattleGround::SL_GetBGInfo( iBGID, pRole->DBID() );
	}
	//-----------------------------------------------------------------------------------
	void SetBattleGroundObjClickState( int iObjID, int iTeamID, int iClickAble )
	{
		CNetSrv_BattleGround_Child::SetBattleGroundObjClickState( iObjID, iTeamID, iClickAble );
	}
	//-----------------------------------------------------------------------------------
	bool SetRoleCampID( int iObjID, int iID )
	{
		RoleDataEx* pRole = Global::GetRoleDataByGUID( iObjID );
		if( pRole == NULL )
			return false;

		if( (unsigned)iID >= _MAX_CAMP_COUNT_ )
			return false;

		if( pRole->TempData.AI.CampID == (CampID_ENUM)iID )
			return true;

		//紀錄玩家切換陣營的資料
		if( pRole->IsPlayer() )
			Print( LV2 , "SetRoleCampID" , " Set Player Camp ID RoleName=%s RoleDBID =%d CampID=%d" , pRole->RoleName() , pRole->DBID() , iID );

		pRole->TempData.AI.CampID = (CampID_ENUM)iID;

		int iRoomID = pRole->RoomID();

		//DelFromPartition( iObjID );
		//AddtoPartition( iObjID, iRoomID );
		NetSrv_OtherChild::SendRangeCampID( pRole , pRole->TempData.AI.CampID );
		return true;
	}
	//-----------------------------------------------------------------------------------
	bool ResetRoleCampID( int ObjID )
	{
		CampID_ENUM CampID;
		RoleDataEx* Role = Global::GetRoleDataByGUID( ObjID );
		if( Role == NULL )
			return false;

		if( Role->IsPlayer() == false )
			return false;

		if( Global::Ini()->IsPvE != false )
		{
			CampID = EM_CampID_Good;
		}
		else 
		{
			bool IsLevel_15 = false;
			for( int i = 0 ; i < EM_Max_Vocation ; i++ )
			{
				if( Role->PlayerBaseData->AbilityList[i].Level >= 15 )
					IsLevel_15 = true;
			}

			//檢查陣營是否有改變
			if( IsLevel_15 == false )
			{
				CampID = EM_CampID_NewHand;
			}
			else if( Role->PlayerBaseData->GoodEvil > -30.0f )
			{
				CampID = EM_CampID_Good;
			}
			else
			{
				CampID = EM_CampID_Evil;
			}
		}


		if( Role->TempData.AI.CampID == (CampID_ENUM)CampID )
			return true;

		Role->TempData.AI.CampID = CampID;

		NetSrv_OtherChild::SendRangeCampID( Role , Role->TempData.AI.CampID );

		return true;
	}
	//-----------------------------------------------------------------------------------
	int GetRoleCampID( int iObjID )
	{
		RoleDataEx* pRole = Global::GetRoleDataByGUID( iObjID );
		if( pRole == NULL )
			return -1;

		return pRole->TempData.AI.CampID;
	}
	//-----------------------------------------------------------------------------------
	void ChangeObjID ( int iObjID, int iID )
	{
		RoleDataEx*			pRole	= Global::GetRoleDataByGUID( iObjID );
		GameObjDbStructEx*	pObj	= g_ObjectData->GetObj( iID );

		if( pRole == NULL || pObj == NULL )
			return;

		if( pRole->IsPlayer() )
			return;

		if( iID == pRole->BaseData.ItemInfo.OrgObjID )
			return;

		DelFromPartition( iObjID );

		pRole->BaseData.ItemInfo.OrgObjID	= iID;
		pRole->BaseData.RoleName.Clear();

		AddtoPartition( iObjID, -1 );
	}
	//-----------------------------------------------------------------------------------
	void SetScore ( int iRoomID, int iTeamID, int iVal )
	{
		CNetSrv_BattleGround_Child::SetScore( iRoomID, iTeamID, iVal );
	}
	//-----------------------------------------------------------------------------------
	int GetScore ( int iRoomID, int iTeamID )
	{
		return CNetSrv_BattleGround_Child::GetScore( iRoomID, iTeamID );
	}
	//-----------------------------------------------------------------------------------
	int GetTeamID ( int iObjID )
	{
		RoleDataEx*			pRole	= Global::GetRoleDataByGUID( iObjID );
		if( pRole )
		{
			return ( pRole->TempData.BattleGroundTeamID + 1 );
		}
		return 0;
	}
	//-----------------------------------------------------------------------------------
	void SetZoneReviveScript	( const char* pszScript )
	{
		Global::Ini()->ReviveScript = pszScript;
	}
	//-----------------------------------------------------------------------------------
	void SetZonePVPScript		( const char* pszScript )
	{
		Global::Ini()->LuaFunc_PVP = pszScript;
	}
	//-----------------------------------------------------------------------------------
	void EndBattleGround		( int iRoomID, int iTeamID )
	{
		((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->EndBattleGround( iRoomID, iTeamID );
	}
	//-----------------------------------------------------------------------------------
	void CloseBattleGround		( int iRoomID )
	{
		((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->CloseBattleGround( iRoomID );
	}
	//-----------------------------------------------------------------------------------
	void SetLandMarkInfo ( int iRoomID, int iID, int iVal, float fX, float fY, float fZ, const char* pszName, int iIconID )
	{
		((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->SetLandMarkInfo( iRoomID, iID, iVal, fX, fY, fZ, pszName, iIconID );
	}
	//-----------------------------------------------------------------------------------
	void SetLandMark		( int iRoomID, int iID, int iVal )
	{
		((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->SetLandMark( iRoomID, iID, iVal );
	}
	//-----------------------------------------------------------------------------------
	int	GetLandMark ( int iRoomID, int iID )
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->GetLandMark( iRoomID, iID );
	}
	//-----------------------------------------------------------------------------------
	void SetVar ( int iRoomID, int iID, int iVal )
	{
		((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->SetVar( iRoomID, iID, iVal );
	}
	//-----------------------------------------------------------------------------------
	int	GetVar ( int iRoomID, int iID )
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->GetVar( iRoomID, iID );
	}
	//-----------------------------------------------------------------------------------
	void AddRecipe( int iObjID, int iRecipeID )
	{
		NetSrv_Recipe::AddRecipe( iObjID, iRecipeID );
	}
	//-----------------------------------------------------------------------------------
	void GetRecipeList	( int iClientID, int iObjID )
	{
		NetSrv_Recipe::SC_RecipeList( iClientID, iObjID );
	}
	//-----------------------------------------------------------------------------------
	bool SetCustomTitleString( int iObjID, const char* pszTitle )
	{
		RoleDataEx*			pRole	= Global::GetRoleDataByGUID( iObjID );
		if( pRole == NULL )
			return false;

		std::wstring outStrName;
		CharacterNameRulesENUM  Ret = g_ObjectData->CheckCharacterNameRules( pszTitle , (CountryTypeENUM)Global::Ini()->CountryType , outStrName );
		if( Ret != EM_CharacterNameRules_Rightful )
		{
			return false;
		}

		pRole->PlayerBaseData->TitleStr = WCharToUtf8( outStrName.c_str() ).c_str();

		//pRole->PlayerBaseData->TitleStr = pszTitle;
		pRole->TempData.UpdateInfo.AllZoneInfoChange = 1;
		NetSrv_MoveChild::SendRangeSetTitleID( pRole );
		return true;
	}
	//-----------------------------------------------------------------------------------
	int	DebugCheck(lua_State *L)
	{
		int iNum = lua_gettop (L);

		// check break point 


		return 0;
	}
	//-----------------------------------------------------------------------------------
	int	CallPlot(lua_State *L)
	{
		int iNum = lua_gettop (L);

		int		iObjID					= 0;
		string	FuncName;

		if( iNum >= 2 )
		{
			iObjID			= luaL_checkint( L, 1 );
			FuncName		= luaL_checkstring( L, 2 );
		}
		else
		{
			// 清掉多餘的 Statck
			return 0;
		}



		PlotRegStruct*		T		= LUA_VMClass::ProcReg();
		BaseItemObject*		pObj = BaseItemObject::GetObj( iObjID );
		if( pObj == NULL )
		{
			return 0;
		}

		pObj->PlotVM()->CallPlot( L, iObjID, FuncName.c_str(), iNum );

		return 0;

	}
	//-----------------------------------------------------------------------------------
	bool CheckBit		( int iValue, int iID )
	{
		return bool( iValue & ( 1 << iID ) );
	}
	//-----------------------------------------------------------------------------------
	int	SetBitValue( int iValue, int iBitID, int iBitValue  )
	{
		int iMask = ( 1 << iBitID );
		
		if( iBitValue == 0 )
			return iValue &( 0xffffff^iMask );			
		else
			return iValue | iMask;			
	}
	//----------------------------------------------------------------------------------
	enum CheckRelationTypeENUM
	{
		EM_CheckRelationType_Attackable	,		//可否攻擊
		EM_CheckRelationType_Enemy		,		//是否是敵人
		EM_CheckRelationType_CheckLine	,		//中間是否沒有阻擋
		EM_CheckRelationType_InHateList	,		//是否在仇恨表內
	};
	//----------------------------------------------------------------------------------
	bool	CheckRelation	( int OwnerID , int TargetID , int Type  )
	{
		BaseItemObject* OwnerObj = Global::GetObj( OwnerID );
		if( OwnerObj == NULL )
			return false;
		RoleDataEx* Owner  = OwnerObj->Role();
		RoleDataEx* Target = Global::GetRoleDataByGUID( TargetID );

		if( Owner == NULL || Target == NULL )
			return false;

		switch( Type )
		{
		case EM_CheckRelationType_Attackable:
			return Owner->CheckAttackable( Target );
		case EM_CheckRelationType_Enemy:
			return Owner->CheckEnemy( Target );
		case EM_CheckRelationType_CheckLine:
			return OwnerObj->Path()->CheckLine( Target->X() , Target->Y()  , Target->Z() , Owner->X() , Owner->Y() , Owner->Z() );
		case EM_CheckRelationType_InHateList:
			{
				for( int i = 0 ; i < Owner->TempData.NPCHate.Hate.Size() ; i++ )
				{
					if(		Owner->TempData.NPCHate.Hate[i].DBID == Target->DBID() 
						&&	Owner->TempData.NPCHate.Hate[i].ItemID == Target->GUID() )
						return true;
				}
				return false;
			}
			break;
		}
		return false;
	}

	const char* GetServerDataLanguage()
	{
		return Global::Ini()->Datalanguage.c_str();
	}

	const char* GetServerStringLanguage()
	{
		return Global::Ini()->Stringlanguage.c_str();
	}

	int GetNumTeam()
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->m_BG_iNumTeam;
	}

	int GetNumQueueMember( int iLVID, int iQueueID )
	{
		iQueueID = iQueueID - 1;
		if( iQueueID >= 0 && iQueueID < ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->m_BG_iNumTeam )
		{
			vector< StructBattleGroundQueue >* pQueue = NULL;

			pQueue = &( ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->m_vecWaitQueue[ iLVID ][ iQueueID ] );		
			return (int)pQueue->size();
		}

		return 0;
	}

	void AddTeamItem ( int iRoomID, int iTeamID, int iItemID, int iItemCount )
	{
		((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->AddTeamItem( iRoomID, iTeamID, iItemID, iItemCount );
	}

	void DelRoleItemByDBID ( int iDBID, int iItemID, int iItemCount )
	{		
		RoleDataEx* Item  = Global::GetRoleDataByDBID( iDBID );
		if( Item == NULL )
		{
			//Print( LV3 , "DelBodyItem" , "Item==NULL ItemID=%d OrgID=%d Count=%d",iItemID, OrgID ,Count);
			return;
		}

		if( Item->DelBodyItem( iItemID , iItemCount , EM_ActionType_PlotDestory ) == false )
		{
			return;
		}
		return;
	}

	void AddTeamHonor( int iRoomID, int iTeamID, int iHonor	)
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->AddTeamHonor( iRoomID, iTeamID, iHonor );
	}

	void SetBattleGroundObjCampID		( int iDBID, int iCampID )
	{
		RoleDataEx* pRoleObj  = Global::GetRoleDataByDBID( iDBID );
		if( pRoleObj != NULL )
		{
			pRoleObj->PlayerTempData->BGInfo.iCampID = iCampID;
		}
	}

	void SetBattleGroundObjForceFlagID	( int iDBID, int iFlagID )
	{
		RoleDataEx* pRoleObj  = Global::GetRoleDataByDBID( iDBID );
		if( pRoleObj != NULL )
		{
			pRoleObj->PlayerTempData->iForceFlagID = iFlagID;
		}
	}

	void SetBattleGroundObjGUIDForceFlag	( int iGUID, int iFlagID )
	{
		RoleDataEx* pRoleObj  = Global::GetRoleDataByGUID( iGUID );
		if( pRoleObj != NULL )
		{
			pRoleObj->PlayerTempData->iForceFlagID = iFlagID;
		}
	}


	// 必需要先執行過 BeginCastBar 之後, 才跑這函式, 在使用 BeginCast bar 時, 手上綁取指定物品, 在 EndCastBar 時拔掉
	void AttachCastMotionTool	( int iObjID, int iAttachItemID )
	{
		CNetSrv_Script::AttachCastMotionToolID( iObjID, iAttachItemID );
	}

	void AttachTool	( int iRoleID, int iToolID, const char* cszLinkID )
	{
		CNetSrv_Script::AttachTool( iRoleID, iToolID, cszLinkID );
	}


	// 使用 Client 介面開始一個倒數計時
	void StartClientCountDown	( int iObjID, int iStartNumber, int iEndNumber, int iOffset, int iMode, int iShowStatusBar, const char* Str)
	{
		//CNetSrv_Script::StartClientCountDown( iObjID, iStartNumber, iEndNumber, iOffset );	// 1816
		CNetSrv_Script::StartClientCountDown( iObjID, iStartNumber, iEndNumber, iOffset, iMode, iShowStatusBar, Str );	// 1817
	}

	void StopClientCountDown (int iObjID)
	{
		CNetSrv_Script::StartClientCountDown( iObjID, 0, 0, 0, 0, -1, "" );
	}

	int	GetBattleGroundRoomID	( int iID )
	{
		return CNetSrv_BattleGround_Child::GetBattleGroundRoomID( iID );
	}

	void MoveRoleToBattleGround ( int iWorldID, int iWorldZoneID, int iClientDBID, int iRoomID, float X, float Y, float Z, float Dir )
	{
		int iZoneID = CNetSrv_BattleGround_Child::GetBattleGroundType();
		CNetSrv_BattleGround_Child::SZ_ChangeZoneToBattleGround( iWorldID, iWorldZoneID, iZoneID, iClientDBID, iRoomID, X, Y, Z, Dir );
	}


	void SetArenaScore ( int iRoomID, int iTeamID, int iArenaType, int iIndex, float fValue )
	{
		((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->SetArenaScore( iRoomID, iTeamID, iArenaType, iIndex, fValue );
	}

	float GetArenaScore ( int iRoomID, int iTeamID, int iArenaType, int iIndex	)
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->GetArenaScore( iRoomID, iTeamID, iArenaType, iIndex );
	}


	int	GetNumTeamMember	( int iRoomID, int iTeamID  )
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->GetNumTeamMember( iRoomID, iTeamID );
	}

	int	GetEnterTeamMemberInfo ( int iRoomID, int iTeamID, int iID  )
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->GetEnterTeamMemberInfo( iRoomID, iTeamID, iID );
	}

	int	GetNumEnterTeamMember	( int iRoomID, int iTeamID  )
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->GetNumEnterTeamMember( iRoomID, iTeamID );
	}

	int	GetDisconnectTeamMember	( int iRoomID, int iTeamID  )
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->GetDisconnectTeamMember( iRoomID, iTeamID );
	}

	void Revive ( int iOwnerID, int iZoneID, float x, float y, float z, float ExpDecRate )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iOwnerID );	
		if( pObj == NULL )
			return;
		Global::PlayerResuretion( pObj , iZoneID, x, y, z, ExpDecRate );
	}

	void DebugMsg( int iTargetID , int iRoomID, const char* Str )
	{
		if( iTargetID == 0 )
		{
			if( BaseItemObject::St() == NULL )
				return;

			vector< BaseItemObject* >& ZoneList = *BaseItemObject::GetZoneObjList();
			for( int i = 0 ; i < (int)ZoneList.size() ; i++ )
			{
				BaseItemObject* Obj = ZoneList[i];

				if( Obj == NULL || Obj->Role()->IsPlayer() == false )
					continue;

				if( Obj->Role()->BaseData.RoomID != iRoomID )
					continue;

				if( Obj->Role()->PlayerBaseData->ManageLV != 0 )
				{
					Obj->Role()->Msg( CharToWchar( Str ).c_str() );
				}
			}
		}
		else
		{
			RoleDataEx* Item  = Global::GetRoleDataByGUID( iTargetID );
			if( Item == NULL )
				return;

			if( Item->PlayerBaseData->ManageLV != 0 )
			{
				Item->Msg( CharToWchar( Str ).c_str() );
			}

			return;
		}
	}

	void DebugLog( int iLV, const char* pszString )
	{
		Print( iLV, "Lua", pszString );
	}


	void SetDelayPatrolTime( int iObjID, int iTime )
	{
		RoleDataEx* pObj  = Global::GetRoleDataByGUID( iObjID );
		if( pObj != NULL )
		{
			pObj->TempData.iDelayPatrol = RoleDataEx::G_SysTime + iTime;
		}
	}

	int	GetGUIDByDBID					( int iDBID )
	{
		RoleDataEx* pObj  = Global::GetRoleDataByDBID( iDBID );
		if( pObj != NULL )
		{
			return pObj->GUID();
		}
		return 0;
	}
	//---------------------------------------------------------------
	void SetMemberVar( int iRoomID, int iTeamID, int iGUID, int iVarID, int iVar )
	{
		((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->SetMemberVar( iRoomID, iTeamID, iGUID, iVarID, iVar );
	}
	//---------------------------------------------------------------
	int GetMemberVar( int iRoomID, int iTeamID, int iGUID, int iVarID )
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->GetMemberVar( iRoomID, iTeamID, iGUID, iVarID );
	}
	//---------------------------------------------------------------
	int	GetMaxTeamMember()
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->m_BG_iNumTeamPeople;
	}
	//---------------------------------------------------------------
	void JoinBattleGround ( int iObjID, int iBattleGroundID, int iPartyID, int iSrvID )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iObjID );
		if( pObj )
		{
			int				iTeamMemberLV[ DF_MAX_TEAMDBID ];
			ZeroMemory( iTeamMemberLV, sizeof( iTeamMemberLV ) );

			CNetSrv_BattleGround::m_pThis->OnRC_JoinBattleGround( pObj, iObjID, iBattleGroundID, -1, iSrvID, iTeamMemberLV );
		}

	}
	//---------------------------------------------------------------
	void LeaveBattleGround( int iObjID )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iObjID );
		
		if( pObj != NULL )
		{
			CNetSrv_BattleGround::m_pThis->OnRC_LeaveBattleGround( pObj );
		}
	}
	//---------------------------------------------------------------
	void JoinBattleGroundWithTeam ( int iObjID, int iZoneID )
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iObjID );

		if( pObj )
		{			
			int				iTeamMemberDBID[ DF_MAX_TEAMDBID ];			
			int				iNumParty		= 0;
			int				index			= 0;

			ZeroMemory( iTeamMemberDBID, sizeof( iTeamMemberDBID ) );

			PartyInfoStruct* Party = PartyInfoListClass::This()->GetPartyInfo( pObj->Role()->BaseData.PartyID );

			if( Party != NULL )
			{
				iNumParty = (int)Party->Member.size();

				for( vector< PartyMemberInfoStruct >::iterator it = Party->Member.begin(); it != Party->Member.end(); it++ )
				{
					iTeamMemberDBID[ index ] = (*it).DBID;
					index++;
				}

				
				CNetSrv_BattleGround::m_pThis->OnRC_JoinBattleGround( pObj, iObjID, 0, iNumParty, iZoneID, iTeamMemberDBID );
			}
		}

	}
	//---------------------------------------------------------------
	int GetWorldQueueRule()
	{
		return ((CNetSrv_BattleGround_Child*)CNetSrv_BattleGround_Child::m_pThis)->m_BG_iWorldQueueRule;
	}
	//---------------------------------------------------------------
	void AddHonor( int iObjID, int iValue )
	{
		RoleDataEx*     Player = Global::GetRoleDataByGUID( iObjID );

		Player->AddValue( EM_RoleValue_Honor , iValue );
		Player->Net_DeltaRoleValue( EM_RoleValue_Honor , iValue );		
	}
	//---------------------------------------------------------------
	void AddRankPoint( int iObjID, int iRankPoint )
	{

	}
	//---------------------------------------------------------------
	void AddTeamRankPoint( int iRoomID, int iTeamID, int iRankPoint )
	{

	}
	//---------------------------------------------------------------
	void SetCameraFollowUnit( int iRoleID, int iObjID, int offsetX, int offsetY, int offsetZ)
	{
		BaseItemObject *pObj	= BaseItemObject::GetObj( iRoleID );

		if( pObj == NULL )
			return;

		PG_Script_LtoC_SetCameraFollowUnit Packet;

		Packet.iObjID	= iObjID;
		Packet.offsetX	= offsetX;
		Packet.offsetY	= offsetY;
		Packet.offsetZ	= offsetZ;

		Global::SendToCli_ByGUID( iRoleID, sizeof( Packet ) , &Packet );
	}
	//---------------------------------------------------------------
	int	GetMemory()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		HANDLE hProcess;

		DWORD dwPID = GetCurrentProcessId();

		hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, dwPID );

		GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc));

		//-----------------------------

		return (int)pmc.WorkingSetSize;
	}
	//---------------------------------------------------------------
	void SetWorldVar ( const char* pszVarName, int iVarValue )
	{
		NetSrv_InstanceChild::SetWorldVar( pszVarName, iVarValue );
	}
	//---------------------------------------------------------------
	int	 GetWorldVar ( const char* pszVarName )
	{
		return NetSrv_InstanceChild::GetWorldVar( pszVarName );
	}
	//---------------------------------------------------------------
	int	 GetIniValueSetting ( const char* pszVarName )
	{
		int iValue =  Global::Ini()->Int( pszVarName );
		return iValue;
	}
	//---------------------------------------------------------------


}

