#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SHCGameStateBase.generated.h"

UCLASS()
class HW09S_API ASHCGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));

};
