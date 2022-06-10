#include "HostSessionWidget.h"
#include <Kismet/GameplayStatics.h>
#include <XYZGameInstance.h>


void UHostSessionWidget::CreateSession()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<UXYZGameInstance>());
	UXYZGameInstance* XYZGameInstance = StaticCast<UXYZGameInstance*>(GameInstance);
	XYZGameInstance->LaunchLobby(4, ServerName, bIsLAN);
}
