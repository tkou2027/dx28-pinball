#include "uv_animation_state.h"


void UVAnimationState::SetAnimationIndex(int index, const std::vector<UVFrameAnimationDesc>& frame_descs)
{
	animation_index = index;
	// initialize frame
	auto& desc = frame_descs.at(index);
	frame = desc.frame_default;
}

void UVAnimationState::Update(const std::vector<UVFrameAnimationDesc>& frame_descs)
{
	if (frame_descs.empty())
	{
		return;
	}
	if (!playing)
	{
		return;
	}

	auto& desc = frame_descs.at(animation_index);
	play_count++;
	if (play_count == desc.play_speed_scale)
	{
		play_count = 0;
		if (!desc.play_loop && frame + 1 >= desc.frame_total)
		{
			playing = false;
			return;
		}
		frame = (++frame) % desc.frame_total;
	}
}

UVRect UVAnimationState::GetUVRect(const std::vector<UVFrameAnimationDesc>& frame_descs) const
{
	UVRect rect{};
	if (frame_descs.empty())
	{
		rect.uv_size = uv_scroll_size;
		rect.uv_offset = uv_scroll_offset;
	}
	else
	{
		auto& desc = frame_descs.at(animation_index);
		rect.uv_size = desc.uv_size;
		rect.uv_offset = uv_scroll_offset + desc.uv_size * (frame + desc.frame_offset);
	}
	return rect;
}

