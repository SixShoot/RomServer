#include "Net_DCBaseChild.h"	
#include "StackWalker/StackWalker.h"
#include "FileVersion/FileVersion.h"
bool	Net_DCBaseChild::_IsStart = true;
//-------------------------------------------------------------------
bool    Net_DCBaseChild::Init()
{
	Net_DCBase::_Init();

	if( This != NULL)
		return false;

	This = NEW( Net_DCBaseChild );

	return true;
}

//--------------------------------------------------------------------------------------
bool    Net_DCBaseChild::Release()
{
	if( This == NULL )
		return false;

	delete This;
	This = NULL;
	
	Net_DCBase::_Release();
	return true;

}
//--------------------------------------------------------------------------------------
void Net_DCBaseChild::OnDataCenterConnect( )
{
	if( _IsStart == false )
		return;
	_IsStart = false;

	char	Buf[512];
	char	ExeFileName[MAX_PATH];
	//¼g¤JVersion¸ê®Æ
	GetModuleFileName( NULL, ExeFileName, MAX_PATH );

	CFileVersion Version;
	if (Version.Open(ExeFileName))
	{
		CString ver = Version.GetFixedFileVersion();
		const char* verStr = ver.GetString();
	
		sprintf( Buf , "INSERT INTO ServerStartLog( Version,Server) VALUES ( '%s' , 'PARTITION' )" , verStr );
		Net_DCBase::LogSqlCommand( Buf );
	}

};