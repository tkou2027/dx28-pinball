#pragma once
#include <string>

enum class BgmName
{
	BGM,
	MAX
};

enum class SoundEffectName
{
	CLEAR,
	HURT,
	MAX
};

class AudioManager
{
public:
	void Initialize();
	int LoadBgm(BgmName name);
	int LoadSoundEffect(SoundEffectName name);
	void Finalize();
private:
	int m_bgm_ids[static_cast<size_t>(BgmName::MAX)];
	int m_se_ids[static_cast<size_t>(SoundEffectName::MAX)];
	std::string m_bgm_paths[static_cast<size_t>(BgmName::MAX)];
	std::string m_se_paths[static_cast<size_t>(SoundEffectName::MAX)];
};