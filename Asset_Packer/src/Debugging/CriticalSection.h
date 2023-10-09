#pragma once

#define WIN32_LEAN_AND_MEAN	
#include <windows.h>

/// <summary>
/// Makes use of the windows critical section to ensure thread safety. This
/// version must be manually locked and unlocked.
/// </summary>
class CriticalSection
{
public:
	/// <summary>
	/// Initialize the critical section.
	/// </summary>
	CriticalSection()
	{
		InitializeCriticalSection(&critical_section);
	}

	CriticalSection(const CriticalSection&) = delete;
	CriticalSection& operator=(const CriticalSection&) = delete;

	/// <summary>
	/// Delete the critical section.
	/// </summary>
	~CriticalSection()
	{
		DeleteCriticalSection(&critical_section);
	}

	/// <summary>
	/// Lock the critical section.
	/// </summary>
	void lock()
	{
		EnterCriticalSection(&critical_section);
	}

	/// <summary>
	/// Unlock the critical section.
	/// </summary>
	void unlock()
	{
		LeaveCriticalSection(&critical_section);
	}
	
protected:
	/// <summary>
	/// The actual critical section that we are using.
	/// </summary>
	mutable CRITICAL_SECTION critical_section;
};

/// <summary>
/// Allows for the automatic locking a critical section 
/// just by creating this object, and unlocking when this goes out of scope.
/// </summary>
class ScopedCriticalSection
{
public:
	/// <summary>
	/// Lock the provided critical section until this object goes out of scope.
	/// </summary>
	/// <param name="critical_section_to_use">The section to lock.</param>
	ScopedCriticalSection(CriticalSection& critical_section_to_use)
		: critical_section(critical_section_to_use)
	{
		critical_section.lock();
	}

	/// <summary>
	/// Not copyable.
	/// </summary>
	ScopedCriticalSection(const CriticalSection&) = delete;

	/// <summary>
	/// Not copyable.
	/// </summary>
	ScopedCriticalSection& operator=(const ScopedCriticalSection&) = delete;

	/// <summary>
	/// Delete the critical section.
	/// </summary>
	~ScopedCriticalSection()
	{
		critical_section.unlock();
	}

protected:
	/// <summary>
	/// The critical section that we are using to handle locking and unlocking.
	/// </summary>
	CriticalSection& critical_section;
};

