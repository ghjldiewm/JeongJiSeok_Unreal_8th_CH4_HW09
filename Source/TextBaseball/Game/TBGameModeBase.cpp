// TBGameModeBase.cpp


#include "Game/TBGameModeBase.h"
#include "TBGameStateBase.h"
#include "Player/TBPlayerController.h"
#include "EngineUtils.h"
#include <Player/TBPlayerState.h>

void ATBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	/*ATBGameStateBase* TBGameStateBase = GetGameState<ATBGameStateBase>();
	if (IsValid(TBGameStateBase) == true)
	{
		TBGameStateBase->MulticastRPCBroadcastLoginMessage(TEXT("XXXXXXX"));
	}

	ATBPlayerController* TBPlayerController = Cast<ATBPlayerController>(NewPlayer);
	if (IsValid(TBPlayerController) == true)
	{
		AllPlayerControllers.Add(TBPlayerController);
	}*/

	ATBPlayerController* TBPlayerController = Cast<ATBPlayerController>(NewPlayer);
	if (IsValid(TBPlayerController) == true)
	{
		TBPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(TBPlayerController);

		ATBPlayerState* TBPS = TBPlayerController->GetPlayerState<ATBPlayerState>();
		if (IsValid(TBPS) == true)
		{
			TBPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ATBGameStateBase* TBGameStateBase = GetGameState<ATBGameStateBase>();
		if (IsValid(TBGameStateBase) == true)
		{
			TBGameStateBase->MulticastRPCBroadcastLoginMessage(TBPS->PlayerNameString);
		}
	}
}

FString ATBGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) {return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ATBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do
	{
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

FString ATBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
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

void ATBGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);
}

void ATBGameModeBase::PrintChatMessageString(ATBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController);

		ATBPlayerState* TBPS = InChattingPlayerController->GetPlayerState<ATBPlayerState>();
		if (IsValid(TBPS) == true)
		{
			FString CombinedMessageString_PlayerInfo = TBPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

			for (TActorIterator<ATBPlayerController> It(GetWorld()); It; ++It)
			{
				ATBPlayerController* TBPlayerController = *It;
				if (IsValid(TBPlayerController) == true)
				{
					FString CombinedMessageString = CombinedMessageString_PlayerInfo + TEXT(" -> ") + JudgeResultString;
					TBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

					int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
					JudgeGame(InChattingPlayerController, StrikeCount);
				}
			}
		}

	}
	else
	{
		for (TActorIterator<ATBPlayerController> It(GetWorld()); It; ++It)
		{
			ATBPlayerController* TBPlayerController = *It;
			if (IsValid(TBPlayerController) == true)
			{
				ATBPlayerState* TBPS = InChattingPlayerController->GetPlayerState<ATBPlayerState>();

				if (IsValid(TBPS) == true)
				{
				FString CombinedMessageString = TBPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

				TBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
				}
			}
		}
	}
}

void ATBGameModeBase::IncreaseGuessCount(ATBPlayerController* InChattingPlayerController)
{
	ATBPlayerState* TBPS = InChattingPlayerController->GetPlayerState<ATBPlayerState>();
	if (IsValid(TBPS) == true)
	{
		TBPS->CurrentGuessCount++;
	}
}

void ATBGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& TBPlayerController : AllPlayerControllers)
	{
		ATBPlayerState* TBPS = TBPlayerController->GetPlayerState<ATBPlayerState>();
		if (IsValid(TBPS) == true)
		{
			TBPS->CurrentGuessCount = 0;
		}
	}
}

void ATBGameModeBase::JudgeGame(ATBPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ATBPlayerState* TBPS = InChattingPlayerController->GetPlayerState<ATBPlayerState>();
		for (const auto& TBPlayerController : AllPlayerControllers)
		{
			if (IsValid(TBPS) == true)
			{
				FString CombinedMessageString = TBPS->PlayerNameString + TEXT(" has won the game.");
				TBPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& TBPlayerController : AllPlayerControllers)
		{
			ATBPlayerState* TBPS = TBPlayerController->GetPlayerState<ATBPlayerState>();
			if (IsValid(TBPS) == true)
			{
				if (TBPS->CurrentGuessCount < TBPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& TBPlayerController : AllPlayerControllers)
			{
				TBPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}