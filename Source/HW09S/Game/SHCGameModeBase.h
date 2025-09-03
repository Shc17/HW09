#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SHCGameModeBase.generated.h"

class ASHCPlayerController;


UCLASS()
class HW09S_API ASHCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& IsGuessNumberString);
	
	void PrintChatMessageString(ASHCPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ASHCPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(ASHCPlayerController* InChattingPlayerController, int InStrikeCount);
protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ASHCPlayerController>> AllPlayerControllers;
	
};
