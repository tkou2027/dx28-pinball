#include "audio_manager.h"
#include "global_context.h"
#include "platform/sound.h"

void AudioManager::Initialize()
{
	// bgm
	m_bgm_paths[static_cast<size_t>(BgmName::BGM)] = "asset/sound/bgm_bleeping_demo.wav";
	for (auto& id : m_bgm_ids)
	{
		id = -1;
	}
	// se
	m_se_paths[static_cast<size_t>(SoundEffectName::CLEAR)] = "asset/sound/se_clear_maou_se_system10.wav";
	m_se_paths[static_cast<size_t>(SoundEffectName::HURT)] = "asset/sound/se_hurt_maou_se_system08.wav";
	for (auto& id : m_se_ids)
	{
		id = -1;
	}
}

void AudioManager::Finalize()
{
	// nothing to do here
}

int AudioManager::LoadBgm(BgmName name)
{
	int index = static_cast<size_t>(name);
	if (m_bgm_ids[index] != -1)
	{
		return m_bgm_ids[index];
	}
	m_bgm_ids[index] = g_global_context.m_sound->LoadSound(m_bgm_paths[index].c_str());
	return m_bgm_ids[index];
}

int AudioManager::LoadSoundEffect(SoundEffectName name)
{
	int index = static_cast<size_t>(name);
	if (m_se_ids[index] != -1)
	{
		return m_se_ids[index];
	}
	m_se_ids[index] = g_global_context.m_sound->LoadSound(m_se_paths[index].c_str());
	return m_se_ids[index];
} 