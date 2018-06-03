#include "AudioGameSettings.h"

void UAudioGameSettings::GetVolume(float &master, float &effects, float &music) {
	GConfig->GetFloat(TEXT("AudioSettings"), TEXT("MasterVolume"), master, GGameIni);
	GConfig->GetFloat(TEXT("AudioSettings"), TEXT("EffectsVolume"), effects, GGameIni);
	GConfig->GetFloat(TEXT("AudioSettings"), TEXT("MusicVolume"), music, GGameIni);
}

void UAudioGameSettings::SetVolume(float master, float effects, float music) {
	GConfig->SetFloat(TEXT("AudioSettings"), TEXT("MasterVolume"), master, GGameIni);
	GConfig->SetFloat(TEXT("AudioSettings"), TEXT("EffectsVolume"), effects, GGameIni);
	GConfig->SetFloat(TEXT("AudioSettings"), TEXT("MusicVolume"), music, GGameIni);

	GConfig->Flush(false, GGameIni);
}