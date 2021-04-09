#if !defined(AFX_OUTPUTMSGMANAGER_H__2E736407_5783_4435_B9E1_A80F45F54AD3__INCLUDED_)
#define AFX_OUTPUTMSGMANAGER_H__2E736407_5783_4435_B9E1_A80F45F54AD3__INCLUDED_

#include "OutputMsg\BOutputMsgClass.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***********************************************************************************************************************
//	log��X�Ҧ�	==> (��� �b�� �W�� �欰id  �ϰ� ��mx ��mz ���󸹽X ĵ�i�T��)
enum	WarningLogCodeENUM
{
	ENUM_WarningLog_Move	=	0		,

};


//***********************************************************************************************************************

class OutputMsgManager  
{
	static char				_Date[64];			//����ɶ�
	static int				_MLv;				

	static BOutputMsgClass	_Warning;			//ĵ�i��T��X
public:
	OutputMsgManager();
	virtual ~OutputMsgManager();
	
	static void		Init(int);					//��l�Ƹ��
	static void		Update();					//��s�ثe�ɶ����
	static char*	Date();						//Ū���ثe����ɶ�

	static void		ItemFile(char*);			//���~�ಾ���Ϳ�X�ɮ�
	
	static void		CloseAll();					//��Ҧ��ɮ�����


	//-------------------------------------------------------------------------------------------------------------------
	//ĵ�i��T��XLog
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
