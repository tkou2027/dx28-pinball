#include "camera_title.h"
#include "component/render/component_camera.h"

void CameraTitle::Initialize()
{
	auto& comp_camera = m_components.Add<ComponentCamera>(m_comp_id_camera);
	comp_camera.InitializeCamera({}, {}); //  TODO
	m_transform.SetPosition({ 0.0f, 0.0f, 0.0f });
}
