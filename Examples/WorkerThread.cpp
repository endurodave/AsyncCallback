#include "WorkerThread.h"
#include "AsyncCallback.h"
#include "UserMsgs.h"
#include "ThreadMsg.h"

//----------------------------------------------------------------------------
// WorkerThread
//----------------------------------------------------------------------------
WorkerThread::WorkerThread(const CHAR* threadName) : ThreadWin(threadName)
{
}

//----------------------------------------------------------------------------
// Process
//----------------------------------------------------------------------------
unsigned long WorkerThread::Process(void* parameter)
{
	MSG msg;
	BOOL bRet;
	while ((bRet = GetMessage(&msg, NULL, WM_USER_BEGIN, WM_USER_END)) != 0)
	{
switch (msg.message)
{
	case WM_DISPATCH_CALLBACK:
	{
		ASSERT_TRUE(msg.wParam != NULL);

        // Get the ThreadMsg from the wParam value
        ThreadMsg* threadMsg = reinterpret_cast<ThreadMsg*>(msg.wParam);

		// Convert the ThreadMsg void* data back to a CallbackMsg* 
		CallbackMsg* callbackMsg = static_cast<CallbackMsg*>(threadMsg->GetData()); 

		// Invoke the callback callback on the target thread
		callbackMsg->GetAsyncCallback()->TargetInvoke(&callbackMsg);

		// Delete dynamic data passed through message queue
		delete threadMsg;
        break;
	}

	case WM_EXIT_THREAD:
		return 0;

	default:
		ASSERT();
}
	}
	return 0;
}