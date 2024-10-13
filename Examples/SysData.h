#ifndef _SYS_DATA_H
#define _SYS_DATA_H

#include "AsyncCallback.h"
#include <mutex>

struct SystemMode
{
	enum Type
	{
		STARTING,
		NORMAL,
		SERVICE,
		SYS_INOP
	};
};

/// @brief Structure to hold system mode callback data. 
class SystemModeChanged
{
public:
	SystemModeChanged() : 
		PreviousSystemMode(SystemMode::STARTING), 
		CurrentSystemMode(SystemMode::STARTING)
	{
	}

	SystemMode::Type PreviousSystemMode;
	SystemMode::Type CurrentSystemMode;
};

/// @brief SysData stores common data accessible by any system thread. This class
/// is thread-safe.
class SysData
{
public:
	/// Clients register with AsyncCallback to get callbacks when system mode changes
	AsyncCallback<SystemModeChanged> SystemModeChangedCallback;

	/// Get singleton instance of this class
	static SysData& GetInstance();

	/// Sets the system mode and notify registered clients via SystemModeChangedCallback.
	/// @param[in] systemMode - the new system mode. 
	void SetSystemMode(SystemMode::Type systemMode);	

private:
	SysData();
	~SysData();

	/// The current system mode data
	SystemMode::Type m_systemMode;

	/// Lock to make the class thread-safe
	std::mutex m_lock;
};

#endif