#pragma once
#include "IAccountPlugin.h"
#include "basefunction/BaseFunction.h"

int		IAccountPlugin::_TimeZone = 0;
IAccountPlugin::IAccountPlugin( IAccountPluginNotify *notify , IniFileClass	*ini )
{
	_pEventNotify = notify; 

	string  DBServer	= ini->Str( "Acc_DBServer" );
	string  DataBase    = ini->Str( "Acc_DataBase" );
	string  Account     = ini->Str( "Acc_Account" );
	string  Password    = ini->Str( "Acc_Password" );
	int		ThreadCount = ini->Int( "Acc_DBThreadCount" );
	_TimeZone			= ini->Int( "TimeZone" );
	if( ThreadCount == 0 )
		ThreadCount = 50;
		
	char* Pwd = DecodePassword( Password.c_str() , Account.c_str() );

	_DBTransfer = NEW DBTransferClass( DBServer.c_str() , DataBase.c_str() , Account.c_str() , Pwd , ThreadCount );
	_AcountInfoDB = NEW DBStructTransferClass<PlayerAccountDBStruct> ( _DBTransfer , PlayerAccountDBStruct::Reader() , "PlayerAccount" );	
	_NormalDB		= NEW DBStructTransferClass<int> ( _DBTransfer , NULL , "" );

	_RDPlayerAcccountBaseSql = NEW CreateDBCmdClass<PlayerAccountDBStruct>( PlayerAccountDBStruct::Reader() , "PlayerAccount" );

};

void IAccountPlugin::Process()
{
	_AcountInfoDB->Process();
	_NormalDB->Process();	
	SqlBaseClass::OnTimeProc( );
};

IAccountPlugin::~IAccountPlugin( void )
{
	while( 1 )
	{
		Sleep( 10);
		if( _DBTransfer->TotalRequestCount() == 0 )
		{
			break;
		}
	}

	delete _DBTransfer;
	delete _AcountInfoDB;
	delete _NormalDB;	
};


#define _CRuHashGenerator_MIX(a, b, c)	\
{									\
	a -= b; a -= c; a ^= (c >> 13);	\
	b -= c; b -= a; b ^= (a << 8);	\
	c -= a; c -= b; c ^= (b >> 13);	\
	a -= b; a -= c; a ^= (c >> 12);	\
	b -= c; b -= a; b ^= (a << 16);	\
	c -= a; c -= b; c ^= (b >> 5);	\
	a -= b; a -= c; a ^= (c >> 3);	\
	b -= c; b -= a; b ^= (a << 10);	\
	c -= a; c -= b; c ^= (b >> 15);	\
}

DWORD GetHashCode(const BYTE *k, DWORD length)
{
	DWORD a, b, c, len;

	// Set up the internal state
	len = length;
	a = b = 0x9e3779b9;		// The golden ratio; an arbitrary value
	c = 0;			// The previous hash value

	// Handle most of the key
	while (len >= 12)
	{
		a += (k[0] +((DWORD) k[1] << 8) + ((DWORD) k[2] << 16) + ((DWORD) k[3] << 24));
		b += (k[4] +((DWORD) k[5] << 8) + ((DWORD) k[6] << 16) + ((DWORD) k[7] << 24));
		c += (k[8] +((DWORD) k[9] << 8) + ((DWORD) k[10] << 16)+ ((DWORD) k[11] << 24));
		_CRuHashGenerator_MIX(a, b, c);
		k += 12; len -= 12;
	}

	// Handle the last 11 bytes
	c += length;
	switch(len)				// All the case statements fall through
	{
	case 11:
		c += ((DWORD) k[10] << 24);
	case 10:
		c += ((DWORD) k[9] << 16);
	case 9:
		c += ((DWORD) k[8] << 8);
		// The first byte of c is reserved for the length
	case 8:
		b += ((DWORD) k[7] << 24);
	case 7:
		b += ((DWORD) k[6] << 16);
	case 6:
		b += ((DWORD) k[5] << 8);
	case 5:
		b += k[4];
	case 4:
		a += ((DWORD) k[3] << 24);
	case 3:
		a += ((DWORD) k[2] << 16);
	case 2:
		a += ((DWORD) k[1] << 8);
	case 1:
		a += k[0];
		// case 0: nothing left to add
	}

	_CRuHashGenerator_MIX(a, b, c);

	// Return the result
	return c;
}

//用來都執行序用的
unsigned	IAccountPlugin::_AccountValue( string Account )
{
	return GetHashCode( (const BYTE*)Account.c_str() , (DWORD)Account.size() );
	/*
	_strlwr( (char*)Account.c_str() );

	int		Value = 0;
	for( int i = 0 ; i < 10 ; i++ )
	{
		if( (Account.c_str())[i] == 0 )
			return Value;
		Value ^= (Account.c_str())[i];
	}
	return 0;
	*/
}