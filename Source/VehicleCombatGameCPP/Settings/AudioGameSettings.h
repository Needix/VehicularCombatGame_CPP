#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AudioGameSettings.generated.h"

UCLASS()
class VEHICLECOMBATGAMECPP_API UAudioGameSettings : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

  public:
	UFUNCTION(BlueprintCallable, Category = "Audio Settings")
	static void GetVolume(float &master, float &effects, float &music);

	UFUNCTION(BlueprintCallable, Category = "Audio Settings")
	static void SetVolume(float master, float effects, float music);
};