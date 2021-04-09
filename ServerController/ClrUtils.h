#pragma once
#ifndef __CLR_UTIL_H__
#define __CLR_UTIL_H__

using namespace System;
using namespace System::Diagnostics;

namespace Runewaker
{
//******************************************************************************************
void CLR_KillProcess(int pid);
void CLR_InitPCManager();
void CLR_ProcessPCManager(unsigned long nowtick, unsigned long elapsedtime);
int CLR_GetSysCpuUsage();
int CLR_GetMemUsage(unsigned long pid); //in KB
//******************************************************************************************

//******************************************************************************************
public
ref class PCManager
{
  protected:
	static PCManager ^ m_This = nullptr;

  protected:
	System::Diagnostics::PerformanceCounter ^ m_PC_SysCpuUsage;
	int m_SysCpuUsage;
	unsigned long m_LastUpdateSysCpuUsageTick;

  public:
	static void Init();
	static void KillProcess(int pid);
	static void Process(unsigned long nowtick, unsigned long elapsedtime);
	static PCManager ^ This() { return m_This; }

		public : PCManager();

  protected:
	void _Process(unsigned long nowtick, unsigned long elapsedtime);

  public:
	property int SysCpuUsage
	{
		int get()
		{
			return m_SysCpuUsage;
		}
	}
};
//******************************************************************************************
} // namespace Runewaker
#endif