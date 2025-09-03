#include "Player/SHCPlayerController.h"

#include "UI/SHCChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HW09S.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/SHCGameModeBase.h"
#include "SHCPlayerState.h"
#include "Net/UnrealNetwork.h"

ASHCPlayerController::ASHCPlayerController()
{
	bReplicates = true;
}

void ASHCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<USHCChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}
	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ASHCPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ASHCPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	if (IsLocalController() == true)
	{
		ASHCPlayerState* SHCPS = GetPlayerState<ASHCPlayerState>();
		if (IsValid(SHCPS) == true)
		{
			FString CombinedMessageString = SHCPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ASHCPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	HW09SFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ASHCPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ASHCPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ASHCGameModeBase* SHCGM = Cast<ASHCGameModeBase>(GM);
		if (IsValid(SHCGM) == true)
		{
			SHCGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

