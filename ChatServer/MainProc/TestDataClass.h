#ifndef		__TESTDATA_H_2003_8_12__
#define		__TESTDATA_H_2003_8_12__
#include "RoleData\RoleDataEx.h"
#include "RoleData\ObjectStruct.h"
#include "ObjectDataFile\ObjectDataBaseFile.h"
class RuStreamFile;
using namespace NSObjectDataFile;

namespace	NS_TestData
{
	void Init( ObjectDataFileClass<GameObjDbStructEx,RuStreamFile>* ObjectDB );

};

#endif