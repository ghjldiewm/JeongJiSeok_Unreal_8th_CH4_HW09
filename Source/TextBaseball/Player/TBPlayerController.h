// TBPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TBPlayerController.generated.h"

class UTBChatInput;
class UUserWidget;

/**
 *
 */
UCLASS()
class TEXTBASEBALL_API ATBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATBPlayerController();

	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FString ReEnterGuideMessageString();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTBChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UTBChatInput> ChatInputWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

	FString ChatMessageString;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
};
