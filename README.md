![License MIT](https://img.shields.io/github/license/BehaviorTree/BehaviorTree.CPP?color=blue)
[![conan Ubuntu](https://github.com/endurodave/AsyncCallback/actions/workflows/cmake_ubuntu.yml/badge.svg)](https://github.com/endurodave/AsyncCallback/actions/workflows/cmake_ubuntu.yml)
[![conan Ubuntu](https://github.com/endurodave/AsyncCallback/actions/workflows/cmake_clang.yml/badge.svg)](https://github.com/endurodave/AsyncCallback/actions/workflows/cmake_clang.yml)
[![conan Windows](https://github.com/endurodave/AsyncCallback/actions/workflows/cmake_windows.yml/badge.svg)](https://github.com/endurodave/AsyncCallback/actions/workflows/cmake_windows.yml)

# Asynchronous Multicast Callbacks with Inter-Thread Messaging

A C++ asynchronous callback framework simplifies passing data between threads.

# Table of Contents

- [Asynchronous Multicast Callbacks with Inter-Thread Messaging](#asynchronous-multicast-callbacks-with-inter-thread-messaging)
- [Table of Contents](#table-of-contents)
- [Preface](#preface)
- [Introduction](#introduction)
- [Callbacks Background](#callbacks-background)
- [Using the Code](#using-the-code)
  - [SysData Example](#sysdata-example)
  - [SysDataClient Example](#sysdataclient-example)
  - [SysDataNoLock Example](#sysdatanolock-example)
  - [Timer Example](#timer-example)
- [Callback Signature Limitations](#callback-signature-limitations)
- [Implementation](#implementation)
- [Heap](#heap)
- [Porting](#porting)
- [Code Size](#code-size)
- [References](#references)
- [Conclusion](#conclusion)


# Preface

Originally published on CodeProject at: <a href="http://www.codeproject.com/Articles/1092727/Asynchronous-Multicast-Callbacks-with-Inter-Thread"><strong>Asynchronous Multicast Callbacks with Inter-Thread Messaging</strong></a>

<p><a href="https://www.cmake.org/">CMake</a>&nbsp;is used to create the build files. CMake is free and open-source software. Windows, Linux and other toolchains are supported. See the <strong>CMakeLists.txt </strong>file for more information.</p>

<p>Asynchronous function invocation allows for easy movement of data between threads. The table below summarizes the various asynchronous function invocation implementations available in C and C++.</p>

| Repository                                                                                            | Language | Key Delegate Features                                                                                                                                                                                                               | Notes                                                                                                                                                                                                      |
|-------------------------------------------------------------------------------------------------------|----------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <a href="https://github.com/endurodave/AsyncMulticastDelegateModern">AsyncMulticastDelegateModern</a> | C++17    | * Function-like template syntax<br> * Any delegate target function type (member, static, free, lambda)<br>  * N target function arguments<br> * N delegate subscribers<br> * Variadic templates<br> * Template metaprogramming      | * Most generic implementation<br> * Lowest lines of source code<br> * Slowest of all implementations<br> * Optional fixed block allocator support<br> * No remote delegate support<br> * Complex metaprogramming |
| <a href="https://github.com/endurodave/AsyncMulticastDelegateCpp17">AsyncMulticastDelegateCpp17</a>   | C++17    | * Function-like template syntax<br> * Any delegate target function type (member, static, free, lambda)<br> * 5 target function arguments<br> * N delegate subscribers<br> * Optional fixed block allocator<br> * Variadic templates | * Selective compile using constexpr<br> * Avoids complex metaprogramming<br> * Faster than AsyncMulticastDelegateModern<br> * No remote delegate support                                                   |
| <a href="https://github.com/endurodave/AsyncMulticastDelegateCpp11">AsyncMulticastDelegateCpp11</a>   | C++11    | * Function-like template syntax<br> * Any delegate target function type (member, static, free, lambda)<br> * 5 target function arguments<br> * N delegate subscribers<br> * Optional fixed block allocator                          | * High lines of source code<br> * Highly repetitive source code                                                                                                                                            |
| <a href="https://github.com/endurodave/AsyncMulticastDelegate">AsyncMulticastDelegate</a>             | C++03    | * Traditional template syntax<br> * Any delegate target function type (member, static, free)<br> * 5 target function arguments<br> * N delegate subscribers<br> * Optional fixed block allocator                                    | * High lines of source code<br> * Highly repetitive source code                                                                                                                                            |
| <a href="https://github.com/endurodave/AsyncCallback">AsyncCallback</a>                               | C++      | * Traditional template syntax<br> * Delegate target function type (static, free)<br> * 1 target function argument<br> * N delegate subscribers                                                                                      | * Low lines of source code<br> * Most compact C++ implementation<br> * Any C++ compiler                                                                                                                    |
| <a href="https://github.com/endurodave/C_AsyncCallback">C_AsyncCallback</a>                           | C        | * Macros provide type-safety<br> * Delegate target function type (static, free)<br> * 1 target function argument<br> * Fixed delegate subscribers (set at compile time)<br> * Optional fixed block allocator                        | * Low lines of source code<br> * Very compact implementation<br> * Any C compiler                                                                                                                          |

# Introduction

<p>Callbacks are a powerful concept used to reduce the coupling between two pieces of code. On a multithreaded system, callbacks have limitations. What I&#39;ve always wanted was a callback mechanism that crosses threads and handles all the low-level machinery to get my event data from one thread to another safety. I need a small, portable and easy to use framework. No more monster <code>switch</code> statements inside a thread loop that typecast OS message queue <code>void*</code> values based upon an enumeration. Create a callback. Register a callback. And the framework automagically invokes the callback with data arguments on a user specified target thread is the goal.&nbsp;</p>

<p>The callback solution presented here provides the following features:</p>

<ol>
	<li><strong>Asynchronous callbacks</strong> &ndash; support asynchronous callbacks to and from any thread</li>
	<li><strong>Thread targeting</strong> &ndash; specify the destination thread for the asynchronous callback</li>
	<li><strong>Callbacks</strong> &ndash; invoke any C or C++ free function with a matching signature</li>
	<li><strong>Type safe</strong> &ndash; user defined, type safe callback function data arguments</li>
	<li><strong>Member functions</strong> &ndash; call instance member functions</li>
	<li><strong>Multicast callbacks</strong> &ndash; store multiple callbacks within a list for sequential invocation</li>
	<li><strong>Thread-safe</strong> &ndash; suitable for use on a multi-threaded system</li>
	<li><strong>Compact</strong> &ndash; small, easy to maintain code base consuming minimal code space</li>
	<li><strong>Portable</strong> &ndash; portable to an embedded or PC-based platform</li>
	<li><strong>Any compiler</strong> &ndash; no advanced C++ language features</li>
	<li><strong>Any OS</strong> - easy porting to any operating system</li>
	<li><strong>Elegant syntax</strong> &ndash; intuitive and easy to use</li>
</ol>

<p>The callback paradigm significantly eases multithreaded application development by placing the callback and callback data onto the thread of control that you specify. Exposing an asynchronous callback interface&nbsp;for a single class, module or an entire subsystem is extremely easy. The framework is no more difficult to use than a standard C callback but with more features.</p>

<p>This article proposes an inter-thread communication mechanism utilizing asynchronous multicast callbacks. The attached source code implements all features above, as I&#39;ll demonstrate.</p>

# Callbacks Background

<p>The idea of a function callback is very useful. In callback terms, a <em>publisher</em> defines the callback signature and allows anonymous registration of a callback function pointer. A <em>subscriber</em> creates a function implementation conforming to the publisher&#39;s callback signature and registers a callback function pointer with the publisher at runtime. The publisher code knows nothing about the subscriber code &ndash; the registration and the callback invocation is anonymous.</p>

<p>Now, on a multithreaded system, you need understand synchronous vs. asynchronous callback invocations. If the callback is synchronous, the callback is executed on the caller&#39;s thread of control. If you put a break point inside the callback, the stack frame will show the publisher function call and the publisher callback all synchronously invoked. There are no multithreaded issues with this scenario as everything is running on a single thread.</p>

<p>If the publisher code has its own thread, it may invoke the callback function on its thread of control and not the subscriber&#39;s thread. A publisher invoked callback can occur at any time completely independent of the subscriber&rsquo;s thread of control. This cross-threading can cause problems for the subscriber if the callback code is not thread-safe since you now have another thread calling into subscriber code base at some unknown interval.</p>

<p>One solution for making a callback function thread-safe is to post a message to the subscriber&#39;s OS queue during the publisher&#39;s callback. The subscriber&#39;s thread later dequeues the message and calls an appropriate function. Since the callback implementation only posts a message, the callback, even if done asynchronously, is thread-safe. In this case, the asynchrony of a message queue provides the thread safety in lieu of software locks.</p>

<p>Callbacks are typically free functions, either a class <code>static</code> member or a global function. In C++, instance member functions are handled differently and have significant limitations when it comes to member function pointers. I won&#39;t go into all the sorted details, the topic has been covered endlessly elsewhere, but suffice to say you can&#39;t have a single pointer point to all function types. This framework supports calling free functions, but offers support to get the call back onto an instance member function.</p>

# Using the Code

<p>I&#39;ll first present how to use the code, and then get into the implementation details.</p>

<p>A publisher uses the&nbsp;<code>AsycCallback&lt;&gt;</code> class to expose a callback interface to potential subscribers. An instance is created with one template argument &ndash; the user data type for function callback argument. In the example below, an <code>int</code>&nbsp;will become the callback function argument.</p>

<pre lang="C++">
AsyncCallback&lt;int&gt; callback;</pre>

<p>To subscribe to callback, create a free function (<code>static</code> member or global) as shown.&nbsp;I&rsquo;ll explain why the <code>&lt;int&gt;</code> argument requires a <code>(const int&amp;, void*)</code> function signature shortly.</p>

<pre lang="C++">
void SimpleCallback(const int&amp; value, void* userData)
{
    cout &lt;&lt; &quot;SimpleCallback &quot; &lt;&lt; value &lt;&lt; endl;
}</pre>

<p>The subscriber registers to receive callbacks using the <code>Register()</code> function. The first argument is a pointer to the callback function. The second argument is a pointer to a thread the callback is to be invoked on.</p>

<pre lang="C++">
callback.Register(&amp;SimpleCallback, &amp;workerThread1);</pre>

<p>When the publisher needs to invoke the callback for all registered subscribers, use <code>operator()</code> or <code>Invoke()</code>. Neither function executes the callback synchronously; instead it dispatches each callback onto the destination thread of control.</p>

<pre lang="C++">
callback(123);
callback.Invoke(123);</pre>

<p>Use <code>Unregister()</code> to unsubscribe a callback.</p>

<pre lang="C++">
callback.Unregister(&amp;SimpleCallback, &amp;workerThread1);</pre>

<p>Alternatively, to unregister all callbacks use <code>Clear()</code>.</p>

<pre lang="C++">
callback.Clear();</pre>

<p>Always check if anyone is subscribed to the callback before invocation using one of these two methods.</p>

<pre lang="C++">
if (callback)
    callback(123);
if (!callback.Empty())
    callback(123);</pre>

<p>An <code>AsyncCallback&lt;&gt;</code> is easily used to add asynchrony to both incoming and outgoing API interfaces. The following examples show how.</p>

## SysData Example

<p><code>SysData</code> is a simple class showing how to expose an <em>outgoing </em>asynchronous interface. The class stores system data and provides asynchronous subscriber&nbsp;notifications when the mode changes. The class interface is shown below.</p>

<pre lang="C++">
class SysData
{
public:
    /// Clients register with AsyncCallback to get callbacks when system mode changes
    AsyncCallback&lt;SystemModeChanged&gt; SystemModeChangedCallback;

    /// Get singleton instance of this class
    static SysData&amp; GetInstance();

    /// Sets the system mode and notify registered clients via SystemModeChangedCallback.
    /// @param[in] systemMode - the new system mode. 
    void SetSystemMode(SystemMode::Type systemMode);    

private:
    SysData();
    ~SysData();

    /// The current system mode data
    SystemMode::Type m_systemMode;

    /// Lock to make the class thread-safe
    LOCK m_lock;
};</pre>

<p>The subscriber interface for&nbsp;receiving&nbsp;callbacks is <code>SystemModeChangedCallback</code>. Calling <code>SetSystemMode() </code>saves the new mode into <code>m_systemMode</code>&nbsp;and notifies all registered&nbsp;subscribers.</p>

<pre lang="C++">
void SysData::SetSystemMode(SystemMode::Type systemMode)
{
    LockGuard lockGuard(&amp;m_lock);

    // Create the callback data
    SystemModeChanged callbackData;
    callbackData.PreviousSystemMode = m_systemMode;
    callbackData.CurrentSystemMode = systemMode;

    // Update the system mode
    m_systemMode = systemMode;

    // Callback all registered subscribers
    if (SystemModeChangedCallback)
        SystemModeChangedCallback(callbackData);
}</pre>

## SysDataClient Example

<p><code>SysDataClient</code> is a callback subscriber and registers for notifications within the constructor. Notice the third argument to <code>Register()</code> is a <code>this</code> pointer. The&nbsp;pointer&nbsp;is passed back as <code>userData </code>on each callback. The framework internally does nothing with <code>userData</code> other that pass it back to the callback invocation. The <code>userData </code>value can be anything the caller wants.</p>

<pre lang="C++">
// Constructor
SysDataClient() :
    m_numberOfCallbacks(0)
{
    // Register for async callbacks
    SysData::GetInstance().SystemModeChangedCallback.Register(&amp;SysDataClient::CallbackFunction, 
        &amp;workerThread1, this);    
}</pre>

<p><code>SysDataClient::CallbackFunction()</code>&nbsp;is now called when the system mode changes. Note that the <code>userData</code> argument is typecast back to a <code>SysDataClient</code> instance. Since <code>Register()</code> provided a <code>this</code> pointer, the callback function is able to access any object instance or function during execution.</p>

<pre lang="C++">
static void CallbackFunction(const SystemModeChanged&amp; data, void* userData)
{
    // The user data pointer originates from the 3rd argument in the Register() function
    // Typecast the void* to SysDataClient* to access object instance data/functions.
    SysDataClient* instance = static_cast&lt;SysDataClient*&gt;(userData);
    instance-&gt;m_numberOfCallbacks++;

    cout &lt;&lt; &quot;CallbackFunction &quot; &lt;&lt; data.CurrentSystemMode &lt;&lt; endl;
}</pre>

<p>When <code>SetSystemMode()</code> is called, anyone interested in the mode changes are notified asynchronously on their desired execution thread.</p>

<pre lang="C++">
// Set new SystemMode values. Each call will invoke callbacks to all 
// registered client subscribers.
SysData::GetInstance().SetSystemMode(SystemMode::STARTING);
SysData::GetInstance().SetSystemMode(SystemMode::NORMAL);</pre>

## SysDataNoLock Example

<p><code>SysDataNoLocks </code>is an alternate implementation that uses&nbsp;a <code>private</code> <code>AsyncCallback&lt;&gt;</code>&nbsp;for setting the system mode asynchronously and without locks.</p>

<pre lang="C++">
class SysDataNoLock
{
public:
&nbsp;&nbsp; &nbsp;/// Clients register with AsyncCallback to get callbacks when system mode changes
&nbsp;&nbsp; &nbsp;AsyncCallback&lt;SystemModeChanged&gt; SystemModeChangedCallback;

&nbsp;&nbsp; &nbsp;/// Get singleton instance of this class
&nbsp;&nbsp; &nbsp;static SysDataNoLock&amp; GetInstance();

&nbsp;&nbsp; &nbsp;/// Sets the system mode and notify registered clients via SystemModeChangedCallback.
&nbsp;&nbsp; &nbsp;/// @param[in] systemMode - the new system mode.&nbsp;
&nbsp;&nbsp; &nbsp;void SetSystemMode(SystemMode::Type systemMode);&nbsp;&nbsp; &nbsp;

private:
&nbsp;&nbsp; &nbsp;SysDataNoLock();
&nbsp;&nbsp; &nbsp;~SysDataNoLock();

&nbsp;&nbsp; &nbsp;/// Private callback to get the SetSystemMode call onto a common thread
&nbsp;&nbsp; &nbsp;AsyncCallback&lt;SystemMode::Type&gt; SetSystemModeCallback;

&nbsp;&nbsp; &nbsp;/// Sets the system mode and notify registered clients via SystemModeChangedCallback.
&nbsp;&nbsp; &nbsp;/// @param[in] systemMode - the new system mode.&nbsp;
&nbsp;&nbsp; &nbsp;/// @param[in] userData - a &#39;this&#39; pointer to SysDataNoLock. &nbsp;
&nbsp;&nbsp; &nbsp;static void SetSystemModePrivate(const SystemMode::Type&amp; systemMode, void* userData);&nbsp;&nbsp; &nbsp;

&nbsp;&nbsp; &nbsp;/// The current system mode data
&nbsp;&nbsp; &nbsp;SystemMode::Type m_systemMode;
};</pre>

<p>The constructor registers&nbsp;<code>SetSystemModePrivate()</code>&nbsp;with the <code>private</code> <code>SetSystemModeCallback</code>.</p>

<pre lang="C++">
SysDataNoLock::SysDataNoLock() :
    m_systemMode(SystemMode::STARTING)
{
    SetSystemModeCallback.Register(&amp;SysDataNoLock::SetSystemModePrivate, &amp;workerThread2, this);
    workerThread2.CreateThread();
}</pre>

<p>The <code>SetSystemMode()</code> function below is an example of an asynchronous <em>incoming</em> interface. To the caller, it looks like a normal function, but under the hood, a private member call is invoked asynchronously.&nbsp;In this case, invoking <code>SetSystemModeCallback</code>&nbsp;causes&nbsp;<code>SetSystemModePrivate()</code>&nbsp;to be called on <code>workerThread2</code>.</p>

<pre lang="C++">
void SysDataNoLock::SetSystemMode(SystemMode::Type systemMode)
{
&nbsp;&nbsp; &nbsp;// Invoke the private callback. SetSystemModePrivate() will be called on workerThread2.
&nbsp;&nbsp; &nbsp;SetSystemModeCallback(systemMode);
}</pre>

<p>Since this <code>private</code> function is always invoked asynchronously on <code>workerThread2</code>&nbsp;it doesn&#39;t require locks.</p>

<pre lang="C++">
void SysDataNoLock::SetSystemModePrivate(const SystemMode::Type&amp; systemMode, void* userData)
{
&nbsp;&nbsp; &nbsp;SysDataNoLock* instance = static_cast&lt;SysDataNoLock*&gt;(userData);

&nbsp;&nbsp; &nbsp;// Create the callback data
&nbsp;&nbsp; &nbsp;SystemModeChanged callbackData;
&nbsp;&nbsp; &nbsp;callbackData.PreviousSystemMode = instance-&gt;m_systemMode;
&nbsp;&nbsp; &nbsp;callbackData.CurrentSystemMode = systemMode;

&nbsp;&nbsp; &nbsp;// Update the system mode
&nbsp;&nbsp; &nbsp;instance-&gt;m_systemMode = systemMode;

&nbsp;&nbsp; &nbsp;// Callback all registered subscribers
&nbsp;&nbsp; &nbsp;if (instance-&gt;SystemModeChangedCallback)
&nbsp;&nbsp; &nbsp;&nbsp;&nbsp; &nbsp;instance-&gt;SystemModeChangedCallback(callbackData);
}</pre>

## Timer Example

<p>Once a callback framework is in place, creating a timer callback service is trivial. Many systems need a way to generate a callback based on a timeout. Maybe it&#39;s a periodic timeout for some low speed polling, or maybe an error timeout in case something doesn&#39;t occur within the expected time frame. Either way, the callback must occur on a specified thread of control. An <code>AsyncCallback&lt;&gt;</code> used inside a <code>Timer</code> class solves this nicely.</p>

<pre lang="C++">
class Timer
{
public:
    AsyncCallback&lt;TimerData&gt; Expired;

    void Start(UINT32 timeout);
    void Stop();
    //...
};</pre>

<p>Users create an instance of the timer and register for the expiration. In this case, <code>MyCallback()</code> is called in 1000ms.</p>

<pre lang="C++">
m_timer.Expired.Register(&amp;MyClass::MyCallback, &amp;myThread, this);
m_timer.Start(1000);</pre>

<p>A <code>Timer </code>implementation isn&#39;t offered in the examples. However, the article &quot;<strong><a href="http://www.codeproject.com/Articles/1156423/Cplusplus-State-Machine-with-Threads">C++ State Machine with Threads</a></strong>&quot; contains a <code>Timer</code> class that shows a complete multithreaded example of <code>AsyncCallback&lt;&gt;</code> integrated with a C++ state machine.</p>

# Callback Signature Limitations

<p>This design has the following limitations imposed on all callback functions:</p>

<ol>
	<li>Each callback handles a single user defined argument type (<code>TData</code>).</li>
	<li>The two callback function arguments are always: <code>const TData&amp;</code>&nbsp;and <code>void*</code>.</li>
	<li>Each callback has a <code>void </code>return type.</li>
</ol>

<p>For instance, if an <code>AsyncCallback&lt;&gt;</code> is declared as:</p>

<pre lang="C++">
AsyncCallback&lt;MyData&gt; myCallback;</pre>

<p>The callback function signature is:</p>

<pre lang="C++">
void MyCallback(const MyData&amp; data, void* userData);</pre>

<p>The design can be extended to support more than one argument if necessary. However, the design somewhat mimics what embedded programmers do all the time, which is something like:</p>

<ol>
	<li>Dynamically create an instance to a <code>struct</code> or <code>class</code> and populate data.</li>
	<li>Post a pointer to the data through an OS message as a <code>void*</code>.</li>
	<li>Get the data from the OS message queue and typecast the <code>void*</code> back to the original type.</li>
	<li>Delete the dynamically created data.</li>
</ol>

<p>In this design, the entire infrastructure happens automatically without any additional effort on the programmer&#39;s part. If multiple data parameters are required, they must be packaged into a single <code>class</code>/<code>struct</code>&nbsp;and used as the callback data argument.</p>

# Implementation

<p>The number of lines of code for the callback framework is surprisingly low. Strip out the comments, and maybe a couple hundred lines of code that are (hopefully) easy to understand and maintain.</p>

<p><code>AsyncCallback&lt;&gt;</code> and <code>AsyncCallbackBase </code>form the basis for publishing a callback interface. The classes are thread-safe. The base version is non-templatized to reduce code space. <code>AsyncCallbackBase</code> provides the invocation list and thread safety mechanisms.</p>

<p><code>AsyncCallback::Invoke()</code> iterates over the list and dispatches callback messages to each target thread. The data is dynamically created to travel through an OS message queue.</p>

<pre lang="C++">
void Invoke(const TData&amp; data) 
{
    LockGuard lockGuard(GetLock());

    // For each registered callback 
    InvocationNode* node = GetInvocationHead();
    while (node != NULL)
    {
        // Create a new instance of callback and copy
        const Callback* callback = new Callback(*node-&gt;CallbackElement);

        // Create a new instance of the callback data and copy
        const TData* callbackData = new TData(data);

        // Create a new message  instance 
        CallbackMsg* msg = new CallbackMsg(this, callback, callbackData);

        // Dispatch message onto the callback destination thread. TargetInvoke()
        // will be called by the target thread. 
        callback-&gt;GetCallbackThread()-&gt;DispatchCallback(msg);

        // Get the next registered callback subscriber 
        node = node-&gt;Next;
    }
}</pre>

<p><code>AsyncCallback::TargetInvoke()</code> is called by target thread to actually execute the callback. Dynamic data is deleted after the callback is invoked.</p>

<pre lang="C++">
virtual void TargetInvoke(CallbackMsg** msg) const
{
    const Callback* callback = (*msg)-&gt;GetCallback();

    // Typecast the void* back to a TData type
    const TData* callbackData = static_cast&lt;const TData*&gt;((*msg)-&gt;GetCallbackData());

    // Typecast a generic callback function pointer to the CallbackFunc type
    CallbackFunc func = reinterpret_cast&lt;CallbackFunc&gt;(callback-&gt;GetCallbackFunction());

    // Execute the registered callback function
    (*func)(*callbackData, callback-&gt;GetUserData());

    // Delete dynamically data sent through the message queue
    delete callbackData;
    delete callback;
    delete *msg;
    *msg = NULL;
}</pre>

<p>Asynchronous callbacks impose certain limitations because everything the callback destination thread needs must be created on the heap, packaged into a class, and placed into an OS message queue.</p>

<p>The insertion into an OS queue is platform specific. The <code>CallbackThread</code> class provides the interface to be implemented on each target platform. See the <strong>Porting </strong>section below for a more complete discussion.</p>

<pre lang="C++">
class CallbackThread
{
public:
    virtual void DispatchCallback(CallbackMsg* msg) = 0;
};</pre>

<p>Once the message is placed into the message queue, platform specific code unpacks the message and calls the <code>AsyncCallbackBase::TargetInvoke()</code> function and destroys dynamically allocated data.</p>

<pre lang="C++">
unsigned long WorkerThread::Process(void* parameter)
{
    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&amp;msg, NULL, WM_USER_BEGIN, WM_USER_END)) != 0)
    {
        switch (msg.message)
        {
            case WM_DISPATCH_CALLBACK:
            {
                ASSERT_TRUE(msg.wParam != NULL);

                // Get the ThreadMsg from the wParam value
                ThreadMsg* threadMsg = reinterpret_cast&lt;ThreadMsg*&gt;(msg.wParam);

                // Convert the ThreadMsg void* data back to a CallbackMsg* 
                CallbackMsg* callbackMsg = static_cast&lt;CallbackMsg*&gt;(threadMsg-&gt;GetData()); 

                // Invoke the callback callback on the target thread
                callbackMsg-&gt;GetAsyncCallback()-&gt;TargetInvoke(&amp;callbackMsg);

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
}</pre>

<p>Notice the thread loop is unlike most systems that have a huge <code>switch</code> statement handling various incoming data messages, type casting <code>void*</code> data, then calling a specific function. The framework supports all callbacks with a single <code>WM_DISPATCH_CALLBACK</code> message. Once setup, the same small thread loop handles every callback. New publisher and subscribers come and go as the system is designed, but the code in-between doesn&#39;t change.</p>

<p>This is a huge benefit as on many systems getting data between threads takes a lot of manual steps. You constantly have to mess with each thread loop, create during sending, destroy data when receiving, and call various OS services and typecasts. Here you do none of that. All the stuff in-between is neatly handled for users.</p>

# Heap

<p>The heap is used to create dynamic data. It stems from using an invocation list and needing to send data objects through the message queue. Remember, your callback data is copied and destroyed during a callback. Most times, the callback data is POD (Plain Old Data Structure). If you have something fancier that can&#39;t be bitwise copied, be sure to implement a copy constructor for the callback data.</p>

<p>On some systems, it is undesirable to use the heap. For those situations, I use a fixed block memory allocator. The <code>xallocator</code> implementation solves the dynamic storage issues and is much faster than the global heap. To use, just include <em>xallocator.h</em> and add the macro <code>XALLOCATOR </code>to the class declaration. An entire class hierarchy can use the fixed block allocator by placing <code>XALLOCTOR</code> in the base class.</p>

<pre lang="C++">
#include &quot;xallocator.h&quot;

class Callback 
{
    XALLOCATOR
    // ...
};</pre>

<p>With <code>xallocator</code> in place, calling <code>operator new</code> or <code>delete </code>allows the fixed block allocator to take over the storage duties. How objects are created and destroyed is exactly the same, only the source of the memory is different. For more information on <code>xallocator</code>, and to get the source code, see the article &quot;<strong><a href="http://www.codeproject.com/Articles/1084801/Replace-malloc-free-with-a-Fast-Fixed-Block-Memory">Replace malloc/free with a Fast Fixed Block Memory Allocator</a></strong>&quot;. The only files needed are <em>Allocator.h/cpp</em> and <em>xallocator.h/cpp</em>.</p>

<p>To use <code>xallocator</code> in the callback framework, place <code>XALLOCATOR</code><strong> </strong>macros in the following class definitions:</p>

<ul>
	<li><code>Callback</code></li>
	<li><code>CallbackMsg</code></li>
	<li><code>InvocationNode</code></li>
</ul>

<p>For the platform specific files, you also include <code>XALLOCATOR</code>. In this example, these are:</p>

<ul>
	<li><code>ThreadMsg</code></li>
	<li><code>SystemModeChanged</code></li>
</ul>

# Porting

<p>The code is an easy port to any platform. There are only two OS services required: threads and a software lock. The code is separated into five directories.&nbsp;</p>

<ol>
	<li><code>AsyncCallback</code><strong> </strong>- core framework implementation files</li>
	<li><code>PortWin</code><strong> </strong>&ndash; Windows-specific files (thread/lock)</li>
	<li><code>Examples</code> &ndash; sample code showing usage</li>
	<li><code>VS2008 </code>&ndash;&nbsp;&nbsp;Visual Studio 2008 project files</li>
	<li><code>VS2015</code> &ndash;&nbsp;Visual Studio 2015 project files</li>
</ol>

<p>The library has a single <code>abstract</code> class <code>CallbackThread </code>with a single pure <code>virtual</code> function:</p>

<pre lang="C++">
virtual void DispatchCallback(CallbackMsg* msg) = 0;</pre>

<p>On most projects, I wrap the underlying raw OS calls into a thread class to encapsulate and enforce the correct behavior. Here, I provide <code>ThreadWin</code> as a wrapper over the <code>CreateThread()</code> Windows API.</p>

<p>Once you have a thread class, just inherit the <code>CallbackThread</code> interface and implement the <code>DispatchCallback()</code> function. On Windows, a simple post to a message queue is all that is required:</p>

<pre lang="C++">
void ThreadWin::DispatchCallback(CallbackMsg* msg)
{
    // Create a new ThreadMsg
    ThreadMsg* threadMsg = new ThreadMsg(WM_DISPATCH_CALLBACK, msg);

    // Post the message to the this thread&#39;s message queue
    PostThreadMessage(WM_DISPATCH_CALLBACK, threadMsg);
}</pre>

<p>The Windows thread loop gets the message and calls the <code>TargetInvoke()</code> function for the incoming instance. The data sent through the queue is deleted once complete.</p>

<pre lang="C++">
switch (msg.message)
{
    case WM_DISPATCH_CALLBACK:
    {
        ASSERT_TRUE(msg.wParam != NULL);

        // Get the ThreadMsg from the wParam value
        ThreadMsg* threadMsg = reinterpret_cast&lt;ThreadMsg*&gt;(msg.wParam);

        // Convert the ThreadMsg void* data back to a CallbackMsg* 
        CallbackMsg* callbackMsg = static_cast&lt;CallbackMsg*&gt;(threadMsg-&gt;GetData()); 

        // Invoke the callback callback on the target thread
        callbackMsg-&gt;GetAsyncCallback()-&gt;TargetInvoke(&amp;callbackMsg);

        // Delete dynamic data passed through message queue
        delete threadMsg;
        break;
    }

    case WM_EXIT_THREAD:
        return 0;

    default:
        ASSERT();
}</pre>

<p>Software locks are handled by the <code>LockGuard</code> class. This class can be updated with locks of your choice, or you can use a different mechanism. Locks are only used in a few places.</p>

# Code Size

<p>To gauge the cost of using this technique, the code was built for an ARM CPU using Keil. If deployed on a project, many <code>AsyncCallback&lt;&gt;</code> instances will be created so it needs to be space efficient.</p>

<p>The incremental code size of for each additional <code>AsyncCallback&lt;&gt;</code>, one subscriber, one registration call, and one callback invocation is around 120 bytes using full optimization. You&rsquo;d certainly use at least this much code moving data from one thread to another manually.</p>

# References

<ul>
	<li><a href="http://www.codeproject.com/Articles/1084801/Replace-malloc-free-with-a-Fast-Fixed-Block-Memory"><strong>Replace malloc/free with a Fast Fixed Block Memory Allocator</strong></a> - by David Lafreniere</li>
	<li><a href="http://www.codeproject.com/Articles/1095196/Win32-Thread-Wrapper-with-Synchronized-Start"><strong>Win32 Thread Wrapper with Synchronized Start</strong></a> - by David Lafreniere</li>
	<li><strong><a href="http://www.codeproject.com/Articles/1156423/Cplusplus-State-Machine-with-Threads">C++ State Machine with Threads</a></strong> - by David Lafreniere</li>
</ul>

# Conclusion

<p>There are many ways to design a publisher/subscriber callback system. This version incorporates unique features I&#39;ve never seen before, especially the ease at which asynchronous callbacks are generated onto a client specified thread of control. The implementation was kept to a minimum to facilitate porting to any system embedded or otherwise.</p>

<p>I&#39;ve used this technique on projects with great success. Each class or subsystem may expose one or more outgoing interfaces with <code>AsyncCallback&lt;&gt;</code> instances. Any code within the system is able to connect and receive asynchronous callbacks with worrying about cross-threading or the machinery to make it all work. A feature like this eases application design and architecturally standardizes inter-thread communication with a well-understood callback paradigm.</p>



