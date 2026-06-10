// TBGameModeBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TBGameModeBase.generated.h"

class ATBPlayerController;

/**
 *
 */
UCLASS()
class TEXTBASEBALL_API ATBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(ATBPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ATBPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(ATBPlayerController* InChattingPlayerController, int InStrikeCount);

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ATBPlayerController>> AllPlayerControllers;
};
