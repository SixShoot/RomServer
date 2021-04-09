#pragma once
#include <vector>
#include "../baseitemobject/BaseItemObject.h"
using namespace std;
//////////////////////////////////////////////////////////////////////////
// �@���Ϊ��X�l���
//////////////////////////////////////////////////////////////////////////
struct FlagPosInfo
{
    BaseItemObject* FlagObj;
    int             ObjID;      //���O
    int             ID;         //�s��
    float           X;
    float           Y;
    float           Z;    
	float			Dir;
	int				FlagDBID;	//�X�l��ƮwID

    FlagPosInfo()
    {
        Init();
    }
    bool    IsEmpty()
    {
        return ( ObjID == -1 );
    }
    void Init()
    {
        memset( this  , 0 , sizeof(*this) );
        ObjID = -1;
		FlagDBID = -1;
    }
};

class   FlagPosClass
{   
	static  vector< vector < FlagPosInfo > >*    _FlagList;    
public:
    static  void			Init();
    static  void			Release();
	
    static  bool			AddFlag( BaseItemObject* FlagObj , int ObjID , int ID , float X , float Y , float Z , float Dir , int FlagDBID = -1 );
    static  bool			DelFlag( BaseItemObject* FlagObj );
	static  bool			DelFlag( int ObjID , int ID , const char* Account );
	static	bool			DelFlag_Group( int ObjiD , const char* Account );

	static  FlagPosInfo*	GetFlag( RoleDataEx* FlagObj );
    static  void			Hide( int ObjID );
    static  void			Show( int ObjID );
    static  FlagPosInfo*	GetFlag( int FlagObjID , int ID );
	static  int				Count( int FlagObjID );
	//���o�Y�@�t�C���X�l
	static  vector < FlagPosInfo >* GetFlagList( int FlagObjID );
	static  bool			 DelFlagList( int FlagObjID , const char* Account );

	static  vector< vector < FlagPosInfo > >*    FlagList() { return _FlagList; };    
 
};

