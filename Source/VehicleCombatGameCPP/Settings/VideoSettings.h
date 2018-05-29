#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine.h"
#include "VideoSettings.generated.h"

#define MIN_SCREEN_WIDTH 1024
#define MIN_SCREEN_HEIGHT 768

UCLASS()
class VEHICLECOMBATGAMECPP_API UVideoSettings : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

  public:
	// Get a list of screen resolutions supported on this machine
	UFUNCTION(BlueprintPure, Category = "Video Settings")
	static bool GetSupportedScreenResolutions(TArray<FString> &resolutions);

	// Get currently set screen resolution
	UFUNCTION(BlueprintPure, Category = "Video Settings")
	static FString GetScreenResolution();

	// Check whether or not we are currently running in fullscreen mode
	UFUNCTION(BlueprintPure, Category = "Video Settings")
	static bool IsInFullscreen();

	// Set the desired screen resolution (does not change it yet)
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool SetScreenResolution(const int32 width, const int32 height, const bool fullscreen);

	// Change the current screen resolution
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool ChangeScreenResolution(const int32 width, const int32 height, const bool fullscreen);

	// Get the current video quality settings
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool GetVideoQualitySettings(int32 &antiAliasing, int32 &effects, int32 &postProcess, int32 &resolution, int32 &shadow, int32 &texture, int32 &viewDistance);

	// Set the quality settings (not applied nor saved yet)
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool SetVideoQualitySettings(const int32 antiAliasing = 3, const int32 effects = 3, const int32 postProcess = 3, const int32 resolution = 100, const int32 shadow = 3, const int32 texture = 3, const int32 viewDistance = 3);

	// Check whether or not we have vertical sync enabled
	UFUNCTION(BlueprintPure, Category = "Video Settings")
	static bool IsVSyncEnabled();

	// Set the vertical sync flag
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool SetVSyncEnabled(const bool vSync);

	// Confirm and save current video mode (resolution and fullscreen/windowed) as well as quality settings
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool SaveVideoModeAndQuality();

	// Revert to original video settings
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
	static bool RevertVideoMode();

	static void LoadSettings();

  private:
	// Try to get the GameUserSettings object from the engine
	static UGameUserSettings *GetGameUserSettings();
};