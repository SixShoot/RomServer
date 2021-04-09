#include "ControllerXML.h"

ServerXML::ServerXML(): Disabled(false)
{
    // Set the XML class name.
    // This name can differ from the C++ class name
    setClassName("ServerXML");
    // Set class version
    setVersion("1.0");
    // Register members. Like the class name, member names can differ from their xml depandants
    Register("RegionName", &RegionName);
    Register("GroupName", &GroupName);
    Register("ServerName", &ServerName);
    Register("ExePath", &ExePath);
    Register("CmdLine", &CmdLine);
    Register("Disabled", &Disabled);
}; 

ControllerXML::ControllerXML()
{
    // Set the XML class name.
    // This name can differ from the C++ class name
    setClassName("ControllerXML");
    // Set class version
    setVersion("1.0");
    // Register members. Like the class name, member names can differ from their xml depandants
    Register("Servers", &Servers);
}; 