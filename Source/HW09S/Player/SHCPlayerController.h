#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SHCPlayerController.generated.h"

class USHCChatInput;
class UUserWidget;

UCLASS()
class HW09S_API ASHCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASHCPlayerController();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USHCChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<USHCChatInput> ChatInputWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

	FString ChatMessageString;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
	
};
