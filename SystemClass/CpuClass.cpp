#include "CpuClass.h"

CpuClass::CpuClass()
{
}

CpuClass::CpuClass(const CpuClass&)
{
}

CpuClass::~CpuClass()
{
}

void CpuClass::Initialize()
{
	// CPU 사용을 폴링하는 쿼리 개체를 만듭니다.
	PDH_STATUS status = PdhOpenQuery(NULL, 0, &m_queryHandle);

	// Initialize the flag indicating whether this object can read the system cpu usage or not.
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	// 시스템의 모든 CPU를 폴링하도록 쿼리 개체를 설정합니다.
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	m_lastSampleTime = GetTickCount();

	m_cpuUsage = 0;

	return;
}

void CpuClass::Shutdown()
{
	if (m_canReadCpu)
	{
		PdhCloseQuery(m_queryHandle);
	}
}

void CpuClass::Frame()
{
	PDH_FMT_COUNTERVALUE value;

	if (m_canReadCpu)
	{
		if ((m_lastSampleTime + 1000) < GetTickCount())
		{
			m_lastSampleTime = GetTickCount();

			PdhCollectQueryData(m_queryHandle);

			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);

			m_cpuUsage = value.longValue;
		}
	}
}

int CpuClass::GetCpuPercentage()
{
	int Usage;

	if (m_canReadCpu)
	{
		Usage = (int)m_cpuUsage;
	}
	else
	{
		Usage = 0;
	}

	return Usage;
}
