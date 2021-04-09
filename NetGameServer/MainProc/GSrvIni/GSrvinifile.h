#ifndef		__GSRVINIFILE_H__
#define		__GSRVINIFILE_H__
#pragma warning (disable:4786)
#include	"IniFile\IniFile.h"
#include	<string>
#include	"../GlobalDef.h"
#include	<map>

using namespace std;	

struct GameObjDbStructEx;
class	GSrvIniFileClass:public IniFileClass, public GSrvInfoClass
{
	map<string,string>		KeyValueMap;

	void	SetKeyValueMap( GameObjDbStructEx* OrgDB );
public:
	bool Init(const char* IniFile);

	int		Int(const char*);
	bool	Int( const char* KeyStr , int&	OutInt );
	string	Str(const char*);
	bool	Str( const char* KeyStr , string&	OutString );
    //----------------------------------------------------------------------
};

#endif
