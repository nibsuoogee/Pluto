#include "PlutoCore.h"
#include "Modules/ModuleManager.h"

#include "Log.h"

void FPlutoCore::StartupModule()
{
	UE_LOG(LogPlutoCore, Log, TEXT("PlutoCore module starting up"));
}

void FPlutoCore::ShutdownModule()
{
	UE_LOG(LogPlutoCore, Log, TEXT("PlutoCore module shutting down"));
}

IMPLEMENT_PRIMARY_GAME_MODULE(FPlutoCore, PlutoCore, "PlutoCore");
