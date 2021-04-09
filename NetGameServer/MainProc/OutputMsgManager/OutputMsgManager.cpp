// OutputMsgManager.cpp: implementation of the OutputMsgManager class.
//
//////////////////////////////////////////////////////////////////////

#include <time.h>
#include "OutputMsgManager.h"
#include <winsock.h>
char				OutputMsgManager::_Date[64];			//日期時間
int					OutputMsgManager::_MLv;				

BOutputMsgClass		OutputMsgManager::_Warning;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//===========================================================================================
OutputMsgManager::OutputMsgManager()
{
}
//===========================================================================================
OutputMsgManager::~OutputMsgManager()
{
}
//===========================================================================================
//初始化資料
void OutputMsgManager::Init		(int ZoneID)
{
	char Buf[128];
	_MLv	= 15;
	Update();

	sprintf_s(Buf, sizeof(Buf),"item.%03d",ZoneID);
	_Warning.FileName(Buf);

}
//===========================================================================================
//更新目前時間日期
void	OutputMsgManager::Update()
{
	static int LastUpDateTime = 0;

	if(LastUpDateTime > clock() )
		return;

	LastUpDateTime += 1000*60;

	time_t Now;
	tm *gmt;

	time( &Now );
	gmt = localtime( &Now );

	sprintf_s(_Date, sizeof(_Date)
							,"%04d-%02d-%02d %02d:%02d:00"
							,gmt->tm_year+1900 
							,gmt->tm_mon+1
							,gmt->tm_mday
							,gmt->tm_hour
							,gmt->tm_min );
	_Date[24] = 0;
}
//===========================================================================================
//讀取目前日期時間
char*	OutputMsgManager::Date()
{
	return _Date;
}
//===========================================================================================
//把所有檔案關閉
void	OutputMsgManager::CloseAll()
{
	_Warning.Flush();				
}
//===========================================================================================
//物品轉移產生輸出檔案
void	OutputMsgManager::ItemFile(char* File )
{
	_Warning.FileName( File );
}

//===========================================================================================
//警告資訊輸出Log
void	OutputMsgManager::Warning(	int						MLv
								,	char*					Account_name
								,	char*					Char_name
								,	WarningLogCodeENUM		Action
								,	int						ZoneID
								,	int						PosX
								,	int						PosZ
								,	int						ItemID
								,	char*					Msg					)
{
	//	log輸出模式	==> (日期 帳號 名稱 行為id  區域 位置x 位置z 物件號碼 警告訊息)	
	_Warning.Write( 0 ,"%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\n"
		, _Date , Account_name , Char_name , int(Action) , ZoneID , PosX , PosZ , ItemID , Msg );
}
//===========================================================================================
/*
void	OutputMsgManager::Warning(	RoleData*				Owner
								,	WarningLogCodeENUM		Action
								,	char*					Msg					)
{
	Warning(	Owner->PlayerBaseData->ManageLV 
			,	Owner->AccountName 
			,	Owner->BaseData.RoleName
			,	Action
			,	Owner->G_ZoneID
			,	Owner->PlayerBaseData->X
			,	Owner->PlayerBaseData->Z
			,	Owner->TempData.ItemID
			,	Msg								);
}	*/
//===========================================================================================