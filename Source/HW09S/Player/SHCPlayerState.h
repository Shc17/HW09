#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SHCPlayerState.generated.h"

UCLASS()
class HW09S_API ASHCPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ASHCPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	FString GetPlayerInfoString();
public:
	UPROPERTY(Replicated)
	FString PlayerNameString;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;
};
