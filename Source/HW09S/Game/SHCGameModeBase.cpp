#include "Game/SHCGameModeBase.h"

#include "SHCGameStateBase.h"
#include "Player/SHCPlayerController.h"
#include "EngineUtils.h"
#include "Player/SHCPlayerState.h"

void ASHCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
}

void ASHCGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ASHCPlayerController* SHCPlayerController = Cast<ASHCPlayerController>(NewPlayer);
	if (IsValid(SHCPlayerController) == true)
	{
		SHCPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		AllPlayerControllers.Add(SHCPlayerController);

		ASHCPlayerState* SHCPS = SHCPlayerController->GetPlayerState<ASHCPlayerState>();
		if (IsValid(SHCPS) == true)
		{
			SHCPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ASHCGameStateBase* CXGameStateBase = GetGameState<ASHCGameStateBase>();
		if (IsValid(CXGameStateBase) == true)
		{
			CXGameStateBase->MulticastRPCBroadcastLoginMessage(SHCPS->PlayerNameString);
		}
	}

}

FString ASHCGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ASHCGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString ASHCGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& IsGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == IsGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), IsGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ASHCGameModeBase::PrintChatMessageString(ASHCPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		IncreaseGuessCount(InChattingPlayerController);
		for (TActorIterator<ASHCPlayerController> It(GetWorld()); It; ++It)
		{
			ASHCPlayerController* SHCPlayerController = *It;
			if (IsValid(SHCPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				SHCPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<ASHCPlayerController> It(GetWorld()); It; ++It)
		{
			ASHCPlayerController* SHCPlayerController = *It;
			if (IsValid(SHCPlayerController) == true)
			{
				SHCPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void ASHCGameModeBase::IncreaseGuessCount(ASHCPlayerController* InChattingPlayerController)
{
	ASHCPlayerState* SHCPS = InChattingPlayerController->GetPlayerState<ASHCPlayerState>();
	if (IsValid(SHCPS) == true)
	{
		SHCPS->CurrentGuessCount++;
	}
}

void ASHCGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& SHCPlayerController : AllPlayerControllers)
	{
		ASHCPlayerState* SHCPS = SHCPlayerController->GetPlayerState<ASHCPlayerState>();
		if (IsValid(SHCPS) == true)
		{
			SHCPS->CurrentGuessCount = 0;
		}
	}
}

void ASHCGameModeBase::JudgeGame(ASHCPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ASHCPlayerState* SHCPS = InChattingPlayerController->GetPlayerState<ASHCPlayerState>();
		for (const auto& SHCPlayerController : AllPlayerControllers)
		{
			if (IsValid(SHCPS) == true)
			{
				FString CombinedMessageString = SHCPS->PlayerNameString + TEXT(" has won the game.");
				SHCPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& SHCPlayerController : AllPlayerControllers)
		{
			ASHCPlayerState* SHCPS = SHCPlayerController->GetPlayerState<ASHCPlayerState>();
			if (IsValid(SHCPS) == true)
			{
				if (SHCPS->CurrentGuessCount < SHCPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& SHCPlayerController : AllPlayerControllers)
			{
				SHCPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}
