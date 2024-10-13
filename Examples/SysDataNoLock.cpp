#include "SysDataNoLock.h"
#include "WorkerThreadStd.h"

WorkerThread workerThread2("WorkerThread2");

//----------------------------------------------------------------------------
// GetInstance
//----------------------------------------------------------------------------
SysDataNoLock& SysDataNoLock::GetInstance()
{
	static SysDataNoLock instance;
	return instance;
}

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
SysDataNoLock::SysDataNoLock() :
	m_systemMode(SystemMode::STARTING)
{
	SetSystemModeCallback.Register(&SysDataNoLock::SetSystemModePrivate, &workerThread2, this);
	workerThread2.CreateThread();
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
SysDataNoLock::~SysDataNoLock()
{
	SetSystemModeCallback.Unregister(&SysDataNoLock::SetSystemModePrivate, &workerThread2, this);
}

//----------------------------------------------------------------------------
// SetSystemMode
//----------------------------------------------------------------------------
void SysDataNoLock::SetSystemMode(SystemMode::Type systemMode)
{
	// Invoke the private callback. SetSystemModePrivate() will be called on workerThread2.
	SetSystemModeCallback(systemMode);
}

//----------------------------------------------------------------------------
// SetSystemModePrivate
//----------------------------------------------------------------------------
void SysDataNoLock::SetSystemModePrivate(const SystemMode::Type& systemMode, void* userData)
{
	SysDataNoLock* instance = static_cast<SysDataNoLock*>(userData);

	// Create the callback data
	SystemModeChanged callbackData;
	callbackData.PreviousSystemMode = instance->m_systemMode;
	callbackData.CurrentSystemMode = systemMode;

	// Update the system mode
	instance->m_systemMode = systemMode;

	// Callback all registered subscribers
	if (instance->SystemModeChangedCallback)
		instance->SystemModeChangedCallback(callbackData);
}