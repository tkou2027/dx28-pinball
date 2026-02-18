#include "view_context.h"

void RenderTarget::AdjustViewPort(uint32_t window_width, uint32_t window_height)
{
	// TODO: resize backbuffer
	float window_aspect = window_width / static_cast<float>(window_height);
	float texture_aspect = m_width / static_cast<float>(m_height);
	if (window_aspect < texture_aspect - Math::EPSILON)
	{
		m_view_port.TopLeftX = 0.0f;
		m_view_port.Width = static_cast<float>(m_width);
		// stretch
		float valid_height = m_height * window_aspect / texture_aspect;
		m_view_port.TopLeftY = (m_height - valid_height) * 0.5f;
		m_view_port.Height = valid_height;
	}
	else if (window_aspect > texture_aspect + Math::EPSILON)
	{
		m_view_port.TopLeftY = 0.0f;
		m_view_port.Height = static_cast<float>(m_height);
		// stretch
		float valid_width = m_width * texture_aspect / window_aspect;
		m_view_port.TopLeftX = (m_width - valid_width) * 0.5f;
		m_view_port.Width = valid_width;
	}
	else
	{
		SetViewPortDefault();
	}
}