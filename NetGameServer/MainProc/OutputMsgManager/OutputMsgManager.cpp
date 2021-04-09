// OutputMsgManager.cpp: implementation of the OutputMsgManager class.
//
//////////////////////////////////////////////////////////////////////

#include <time.h>
#include "OutputMsgManager.h"
#include <winsock.h>
char				OutputMsgManager::_Date[64];			//����ɶ�
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
//��l�Ƹ��
void OutputMsgManager::Init		(int ZoneID)
{
	char Buf[128];
	_MLv	= 15;
	Update();

	sprintf_s(Buf, sizeof(Buf),"item.%03d",ZoneID);
	_Warning.FileName(Buf);

}
//===========================================================================================
//��s�ثe�ɶ����
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
//Ū���ثe����ɶ�
char*	OutputMsgManager::Date()
{
	return _Date;
}
//===========================================================================================
//��Ҧ��ɮ�����
void	OutputMsgManager::CloseAll()
{
	_Warning.Flush();				
}
//===========================================================================================
//���~�ಾ���Ϳ�X�ɮ�
void	OutputMsgManager::ItemFile(char* File )
{
	_Warning.FileName( File );
}

//===========================================================================================
//ĵ�i��T��XLog
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
	//	log��X�Ҧ�	==> (��� �b�� �W�� �欰id  �ϰ� ��mx ��mz ���󸹽X ĵ�i�T��)	
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