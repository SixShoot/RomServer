#pragma once
#include "KeyBoardCmd.h" 
#include <conio.h>
#include "RemotetableInput/RemotableInput.h"
#include "ControllerClient/ControllerClient.h"
//----------------------------------------------------------------------------------------
//��l��
KeyBoardCmd::KeyBoardCmd( AccountMainClass* S )
{
	g_remotableInput.Initialize();
	g_remotableInput.OutputStartupMessage();

	_Proc = S;
    KeyBaordCmdManageStruct GMS;

    GMS.Init( "help" , &KeyBoardCmd::Help 
        , "Help �C�X�����άY�^��r�}�Y��GM���O\n help a or help " );
    _CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "Ping" , &KeyBoardCmd::Ping 
		, "�ˬd�O�_�٬���" );
	_CmdMap[ GMS.Name ] = GMS;	

	GMS.Init( "Exit" , &KeyBoardCmd::Exit 
		, "�����{��" );
	_CmdMap[ GMS.Name ] = GMS;	

}
//----------------------------------------------------------------------------------------
int         KeyBoardCmd::_Count( )
{
    return (int)_Val.size();
}
//----------------------------------------------------------------------------------------
float		KeyBoardCmd::_GetFloat( int ID )
{
    if( ID >= (int)_Val.size() )
        return 0;

    return (float) atof( (char*)_Val[ID].c_str() );

}
//----------------------------------------------------------------------------------------
int		KeyBoardCmd::_GetNum( int ID )
{
    if( ID >= (int)_Val.size() )
        return 0;

    return atol( (char*)_Val[ID].c_str() );

}
//----------------------------------------------------------------------------------------
char*	KeyBoardCmd::_GetStr( int ID )
{
    if( ID >= (int)_Val.size() )
        return "";

    return (char*)_Val[ID].c_str();

}
//----------------------------------------------------------------------------------------
//GM�R�O�B�z
bool	KeyBoardCmd::CmdProc( const char* Str )
{

    int		i;
    char	seps[] = " ,\t\n";
    char*   Arg;
    char	Buf[1024];

    _Val.clear();

    if( Str[0] == '\0' || strlen(Str) <= 1)
        return false;

    strcpy( Buf , Str );

    map< string , KeyBaordCmdManageStruct >::iterator Iter;

    char* token = strtok( Buf , seps );

    //�ন�p�g
    _strlwr( token );

    Iter = _CmdMap.find( token );

    if( Iter == _CmdMap.end() )
    {
        //printf( "\n���O�����D!! %s not find" , token );
		PrintToController(false,  "\n���O�����D!! %s not find" , token );
        return false;
    }

    _Val.push_back( token );

    KeyBaordCmdManageStruct KBCmd =Iter->second;

    //���ƭ�
    for( i = 0 ; ; i++ )
    {
        Arg = strtok( NULL , seps );
        if( Arg== NULL ) 
            break;

        _Val.push_back( Arg );
    }

    //�ˬd�O�_���d�M���O
    if( _Val.size() >= 2 && _Val[1][0] == '?' )
    {
        //printf( "\n%s" , (char*)KBCmd.Help.c_str() );
		PrintToController(false,  "\n%s" , (char*)KBCmd.Help.c_str() );
        return true;
    }

    return (this->*KBCmd.Fun)( );

}

//----------------------------------------------------------------------------------------
//Help �C�X�Ҧ���GM���O
bool	KeyBoardCmd::Help( )
{
    char*	Key = _strlwr( _GetStr( 1 ) );
    int		i;
    char	Buf[256];
    char	Temp[256];


    map< string , KeyBaordCmdManageStruct >::iterator Iter;

    //printf( "\n------------------ Help ------------------" );
	PrintToController(false,  "\n------------------ Help ------------------" );
    for( Iter = _CmdMap.begin() ; Iter != _CmdMap.end() ; )
    {
        Buf[0] = 0;
        for( i =0  ; i < 3 && Iter != _CmdMap.end();  Iter++)
        {
            if( Key[0] >= 'a' && Key[0] <= 'z' )
            {
                if( Iter->second.Name.c_str()[1] != Key[0] )
                    continue;
            }
            sprintf(Temp , "%-14s " , Iter->second.Name.c_str() );
            strcat( Buf , Temp );
            i++;
        }
        //Owner->Tell( Buf );
        //printf( "\n%s" , Buf );
		PrintToController(false,  "\n%s" , Buf );

    }

    return true;
}
//----------------------------------------------------------------------------------------
//�B�z�Ӧ�Controller�����O��J
bool KeyBoardCmd::ControllerInput( )
{
	string CmdStr = "";
	if (!ControllerClient::GetCmd(CmdStr))
		return false;

	return CmdProc( CmdStr.c_str() );
}
//----------------------------------------------------------------------------------------
//�B�z��L��J
bool    KeyBoardCmd::KeyBoardInput( )
{
	int     Key;
	bool    Ret;

	// Flush output
	fflush(NULL);

	//�յ۱���controller����J, �@������@�����O
	if (ControllerInput())
		return true;

	if( g_remotableInput.InputAvailable() == false )
		return false;

	Key = g_remotableInput.GetChar();

    switch( Key )
    {
	case 10:
    case 13: //Enter
        Ret = CmdProc( CmdStr.c_str() );
        CmdStr.clear();
        printf("\n");
        return Ret; 
    case 8:  //BACK SPACE
        if( CmdStr.size() > 0)
            CmdStr.erase( CmdStr.size()-1 , 1 );
        break;
    default:
        CmdStr += Key;
        break;
    }
    return false;
}
//----------------------------------------------------------------------------------------
//�����{�� 
bool    KeyBoardCmd::Exit( )
{
	_Proc->Exit();
	return true;
}
//----------------------------------------------------------------------------------------
//�ˬd�O�_�٬���
bool    KeyBoardCmd::Ping        ( )
{
	//printf( "Ping\n" );
	PrintToController(false,  "Ping\n" );
	fflush( NULL );
	return true;
}
//----------------------------------------------------------------------------------------