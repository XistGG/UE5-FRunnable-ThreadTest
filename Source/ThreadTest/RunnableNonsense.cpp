// Copyright (c) 2025 Xist.GG LLC

#include "RunnableNonsense.h"

#include "XistLog.h"

std::atomic<uint32> FRunnableNonsense::ThreadsCreated {0};
const FName FRunnableNonsense::ThreadNamePrefix (TEXT("Nonsense"));

FRunnableNonsense::FRunnableNonsense()
	: ThreadName(TEXT("UNDEFINED"))
{
}

FRunnableNonsense::~FRunnableNonsense()
{
	UE_LOG(LogXist, Log, TEXT("Frame=%llu %s destruction started"), GFrameNumber, *GetName());

	if (Thread != nullptr)
	{
		constexpr bool bWait = true;
		Thread->Kill(bWait);

		delete Thread;
	}

	UE_LOG(LogXist, Log, TEXT("Frame=%llu %s destruction finished"), GFrameNumber, *GetName());
}

void FRunnableNonsense::Start()
{
	// You must call this method whenever you want to actually start the thread running.

	// Assign the Id and Name for the new thread we're starting
	Id = 1 + ThreadsCreated.fetch_add(1);  // never use value 0, so always add 1
	ThreadName = FString::Printf(TEXT("%s[%d]"), *ThreadNamePrefix.ToString(), Id);

	UE_LOG(LogXist, Log, TEXT("Frame=%llu %s starting"), GFrameNumber, *GetName());
	Thread = FRunnableThread::Create(this, *GetName());
}

bool FRunnableNonsense::Init()
{
	// This method is automatically executed after FRunnableThread::Create()
	// If this returns false, the thread will immediately abort.

	UE_LOG(LogXist, Log, TEXT("Frame=%llu %s initialized"), GFrameNumber, *GetName());

	return true;
}

uint32 FRunnableNonsense::Run()
{
	// This method is called automatically after FRunnableThread::Create(),
	// assuming this->Init() returned true.

	UE_LOG(LogXist, Log, TEXT("Frame=%llu %s started running"), GFrameNumber, *GetName());

	bool bContinueWorking {true};
	while (bShouldRun && bContinueWorking)
	{
		bContinueWorking = Work();
		FPlatformProcess::Sleep(SleepInterval);
	}

	UE_LOG(LogXist, Log, TEXT("Frame=%llu %s stopped running"), GFrameNumber, *GetName());
	return 0;
}

void FRunnableNonsense::Stop()
{
	// This method is called automatically by FRunnableThread during Thread->Kill().

	// You can also call this manually if/when you want to abort a thread before it has
	// completed all the work it wants to do.

	UE_LOG(LogXist, Log, TEXT("Frame=%llu %s scheduled to stop"), GFrameNumber, *GetName());

	bShouldRun = false;
}

void FRunnableNonsense::Exit()
{
	// This method is called automatically by FRunnableThread after this->Run() returns.
	// Use this time to clean up any resources the thread has allocated.

	UE_LOG(LogXist, Log, TEXT("Frame=%llu %s exiting"), GFrameNumber, *GetName());

	// ... Clean up after the thread here ...

	bHasExited = true;
}

bool FRunnableNonsense::Work()
{
	const float WorkTime = FMath::RandRange(0.5f, 2.f);
	UE_LOG(LogXist, Log, TEXT("Frame=%llu %s working for %.2f sec"), GFrameNumber, *GetName(), WorkTime);

	FPlatformProcess::Sleep(WorkTime);

	// 90% chance to continue working
	return 0.9f >= FMath::RandRange(0.f, 1.f);
}
