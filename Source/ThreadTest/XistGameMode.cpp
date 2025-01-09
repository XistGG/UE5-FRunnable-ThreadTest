// Copyright (c) 2025 Xist.GG LLC

#include "XistGameMode.h"
#include "XistPlayerController.h"
#include "XistCharacter.h"
#include "UObject/ConstructorHelpers.h"

AXistGameMode::AXistGameMode()
{
	// Set default classes
	DefaultPawnClass = AXistCharacter::StaticClass();
	PlayerControllerClass = AXistPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character if possible
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Character"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller if possible
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Blueprints/BP_PlayerController"));
	if (PlayerControllerBPClass.Class != nullptr)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}