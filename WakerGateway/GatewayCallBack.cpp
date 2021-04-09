#include "GatewayClass.h"
//-----------------------------------------------------------------------------------
// 初始化 Client 與 Server 的 CallBack函式
//-----------------------------------------------------------------------------------
void	GatewayClass::InitPGCallBack()
{
    int		i;
    PGBaseInfo	PGBase;
    //callback 函式陣列出始化
    for( i = 0 ; i < EM_SysNet_Packet_Count ; i++ )
    {
        _PGCallBack[i] = _PGxUnKnow;
    }

    _PGCallBack[ EM_SysNet_Switch_Gateway_Register   ] = _SysNet_Switch_Gateway_Register;
	_PGCallBack[ EM_SysNet_X_DataTransmitToWorldGSrv ] = _SysNet_X_DataTransmitToWorldGSrv;
	_PGCallBack[ EM_SysNet_X_DataTransmitToWorldGSrv_GSrvID ] = _SysNet_X_DataTransmitToWorldGSrv_GSrvID;
}

void GatewayClass::_SysNet_Switch_Gateway_Register( GatewayClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
	SysNet_Switch_Gateway_Register* RegPG = (SysNet_Switch_Gateway_Register*)PG;
	SysNet_Gateway_Switch_RegisterResult Result;

	Print( LV1 , "_SysNet_Switch_Gateway_Register" , "(NetID:%d)" , NetID );

	Result.WorldID = RegPG->WorldID;

	if( unsigned( Result.WorldID ) > 1000  )
	{
		Print( LV1 , "_SysNet_Switch_Gateway_Register" , "(World:%d) too big" , Result.WorldID );
		Result.Result = false;
		Obj->_NetBSwitch->Send( NetID , sizeof(Result) , &Result );
		return;
	}

	vector< SwitchInfoStruct >::iterator Iter;
	SwitchInfoStruct Temp;
	while( RegPG->WorldID >= Obj->_SwitchMap.size() )
	{
		Obj->_SwitchMap.push_back( Temp );
	}

	SwitchInfoStruct& SwitchInfo = Obj->_SwitchMap[ RegPG->WorldID ];

	if( SwitchInfo.Stat != EM_NetStat_Unknow )
	{
		Print( LV1 , "_SysNet_Switch_Gateway_Register" , "(World:%d) relogin" , Result.WorldID );
		Result.Result = false;
		Obj->_NetBSwitch->Send( NetID , sizeof(Result) , &Result );
		return;
	}

	Result.Result = true;
	Obj->_NetBSwitch->Send( NetID , sizeof(Result) , &Result );
	Obj->_NetIDtoWorldID.Set( NetID , RegPG->WorldID );


	SwitchInfo.ID = NetID;
	SwitchInfo.WorldID = RegPG->WorldID;
	SwitchInfo.Stat = EM_NetStat_Connect;

	//通知所有Switch有新的World
	SysNet_Gateway_Switch_WorldConnect NewWorldPG;
	for( Iter = Obj->_SwitchMap.begin() ; Iter != Obj->_SwitchMap.end() ; Iter++ )
	{
		if( Iter->Stat != EM_NetStat_Connect )
			continue;

		NewWorldPG.WorldID = RegPG->WorldID;
		Obj->SendToSwitch( Iter->ID , sizeof( NewWorldPG ) , &NewWorldPG );
		if( RegPG->WorldID != Iter->WorldID )
		{
			NewWorldPG.WorldID = Iter->WorldID;
			Obj->SendToSwitch( NetID , sizeof( NewWorldPG ) , &NewWorldPG );
		}
	}

}
void GatewayClass::_SysNet_X_DataTransmitToWorldGSrv( GatewayClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
	SysNet_X_DataTransmitToWorldGSrv* TranPG = (SysNet_X_DataTransmitToWorldGSrv*)PG;
	TranPG->FromSrvType = EM_FromServerType_Gateway;
	Obj->SendToSwitch_ByWorldID( TranPG->ToWorldID , Size , TranPG );

	Obj->_PGInfoManage.AddPacket( *(int*)(TranPG->Data) , Size );

}

void GatewayClass::_SysNet_X_DataTransmitToWorldGSrv_GSrvID( GatewayClass* Obj , int NetID , PGSysNULL* PG , int Size )
{
	SysNet_X_DataTransmitToWorldGSrv_GSrvID* TranPG = (SysNet_X_DataTransmitToWorldGSrv_GSrvID*)PG;
	TranPG->FromSrvType = EM_FromServerType_Gateway;
	Obj->SendToSwitch_ByWorldID( TranPG->ToWorldID , Size , TranPG );
	{
		int pgCmdID = *(int*)(&TranPG->Data);		
	}
	Obj->_PGInfoManage.AddPacket( *(int*)(TranPG->Data) , Size );
}
