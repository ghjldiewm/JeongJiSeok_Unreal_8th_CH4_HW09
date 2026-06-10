// TBGameStateBase.cpp


#include "Game/TBGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TBPlayerController.h"

void ATBGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ATBPlayerController* TBPC = Cast<ATBPlayerController>(PC);
			if (IsValid(TBPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				TBPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
