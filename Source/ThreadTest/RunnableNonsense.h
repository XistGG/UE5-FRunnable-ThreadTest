// Copyright (c) 2025 Xist.GG LLC

#pragma once

#include "HAL/Runnable.h"

class FRunnableThread;

/**
 * Runnable Nonsense
 *
 * This thread doesn't actually do anything other than sleep for random time periods,
 * to demonstrate the usage and lifecycle of FRunnable.
 */
class FRunnableNonsense
	: public FRunnable
{
public:
	FRunnableNonsense();
	virtual ~FRunnableNonsense() override;

	// Get the unique thread Id (ONLY VALID AFTER Start())
	uint32 GetId() const { return Id; }

	// Get the unique thread Name (ONLY VALID AFTER Start())
	const FString& GetName() const { return ThreadName; }

	/** @return True as long as the thread is supposed to keep running; False when we want it to stop */
	bool ShouldBeRunning() const { return bShouldRun; }

	/** @return True after the thread has finished running and cleaned up after itself; False before then */
	bool HasExited() const { return bHasExited; }

	/** Start this thread running */
	void Start();

	//~Begin FRunnable interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	//~End of FRunnable interface

private:
	/**
	 * Do one chunk of work
	 *
	 * Ideally you want this to finish in a reasonable period of time.  When we ask a thread
	 * to stop running, it will never respond to that request as long as this Work() method
	 * is executing.  Small chunks are ideal and huge chunks are discouraged.
	 * 
	 * @return True if there is more work to be done; False if all work has been completed.
	 */
	bool Work();

	static const FName ThreadNamePrefix;
	static std::atomic<uint32> ThreadsCreated;

	uint32 Id {0};
	FString ThreadName;
	float SleepInterval {0.5f};

	bool bHasExited {false};
	bool bShouldRun {true};
	FRunnableThread* Thread {nullptr};

};
