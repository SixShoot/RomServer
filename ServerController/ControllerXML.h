#include <iostream>
#include <sstream>
#include "xmlserialization.h"

using namespace xmls;

class ServerXML: public Serializable
{
public:
    ServerXML();    
	xString RegionName;
	xString GroupName;
	xString ServerName;
	xString ExePath;
	xString CmdLine;
	xBool Disabled;
};

class ControllerXML: public Serializable
{
public:
    ControllerXML();    
    Collection<ServerXML> Servers;
};