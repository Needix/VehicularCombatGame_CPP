#include "VideoSettings.h"

bool UVideoSettings::GetSupportedScreenResolutions(TArray<FString> &resolutions) {
	FScreenResolutionArray resolutionsArray;

	if (RHIGetAvailableResolutions(resolutionsArray, true)) {
		for (const FScreenResolutionRHI &resolution : resolutionsArray) {
			if (resolution.Width < MIN_SCREEN_WIDTH || resolution.Height < MIN_SCREEN_HEIGHT) {
				continue;
			}

			FString Str = FString::FromInt(resolution.Width) + "x" + FString::FromInt(resolution.Height);
			resolutions.AddUnique(Str);
		}

		return true;
	}

	return false;
}

FString UVideoSettings::GetScreenResolution() {
	UGameUserSettings *settings = GetGameUserSettings();

	if (!settings) {
		return FString("");
	}

	FIntPoint resolution = settings->GetScreenResolution();
	return FString::FromInt(resolution.X) + "x" + FString::FromInt(resolution.Y);
}

bool UVideoSettings::IsInFullscreen() {
	UGameUserSettings *settings = GetGameUserSettings();

	if (!settings) {
		return false;
	}

	return settings->GetFullscreenMode() == EWindowMode::Fullscreen;
}

bool UVideoSettings::SetScreenResolution(const int32 width, const int32 height, const bool fullscreen) {
	UGameUserSettings *settings = GetGameUserSettings();

	if (!settings) {
		return false;
	}

	settings->SetScreenResolution(FIntPoint(width, height));
	settings->SetFullscreenMode(fullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed);

	return true;
}

bool UVideoSettings::ChangeScreenResolution(const int32 width, const int32 height, const bool fullscreen) {
	UGameUserSettings *settings = GetGameUserSettings();

	if (!settings) {
		return false;
	}

	EWindowMode::Type windowMode = fullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed;
	settings->RequestResolutionChange(width, height, windowMode, false);
    settings->SetScreenResolution(FIntPoint(width, height));
	settings->SetFullscreenMode(windowMode);

	return true;
}

bool UVideoSettings::GetVideoQualitySettings(int32 &antiAliasing, int32 &effects, int32 &postProcess, int32 &resolution, int32 &shadow, int32 &texture, int32 &viewDistance) {
	UGameUserSettings *settings = GetGameUserSettings();

	if (!settings) {
		return false;
	}

	antiAliasing = settings->ScalabilityQuality.AntiAliasingQuality;
	effects = settings->ScalabilityQuality.EffectsQuality;
	postProcess = settings->ScalabilityQuality.PostProcessQuality;
	resolution = settings->ScalabilityQuality.ResolutionQuality;
	shadow = settings->ScalabilityQuality.ShadowQuality;
	texture = settings->ScalabilityQuality.TextureQuality;
	viewDistance = settings->ScalabilityQuality.ViewDistanceQuality;

	return true;
}

bool UVideoSettings::SetVideoQualitySettings(const int32 antiAliasing, const int32 effects, const int32 postProcess, const int32 resolution, const int32 shadow, const int32 texture, const int32 viewDistance) {
	UGameUserSettings *settings = GetGameUserSettings();

	if (!settings) {
		return false;
	}

	settings->ScalabilityQuality.AntiAliasingQuality = antiAliasing;
	settings->ScalabilityQuality.EffectsQuality = effects;
	settings->ScalabilityQuality.PostProcessQuality = postProcess;
	settings->ScalabilityQuality.ResolutionQuality = resolution;
	settings->ScalabilityQuality.ShadowQuality = shadow;
	settings->ScalabilityQuality.TextureQuality = texture;
	settings->ScalabilityQuality.ViewDistanceQuality = viewDistance;

	return true;
}

// Check whether or not we have vertical sync enabled
bool UVideoSettings::IsVSyncEnabled() {
	UGameUserSettings *settings = GetGameUserSettings();

	if (!settings) {
		return false;
	}

	return settings->IsVSyncEnabled();
}

// Set the vertical sync flag
bool UVideoSettings::SetVSyncEnabled(const bool vSync) {
	UGameUserSettings *settings = GetGameUserSettings();

	if (!settings) {
		return false;
	}

	settings->SetVSyncEnabled(vSync);

	return true;
}

// Confirm and save current video mode (resolution and fullscreen/windowed)
bool UVideoSettings::SaveVideoModeAndQuality() {
	UGameUserSettings *settings = GetGameUserSettings();

	if (!settings) {
		return false;
	}

	settings->ConfirmVideoMode();
	settings->ApplySettings(false);

	return true;
}

// Revert to original video settings
bool UVideoSettings::RevertVideoMode() {
	UGameUserSettings *settings = GetGameUserSettings();

	if (!settings) {
		return false;
	}

	settings->RevertVideoMode();
    settings->ApplySettings(false);

	return true;
}

void UVideoSettings::LoadSettings() {
    GetGameUserSettings();
}

UGameUserSettings *UVideoSettings::GetGameUserSettings() {
	if (GEngine != nullptr) {
		return GEngine->GameUserSettings;
	}

	return nullptr;
}