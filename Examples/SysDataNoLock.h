#ifndef _SYS_DATA_NO_LOCK_H
#define _SYS_DATA_NO_LOCK_H

#include "AsyncCallback.h"
#include "SysData.h"

/// @brief SysData stores common data accessible by any system thread. This class
/// is thread-safe.
class SysDataNoLock
{
public:
	/// Clients register with AsyncCallback to get callbacks when system mode changes
	AsyncCallback<SystemModeChanged> SystemModeChangedCallback;

	/// Get singleton instance of this class
	static SysDataNoLock& GetInstance();

	/// Sets the system mode and notify registered clients via SystemModeChangedCallback.
	/// @param[in] systemMode - the new system mode. 
	void SetSystemMode(SystemMode::Type systemMode);	

private:
	SysDataNoLock();
	~SysDataNoLock();

	/// Private callback to get the SetSystemMode call onto a common thread
	AsyncCallback<SystemMode::Type> SetSystemModeCallback;

	/// Sets the system mode and notify registered clients via SystemModeChangedCallback.
	/// @param[in] systemMode - the new system mode. 
	/// @param[in] userData - a 'this' pointer to SysDataNoLock.  
	static void SetSystemModePrivate(const SystemMode::Type& systemMode, void* userData);	

	/// The current system mode data
	SystemMode::Type m_systemMode;
};

#endif