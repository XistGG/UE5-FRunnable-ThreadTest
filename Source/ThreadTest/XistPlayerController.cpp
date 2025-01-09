// Copyright (c) 2025 Xist.GG LLC

#include "XistPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "RunnableNonsense.h"
#include "XistLog.h"
#include "Engine/LocalPlayer.h"

AXistPlayerController::AXistPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	ShortPressThreshold = 0.2f;  // 200 ms
	FollowTime = 0.f;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BP_Cursor(TEXT("/Game/UI/Cursor/FX_Cursor"));
	FXCursor = BP_Cursor.Object;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> BP_IMC(TEXT("/Game/Input/IMC_Default"));
	DefaultMappingContext = BP_IMC.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> BP_ClickAction(TEXT("/Game/Input/Actions/IA_SetDestination_Click"));
	SetDestinationClickAction = BP_ClickAction.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> BP_TouchAction(TEXT("/Game/Input/Actions/IA_SetDestination_Touch"));
	SetDestinationTouchAction = BP_TouchAction.Object;
}

void AXistPlayerController::StartNonsense(int32 Num)
{
	for (int32 i = 0; i < Num; i++)
	{
		FRunnableNonsense* Nonsense = new FRunnableNonsense();

		// Start the thread running
		Nonsense->Start();

		NonsenseThreads.Add(Nonsense->GetId(), Nonsense);
	}
}

void AXistPlayerController::StopNonsense()
{
	for (auto& Tuple : NonsenseThreads)
	{
		FRunnableNonsense* Nonsense = Tuple.Value;
		if (Nonsense && !Nonsense->HasExited())
		{
			// Instruct this thread to stop.
			// This doesn't actually stop it or exit it, it must do that on its own.
			// Cleanup is done later after it actually exits, in AXistPlayerController::Tick()

			Nonsense->Stop();
		}
	}
}

void AXistPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AXistPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// This blocks the Game Thread for a while when there are a lot of threads running
	for (auto& Tuple : NonsenseThreads)
	{
		FRunnableNonsense* Nonsense = Tuple.Value;
		delete Nonsense;  // This waits for the thread to exit
	}
	NonsenseThreads.Empty();

	Super::EndPlay(EndPlayReason);
}

void AXistPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Detect NonsenseThreads that have finished running

	TSet<uint32> KeysToPrune;
	for (auto& Tuple : NonsenseThreads)
	{
		FRunnableNonsense* Nonsense = Tuple.Value;
		if (Nonsense && Nonsense->HasExited())
		{
			// This NonsenseThread has already exited, we need to free memory associated with it
			KeysToPrune.Add(Tuple.Key);
		}
	}

	// Free memory associated with the threads that have exited

	if (KeysToPrune.Num() > 0)
	{
		UE_LOG(LogXist, Log, TEXT("Removing exited (%d/%d) nonsense threads"), KeysToPrune.Num(), NonsenseThreads.Num());

		for (uint32 Key : KeysToPrune)
		{
			FRunnableNonsense* Nonsense = NonsenseThreads.FindChecked(Key);
			delete Nonsense;

			NonsenseThreads.Remove(Key);
		}

		if (NonsenseThreads.IsEmpty())
		{
			UE_LOG(LogXist, Log, TEXT("All nonsense threads have exited"));
		}
	}
}

void AXistPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AXistPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AXistPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AXistPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AXistPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AXistPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AXistPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AXistPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AXistPlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogXist, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AXistPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AXistPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AXistPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AXistPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AXistPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}
