#include "ClrUtils.h"
using namespace System::Diagnostics;

namespace Runewaker
{
	//******************************************************************************************
	//---------------------------------------------------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------------------------------------------------
	void CLR_KillProcess(int pid)
	{
		PCManager::This()->KillProcess(pid);
	}

	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	void CLR_InitPCManager()
	{
		PCManager::Init();
	}

	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	void CLR_ProcessPCManager(unsigned long nowtick, unsigned long elapsedtime)
	{
		PCManager::Process(nowtick, elapsedtime);
	}

	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	int CLR_GetSysCpuUsage()
	{
		return PCManager::This()->SysCpuUsage;
	}

	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	int	CLR_GetMemUsage(unsigned long pid)
	{
		int Result = 0;

		System::Diagnostics::Process^ p = System::Diagnostics::Process::GetProcessById(pid);

		if (p && p->PeakWorkingSet64)
		{
			Result = (p->PeakWorkingSet64 / 1024);
		}

		return Result;
	}

	//******************************************************************************************
	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	void PCManager::Init()
	{
		if (m_This == nullptr)
		{
			m_This = gcnew PCManager();
		}
	}

	//---------------------------------------------------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------------------------------------------------
	void PCManager::KillProcess(int pid)
	{
		System::Diagnostics::Process ^ps = System::Diagnostics::Process::GetProcessById(pid);
		if (ps)
		{
			ps->Kill();
		}
	}

	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	void PCManager::Process(unsigned long nowtick, unsigned long elapsedtime)
	{
		if (m_This != nullptr)
		{
			m_This->_Process(nowtick, elapsedtime);
		}		
	}

	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	PCManager::PCManager()
	{
		m_PC_SysCpuUsage = gcnew PerformanceCounter;
		m_PC_SysCpuUsage->CategoryName = "Processor";
		m_PC_SysCpuUsage->CounterName  = "% Processor Time";
		m_PC_SysCpuUsage->InstanceName = "_Total";

		m_SysCpuUsage = 0;
		m_LastUpdateSysCpuUsageTick = 0;
	}

	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	void PCManager::_Process(unsigned long nowtick, unsigned long elapsedtime)
	{
		if (nowtick - m_LastUpdateSysCpuUsageTick >= 1000)
		{
			m_SysCpuUsage = m_PC_SysCpuUsage->NextValue();
			m_LastUpdateSysCpuUsageTick = nowtick;
		}
	}
}