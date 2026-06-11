// TBPlayerController.cpp


#include "Player/TBPlayerController.h"
#include "UI/TBChatInput.h"
#include <Kismet/KismetSystemLibrary.h>
#include <TextBaseball.h>
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/TBGameModeBase.h"
#include "TBPlayerState.h"
#include "Net/UnrealNetwork.h"

ATBPlayerController::ATBPlayerController()
{
	bReplicates = true;
}

void ATBPlayerController::BeginPlay()
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
		ChatInputWidgetInstance = CreateWidget<UTBChatInput>(this, ChatInputWidgetClass);
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

void ATBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	//PrintChatMessageString(ChatMessageString);
	if (IsLocalController() == true)
	{
		ServerRPCPrintChatMessageString(ChatMessageString);
	}
	
}

void ATBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	// TextBaseball.h 에서 만든 MyPrintString() 함수로 대체
	//UKismetSystemLibrary::PrintString(this, ChatMessageString, true, true, FLinearColor::Red, 5.0f);

	// 로깅용 코드
	/*FString NetModeString = TextBaseballFunctionLibrary::GetNetModeString(this);
	FString CombinedMessageString = FString::Printf(TEXT("%s: %s"), *NetModeString, *InChatMessageString);
	TextBaseballFunctionLibrary::MyPrintString(this, CombinedMessageString, 10.f);*/

	TextBaseballFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ATBPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

FString ATBPlayerController::ReEnterGuideMessageString()
{
	return TEXT("다시 입력하세요.");
}

void ATBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ATBPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	/*for (TActorIterator<ATBPlayerController> It(GetWorld()); It; ++It)
	{
		ATBPlayerController* TBPlayerController = *It;
		if (IsValid(TBPlayerController) == true)
		{
			TBPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
		}
	}*/ // ATBGameModeBase::PrintChatMessageString() 이곳으로 옮겨짐

	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ATBGameModeBase* TBGM = Cast<ATBGameModeBase>(GM);
		if (IsValid(TBGM) == true)
		{
			TBGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}
