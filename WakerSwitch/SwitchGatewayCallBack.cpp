#include "SwitchClass.h"
#include "RemotetableInput/RemotableInput.h"

//���s�s��Gateway
int SwitchClass::_ReconnectGateway( SchedularInfo*	Obj , void* InputClass )
{
	SwitchClass* This = (SwitchClass*)InputClass;
	Print( LV5 , "_ReconnectGateway" ,  "connect to Gateway" );
	This->ConnectGateway( (char*)This->_GatewayIP.c_str() , This->_GatewayPort );
	return 0;
}
void	SwitchClass::_OnGatewayConnected    ( int NetID )
{
	_GatewayNetID = NetID;
	_Gateway_WorldIDSet.clear();
	//Gateway ���U
	SysNet_Switch_Gateway_Register Send;
	Send.WorldID = m_WorldID;
	_NetGateway->Send( NetID , sizeof( Send ) , &Send );
}
void	SwitchClass::_OnGatewayDisconnected ( int NetID )
{
	_GatewayNetID = -1;
	Schedular()->Push( _ReconnectGateway , 10000 , this , NULL );
}
bool	SwitchClass::_OnGatewayPacket       ( DWORD NetID , DWORD Size , void* Data )
{

	PGSysNULL* PG = ( PGSysNULL *) Data;

	//------------------------------------------------------------------------------
	//�ˬd�O�_���t�Ϋʥ]
	{
		if(_PGInfo.size() <= (UINT32)PG->Command)
		{
			Print( LV2 , "_OnGatewayPacket" ,"(NetI:%d): packet cmd=%d out of range " , NetID , PG->Command );
			return false;
		}

		//�v���ˬd
		PGBaseInfo *PGinfo = &( _PGInfo[ PG->Command ] );


		//�ӷ��ˬd
		if( PGinfo->Type.Gateway == false )
		{
			Print( LV2 , "_OnGatewayPacket" , "(NetID:%d)_OnGatewayPacket : the source of packet , error (PGinfo->Type.Gateway == false ??) cmd=%d " , NetID , PG->Command );
			return false;
		}

		_PGCallBack[PG->Command]( this , NetID , PG , Size );
	}

	return true;
}
void	SwitchClass::_OnGatewayConnectFailed( int NetID )
{
	_GatewayNetID = -1;
	Schedular()->Push( _ReconnectGateway , 5000 , this , NULL );
}


//�q�����U�O�_���
void	SwitchClass::_PGxGatewayRegisterResult	( SwitchClass* Obj , int NetID , PGSysNULL* PG,int Size )
{
	SysNet_Gateway_Switch_RegisterResult* Resut = (SysNet_Gateway_Switch_RegisterResult*)PG;
	if( Resut->Result == true )
	{
		Print( LV2 , "_PGxGatewayRegisterResult" , " Register OK " );
	}
	else
	{
		Print( LV2 , "_PGxGatewayRegisterResult" , " Register FAILED " );
	}
}
//���@�ɳs�u
void	SwitchClass::_PGxGatewayWorldConnect	( SwitchClass* Obj , int NetID , PGSysNULL* _PG,int Size )
{
	SysNet_Gateway_Switch_WorldConnect* PG = (SysNet_Gateway_Switch_WorldConnect*)_PG;
	Obj->_Gateway_WorldIDSet.insert( PG->WorldID );
}
//�q�����@���_�u
void	SwitchClass::_PGxGatewayWorldDisconnect	( SwitchClass* Obj , int NetID , PGSysNULL* _PG,int Size )
{
	SysNet_Gateway_Switch_WorldDisconnect* PG = (SysNet_Gateway_Switch_WorldDisconnect*)_PG;
	Obj->_Gateway_WorldIDSet.erase( PG->WorldID );
}