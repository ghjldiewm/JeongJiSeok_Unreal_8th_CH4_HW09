// TBPawn.cpp


#include "Player/TBPawn.h"
#include "TextBaseball.h"


void ATBPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = TextBaseballFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("TBPawn::BeginPlay() %s [%s]"), *TextBaseballFunctionLibrary::GetNetModeString(this), *NetRoleString);
	TextBaseballFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ATBPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = TextBaseballFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("TBPawn::PossessedBy() %s [%s]"), *TextBaseballFunctionLibrary::GetNetModeString(this), *NetRoleString);
	TextBaseballFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

