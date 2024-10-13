#include "WorkerThreadStd.h"
#include "AsyncCallback.h"
#include "SysData.h"
#include "SysDataNoLock.h"
#include <iostream>
#include <chrono>

using namespace std;

WorkerThread workerThread1("WorkerThread1");

/// @brief Test client to get callbacks from SysData::SystemModeChangedCallback and 
/// SysDataNoLock::SystemModeChangedCallback
class SysDataClient
{
public:
	// Constructor
	SysDataClient() :
		m_numberOfCallbacks(0)
	{
		// Register for async callbacks
		SysData::GetInstance().SystemModeChangedCallback.Register(&SysDataClient::CallbackFunction, &workerThread1, this);
		SysDataNoLock::GetInstance().SystemModeChangedCallback.Register(&SysDataClient::CallbackFunction, &workerThread1, this);
	}

	~SysDataClient()
	{
		// Unregister the all registered callbacks at once
		SysData::GetInstance().SystemModeChangedCallback.Clear(); 

		// Alternatively unregister a single callback
		SysDataNoLock::GetInstance().SystemModeChangedCallback.Unregister(&SysDataClient::CallbackFunction, &workerThread1, this);
	}

private:
	static void CallbackFunction(const SystemModeChanged& data, void* userData)
	{
		// The user data pointer oringates from the 3rd argument in the Register() function
		// Typecast the void* to SysDataClient* to access object instance data/functions.
		SysDataClient* instance = static_cast<SysDataClient*>(userData);
		instance->m_numberOfCallbacks++;

		cout << "CallbackFunction " << data.CurrentSystemMode << endl;
	}

	int m_numberOfCallbacks;
};

/// Simple free callback function
void SimpleCallback(const int& value, void* userData)
{
	cout << "SimpleCallback " << value << endl;
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int main(void)
{	
	// Create the worker threads
	workerThread1.CreateThread();
	SysDataNoLock::GetInstance();

	// Create async callback and register a function pointer
	AsyncCallback<int> callback;
	callback.Register(&SimpleCallback, &workerThread1);

	// Asynchronously invoke the callback
	callback(123);
	callback.Invoke(123);

	// Unregister the callback
	callback.Unregister(&SimpleCallback, &workerThread1);

	// Create a SysDataClient instance on the stack
	SysDataClient sysDataClient;

	// Set new SystemMode values. Each call will invoke callbacks to all 
	// registered client subscribers.
	SysData::GetInstance().SetSystemMode(SystemMode::STARTING);
	SysData::GetInstance().SetSystemMode(SystemMode::NORMAL);

	// Set new SystemMode values for SysDataNoLock.
	SysDataNoLock::GetInstance().SetSystemMode(SystemMode::SERVICE);
	SysDataNoLock::GetInstance().SetSystemMode(SystemMode::SYS_INOP);

	// Give time for callbacks to occur on worker threads
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	workerThread1.ExitThread();
	return 0;
}

