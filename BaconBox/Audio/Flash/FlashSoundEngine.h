/**
 * @file
 * @ingroup Audio
 */
#ifndef BB_FLASH_SOUND_ENGINE_H
#define BB_FLASH_SOUND_ENGINE_H

#include <list>

#include "BaconBox/Audio/SoundEngine.h"
#include "BaconBox/Audio/MusicEngine.h"

#include <AS3/AS3.h>
#include <AS3/AS3++.h>

namespace BaconBox {
	class FlashSoundFX;

	class FlashSoundEngine : public SoundEngine {
		friend class AudioEngine;
                friend class BaseEngine;
                friend class ResourceManager;
	public:

		static FlashSoundEngine& getInstance();

		SoundFX* getSoundFX(const std::string& key, bool survive = true);

	private:
		std::list<FlashSoundFX*> audios;


		FlashSoundEngine();

		~FlashSoundEngine();

		void update();

		SoundInfo* loadSoundFromBundle(const std::string& bundleKey);

		SoundInfo* loadSound(const std::string& filePath);

		SoundInfo* loadSound(const SoundParameters& params);

		bool unloadSound(SoundInfo* sound);

		MusicInfo* loadMusic(const std::string& filePath);

		MusicInfo* loadMusic(const MusicParameters& params);
		MusicInfo* loadMusicFromBundle(const std::string& bundleKey);

		bool unloadMusic(MusicInfo* music);
	};
}

#endif
