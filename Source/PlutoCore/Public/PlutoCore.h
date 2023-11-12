#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FPlutoCore : public IModuleInterface
{
public:
	static inline FPlutoCore& Get()
	{
		return FModuleManager::LoadModuleChecked<FPlutoCore>("PlutoCore");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("PlutoCore");
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
