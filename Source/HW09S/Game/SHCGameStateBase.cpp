#include "Game/SHCGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/SHCPlayerController.h"

void ASHCGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ASHCPlayerController* SHCPC = Cast<ASHCPlayerController>(PC);
			if (IsValid(SHCPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				SHCPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
