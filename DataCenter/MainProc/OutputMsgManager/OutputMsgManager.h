#if !defined(AFX_OUTPUTMSGMANAGER_H__2E736407_5783_4435_B9E1_A80F45F54AD3__INCLUDED_)
#define AFX_OUTPUTMSGMANAGER_H__2E736407_5783_4435_B9E1_A80F45F54AD3__INCLUDED_

#include "OutputMsg\BOutputMsgClass.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***********************************************************************************************************************
//	log輸出模式	==> (日期 帳號 名稱 行為id  區域 位置x 位置z 物件號碼 警告訊息)
enum	WarningLogCodeENUM
{
	ENUM_WarningLog_Move	=	0		,

};


//***********************************************************************************************************************

class OutputMsgManager  
{
	static char				_Date[64];			//日期時間
	static int				_MLv;				

	static BOutputMsgClass	_Warning;			//警告資訊輸出
public:
	OutputMsgManager();
	virtual ~OutputMsgManager();
	
	static void		Init(int);					//初始化資料
	static void		Update();					//更新目前時間日期
	static char*	Date();						//讀取目前日期時間

	static void		ItemFile(char*);			//物品轉移產生輸出檔案
	
	static void		CloseAll();					//把所有檔案關閉


	//-------------------------------------------------------------------------------------------------------------------
	//警告資訊輸出Log
	static void		Warning(	int						MLv
							,	char*					Account_name
							,	char*					Char_name
							,	WarningLogCodeENUM		Action
							,	int						ZoneID
							,	int						PosX
							,	int						PosZ
							,	int						ItemID
							,	char*					Msg					);
/*
	static void		Warning(	RoleData*				Owner
							,	WarningLogCodeENUM		Action
							,	char*					Msg					);
                            */
	//-------------------------------------------------------------------------------------------------------------------

};


#endif // !defined(AFX_OUTPUTMSGMANAGER_H__2E736407_5783_4435_B9E1_A80F45F54AD3__INCLUDED_)
