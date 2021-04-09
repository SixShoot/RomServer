#pragma once
#include "TelnetClass.h"

//封包基本型
struct WebServicePacketBase
{
	unsigned	Command;
	char		Data[0];
};


bool		LBTelnet::OnRecv			( DWORD NetID, DWORD dwPackSize, PVOID pPacketData )
{
	if( _Parent->_OnPacket(NetID,dwPackSize,pPacketData) == false )
	{
		//資料異常
		_Parent->_NetTelnet->Close( NetID );
	}
	return true;   
}
void		LBTelnet::OnConnectFailed	( DWORD NetID, DWORD dwFailedCode )
{
	//_Parent->_OnConnectFailed(NetID);
}
void		LBTelnet::OnConnect		( DWORD NetID )
{
	_Parent->_OnConnected(NetID);
}
void		LBTelnet::OnDisconnect	( DWORD NetID )
{
	_Parent->_OnDisconnected(NetID);
}
CEventObj*	LBTelnet::OnAccept		( DWORD NetID )
{
	return this;
}


void	TelnetClass::_OnConnected       ( int NetID )
{
	
	while( (unsigned)_PacketBuffList.size() <= NetID )
		_PacketBuffList.push_back( NULL );

	_PacketBuffList[ NetID ] = NEW PacketBuffClass;

	for( unsigned i = 0 ; i < _OnConnectFunc.size() ; i++ )
	{
		_OnConnectFunc[i]( NetID );
	}
}
void	TelnetClass::_OnDisconnected    ( int NetID )
{
	delete _PacketBuffList[ NetID ];
	_PacketBuffList[ NetID ] = NULL;

	for( unsigned i = 0 ; i < _OnDisconnectFunc.size() ; i++ )
	{
		_OnDisconnectFunc[i]( NetID );
	}
}
bool	TelnetClass::_OnPacket          ( DWORD dwNetID, DWORD Size , PVOID Data )
{

	PacketBuffClass* _PacketBuff = _PacketBuffList[ dwNetID ];
	if( _PacketBuff == NULL )
		return false;
	
	char* DataPos = (char*)Data;
	for( unsigned i = 0 ; i < Size ; i++ )
	{
		if( _PacketBuff->SetChar( DataPos[i] ) == false )
		{
			return false;
		}
		if( _PacketBuff->IsReadOK() )
		{
			WebServicePacketBase* PGBase = (WebServicePacketBase*)_PacketBuff->GetData();
			if(		(unsigned)PGBase->Command > _OnPacketFunc.size() 
				||	_OnPacketFunc[ PGBase->Command ] == NULL )
			{
				Print( LV5 , "TelnetClass::_OnPacket" , "_OnPacket[ %d ] == NULL" , PGBase->Command );
			}
			else
			{
				_OnPacketFunc[ PGBase->Command ]( dwNetID , _PacketBuff->PacketSize() , _PacketBuff->GetData() );
			}

			_PacketBuff->Clear();
		}
	}
	return true;
}


TelnetClass::TelnetClass( )
{
	_NetTelnet     = CreateNetBridge( );
	_NetListener   = new LBTelnet( this );	//資料列舉Class

}
TelnetClass::~TelnetClass()
{
	DeleteNetBridge( _NetTelnet );
	delete _NetListener;
		//vector< PacketBuffClass*>	_PacketBuffList;
	for( unsigned i = 0 ; i < _PacketBuffList.size() ; i++ )
	{
		if( _PacketBuffList[i] != NULL )
			delete _PacketBuffList[i];
	}
}

void TelnetClass::Host( const char* IP , int Port )
{
	int IPnum = ConvertIP( IP );
	_NetTelnet->Host( IPnum , Port , _NetListener );	
}

//定時處理
bool	TelnetClass::Process( )
{
	return true;
}

//登出
bool    TelnetClass::Close( int NetID )
{
	_NetTelnet->Close( NetID );
	return true;
}

//送資料
void	TelnetClass::Send( int NetID ,int Size , void* Data )
{
	_NetTelnet->Send( NetID , sizeof(int) , (void*)&Size );
	_NetTelnet->Send( NetID , Size , Data );
}

bool    TelnetClass::RegOnConnectFunction		( OnConnectFunctionBase Func )
{
	_OnConnectFunc.push_back( Func );
	return false;
}
bool    TelnetClass::RegOnDisConnectFunction	( OnDisconnectFunctionBase Func )
{
	_OnDisconnectFunc.push_back( Func );
	return false;
}
bool    TelnetClass::RegOnPacketFunction		( int PGid , OnPacketFunctionBase Func )
{
	if( PGid < 0  )
		return false;

	while(  (int)_OnPacketFunc.size() <= PGid )
		_OnPacketFunc.push_back( NULL );

	if( _OnPacketFunc[ PGid ] != NULL )
	{
		Print(LV5 , "RegOnPacketFunction" , "_OnPacket[ %s ] != NULL " , PGid );
		return false;
	}

	_OnPacketFunc[ PGid ] = Func;
	return true;
}