// Copyright (c) 2025 Xist.GG LLC

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "XistPlayerController.generated.h"

class FRunnableNonsense;
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

UCLASS()
class AXistPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Set class defaults
	AXistPlayerController();

	/**
	 * Start Num FRunnableNonsense threads
	 * @param Num The number of threads to start
	 */
	UFUNCTION(BlueprintCallable)
	void StartNonsense(int32 Num = 1);

	/**
	 * Instruct all currently running FRunnableNonsense threads to stop.
	 *
	 * They don't stop immediately, they have to finish their current Work() first,
	 * but eventually they do stop.
	 */
	UFUNCTION(BlueprintCallable)
	void StopNonsense();

protected:
	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** IMC */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	/** Click to Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	/** Touch to Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

	virtual void SetupInputComponent() override;

	virtual void BeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();

private:
	FVector CachedDestination;

	bool bIsTouch {false}; // Is it a touch device
	float FollowTime; // For how long it has been pressed

	// Map of all current FRunnableNonsense threads we're managing
	TMap<uint32, FRunnableNonsense*> NonsenseThreads;
};
