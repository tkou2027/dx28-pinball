#include "model_geometry.h"
#include "math/hex.h"

void Geometry::CreateBox(float width, float height, float depth, MeshGeometry& mesh)
{
	constexpr int box_face_num{ 6 };
	constexpr int box_vertex_num_per_face{ 4 };
	constexpr int box_index_num_per_face{ 6 };
	constexpr int box_vertex_num{ box_face_num * box_vertex_num_per_face };
	constexpr int box_index_num{ box_face_num * box_index_num_per_face };

	float width_half = width / 2.0f;
	float height_half = height / 2.0f;
	float depth_half = depth / 2.0f;

	// create verticies
	mesh.positions.resize(box_vertex_num);
	mesh.normals.resize(box_vertex_num);
	mesh.texcoords.resize(box_vertex_num);

	// position ====
	// -x
	mesh.positions[0] = { -width_half,  height_half,  depth_half };
	mesh.positions[1] = { -width_half,  height_half, -depth_half };
	mesh.positions[2] = { -width_half, -height_half,  depth_half };
	mesh.positions[3] = { -width_half, -height_half, -depth_half };
	// +x
	mesh.positions[4] = { width_half,  height_half, -depth_half };
	mesh.positions[5] = { width_half,  height_half,  depth_half };
	mesh.positions[6] = { width_half, -height_half, -depth_half };
	mesh.positions[7] = { width_half, -height_half,  depth_half };
	// -y
	mesh.positions[8] = { -width_half, -height_half, -depth_half };
	mesh.positions[9] = { width_half, -height_half, -depth_half };
	mesh.positions[10] = { -width_half, -height_half,  depth_half };
	mesh.positions[11] = { width_half, -height_half,  depth_half };
	// +y
	mesh.positions[12] = { -width_half, height_half,  depth_half };
	mesh.positions[13] = { width_half, height_half,  depth_half };
	mesh.positions[14] = { -width_half, height_half, -depth_half };
	mesh.positions[15] = { width_half, height_half, -depth_half };
	// -z
	mesh.positions[16] = { -width_half,  height_half, -depth_half };
	mesh.positions[17] = { width_half,  height_half, -depth_half };
	mesh.positions[18] = { -width_half, -height_half, -depth_half };
	mesh.positions[19] = { width_half, -height_half, -depth_half };
	// +z
	mesh.positions[20] = { width_half,  height_half, depth_half };
	mesh.positions[21] = { -width_half,  height_half, depth_half };
	mesh.positions[22] = { width_half, -height_half, depth_half };
	mesh.positions[23] = { -width_half, -height_half, depth_half };

	// normal ====
	// for each vertex per face
	for (int i = 0; i < box_vertex_num_per_face; i++)
	{
		mesh.normals[i] = { -1.0f, 0.0f, 0.0f };
		mesh.normals[i + box_vertex_num_per_face] = { 1.0f, 0.0f, 0.0f };
		mesh.normals[i + box_vertex_num_per_face * 2] = { 0.0f, -1.0f, 0.0f };
		mesh.normals[i + box_vertex_num_per_face * 3] = { 0.0f, 1.0f, 0.0f };
		mesh.normals[i + box_vertex_num_per_face * 4] = { 0.0f, 0.0f, -1.0f };
		mesh.normals[i + box_vertex_num_per_face * 5] = { 0.0f, 0.0f, 1.0f };
	}

	// texcoords ====
	// for each face
	for (int i = 0; i < box_vertex_num; i += box_vertex_num_per_face)
	{
		mesh.texcoords[i] = { 0.0f, 0.0f };
		mesh.texcoords[i + 1] = { 1.0f, 0.0f };
		mesh.texcoords[i + 2] = { 0.0f, 1.0f };
		mesh.texcoords[i + 3] = { 1.0f, 1.0f };
	}

	// index ====
	mesh.indices.resize(box_index_num);
	for (int i = 0, index_start = 0; i < box_index_num; i += box_index_num_per_face, index_start += box_vertex_num_per_face)
	{
		// top left
		mesh.indices[i] = index_start;
		mesh.indices[i + 1] = index_start + 1;
		mesh.indices[i + 2] = index_start + 2;
		// bottom right
		mesh.indices[i + 3] = index_start + 1;
		mesh.indices[i + 4] = index_start + 3;
		mesh.indices[i + 5] = index_start + 2;
	}
	ComputeTangents(mesh);
}

void Geometry::SetBoxUV(MeshGeometry& mesh, bool top)
{
	constexpr int box_face_num{ 6 };
	constexpr int box_vertex_num_per_face{ 4 };
	constexpr int box_index_num_per_face{ 6 };
	constexpr int box_vertex_num{ box_face_num * box_vertex_num_per_face };
	constexpr int box_index_num{ box_face_num * box_index_num_per_face };
	mesh.texcoords.resize(box_vertex_num);
	// texcoords ====
	Vector2 uv_size{ 1.0f, 0.25f };
	// for each face
	for (int i = 0, face = 0; i < box_vertex_num; i += box_vertex_num_per_face, face++)
	{
		Vector2 uv_offset{};
		if (face == 3) // top
		{
			uv_offset.y = 0.0f;
		}
		else if (face == 2) // bottom
		{
			uv_offset.y = uv_size.y * 3;
		}
		else
		{
			uv_offset.y = uv_size.y * (top ? 1 : 2);
		}
		mesh.texcoords[i] = { uv_offset.x            , uv_offset.y };
		mesh.texcoords[i + 1] = { uv_offset.x + uv_size.x, uv_offset.y };
		mesh.texcoords[i + 2] = { uv_offset.x            , uv_offset.y + uv_size.y };
		mesh.texcoords[i + 3] = { uv_offset.x + uv_size.x, uv_offset.y + uv_size.y };
	}
	ComputeTangents(mesh);
}

void Geometry::CreatePlane(float width, float height, MeshGeometry& mesh)
{
	float width_half = width / 2.0f;
	float depth_half = height / 2.0f;

	mesh.positions.resize(4);
	mesh.normals.resize(4);
	mesh.texcoords.resize(4);
	mesh.indices.resize(6);

	mesh.positions[0] = { -width_half, 0,  depth_half };
	mesh.positions[1] = { width_half, 0,  depth_half };
	mesh.positions[2] = { -width_half, 0, -depth_half };
	mesh.positions[3] = { width_half, 0, -depth_half };

	mesh.texcoords[0] = { 0.0f, 0.0f };
	mesh.texcoords[1] = { 1.0f, 0.0f };
	mesh.texcoords[2] = { 0.0f, 1.0f };
	mesh.texcoords[3] = { 1.0f, 1.0f };

	for (int i = 0; i < 4; i++)
	{
		mesh.normals[i] = { 0.0f, 1.0f, 0.0f };
	}

	// top left
	mesh.indices[0] = 0;
	mesh.indices[1] = 1;
	mesh.indices[2] = 2;
	// bottom right
	mesh.indices[3] = 1;
	mesh.indices[4] = 3;
	mesh.indices[5] = 2;
	ComputeTangents(mesh);
}

void Geometry::CreatePlaneXY(float width, float height, MeshGeometry& mesh)
{
	float width_half = width / 2.0f;
	float height_half = height / 2.0f;

	mesh.positions.resize(4);
	mesh.normals.resize(4);
	mesh.texcoords.resize(4);
	mesh.indices.resize(6);

	mesh.positions[0] = { -width_half, height_half, 0.0f };
	mesh.positions[1] = { width_half, height_half, 0.0f };
	mesh.positions[2] = { -width_half, -height_half, 0.0f };
	mesh.positions[3] = { width_half, -height_half, 0.0f };

	mesh.texcoords[0] = { 0.0f, 0.0f };
	mesh.texcoords[1] = { 1.0f, 0.0f };
	mesh.texcoords[2] = { 0.0f, 1.0f };
	mesh.texcoords[3] = { 1.0f, 1.0f };

	for (int i = 0; i < 4; i++)
	{
		mesh.normals[i] = { 0.0f, 0.0f, 1.0f };
	}

	// top left
	mesh.indices[0] = 0;
	mesh.indices[1] = 1;
	mesh.indices[2] = 2;
	// bottom right
	mesh.indices[3] = 1;
	mesh.indices[4] = 3;
	mesh.indices[5] = 2;
	ComputeTangents(mesh);
}

void Geometry::CreateCylinderSide(
	float radius, float height, int num_slice, int num_stack, bool reverse_normal, MeshGeometry& mesh)
{
	assert(num_slice >= 3 && num_stack >= 1);
	const int verts_per_slice = (num_stack + 1);
	const int vertex_num = (num_slice + 1) * verts_per_slice; // duplicate seam
	const int index_num = num_slice * num_stack * 6;

	// ensure we won't overflow uint16_t
	assert(index_num <= 0xFFFF);

	mesh.positions.clear();
	mesh.normals.clear();
	mesh.texcoords.clear();
	mesh.indices.clear();

	mesh.positions.resize(vertex_num);
	mesh.normals.resize(vertex_num);
	mesh.texcoords.resize(vertex_num);
	mesh.indices.resize(index_num);

	// build vertices
	for (int slice = 0; slice <= num_slice; ++slice)
	{
		float u = static_cast<float>(slice) / static_cast<float>(num_slice); // 0..1
		float angle = u * Math::TWO_PI;
		float cx = sinf(angle);
		float cz = cosf(angle);
		int base = slice * verts_per_slice;

		// choose normal direction based on reverse_normal
		Vector3 faceNormal = reverse_normal ? Vector3{ -cx, 0.0f, -cz } : Vector3{ cx, 0.0f, cz };

		for (int stack = 0; stack <= num_stack; ++stack)
		{
			float v = static_cast<float>(stack) / static_cast<float>(num_stack); // 0..1
			float y = -height * 0.5f + v * height;
			mesh.positions[base + stack] = { radius * cx, y, radius * cz };
			mesh.normals[base + stack] = faceNormal;
			// Optionally flip U when reversing to keep texture orientation consistent:
			mesh.texcoords[base + stack] = reverse_normal ? Vector2{ 1.0f - u, v } : Vector2{ u, v };
		}
	}

	// build indices (same winding as CreateBox/Plane when not reversed)
	int idx = 0;
	for (int slice = 0; slice < num_slice; ++slice)
	{
		int base = slice * verts_per_slice;
		int base_next = (slice + 1) * verts_per_slice;
		for (int stack = 0; stack < num_stack; ++stack)
		{
			// map to face ordering: index_start, index_start+1, index_start+2, index_start+3
			uint16_t i0 = static_cast<uint16_t>(base + stack);
			uint16_t i1 = static_cast<uint16_t>(base_next + stack);
			uint16_t i2 = static_cast<uint16_t>(base + stack + 1);
			uint16_t i3 = static_cast<uint16_t>(base_next + stack + 1);

			if (!reverse_normal)
			{
				// original winding
				mesh.indices[idx++] = i0;
				mesh.indices[idx++] = i1;
				mesh.indices[idx++] = i2;

				mesh.indices[idx++] = i1;
				mesh.indices[idx++] = i3;
				mesh.indices[idx++] = i2;
			}
			else
			{
				// reversed winding (flip triangle order) so front faces face inward
				mesh.indices[idx++] = i2;
				mesh.indices[idx++] = i1;
				mesh.indices[idx++] = i0;

				mesh.indices[idx++] = i2;
				mesh.indices[idx++] = i3;
				mesh.indices[idx++] = i1;
			}
		}
	}
	ComputeTangents(mesh);
}

void Geometry::CreateCylinder(float radius, float height, int num_slice, int num_stack, MeshGeometry& mesh)
{
	assert(num_slice >= 3 && num_stack >= 1);

	// Build side first (copy of CreateCylinderSide logic)
	const int verts_per_slice = (num_stack + 1);
	const int side_vertex_num = (num_slice + 1) * verts_per_slice; // duplicate seam
	const int side_index_num = num_slice * num_stack * 6;

	// clear mesh
	mesh.positions.clear();
	mesh.normals.clear();
	mesh.texcoords.clear();
	mesh.indices.clear();

	// Reserve approximate sizes to avoid excessive reallocations
	mesh.positions.reserve(side_vertex_num + (num_slice + 1) * 2 + 2);
	mesh.normals.reserve(side_vertex_num + (num_slice + 1) * 2 + 2);
	mesh.texcoords.reserve(side_vertex_num + (num_slice + 1) * 2 + 2);
	mesh.indices.reserve(side_index_num + num_slice * 6 * 2); // side + top + bottom

	// --- side vertices ---
	for (int slice = 0; slice <= num_slice; ++slice)
	{
		float u = static_cast<float>(slice) / static_cast<float>(num_slice); // 0..1
		float angle = u * Math::TWO_PI;
		float cx = sinf(angle);
		float cz = cosf(angle);
		int base = slice * verts_per_slice;
		for (int stack = 0; stack <= num_stack; ++stack)
		{
			float v = static_cast<float>(stack) / static_cast<float>(num_stack); // 0..1
			float y = -height * 0.5f + v * height;
			mesh.positions.push_back({ radius * cx, y, radius * cz });
			mesh.normals.push_back({ cx, 0.0f, cz });
			mesh.texcoords.push_back({ u, v });
		}
	}

	// --- side indices ---
	for (int slice = 0; slice < num_slice; ++slice)
	{
		int base = slice * verts_per_slice;
		int base_next = (slice + 1) * verts_per_slice;
		for (int stack = 0; stack < num_stack; ++stack)
		{
			uint16_t i0 = static_cast<uint16_t>(base + stack);
			uint16_t i1 = static_cast<uint16_t>(base_next + stack);
			uint16_t i2 = static_cast<uint16_t>(base + stack + 1);
			uint16_t i3 = static_cast<uint16_t>(base_next + stack + 1);

			mesh.indices.push_back(i0);
			mesh.indices.push_back(i1);
			mesh.indices.push_back(i2);

			mesh.indices.push_back(i1);
			mesh.indices.push_back(i3);
			mesh.indices.push_back(i2);
		}
	}

	// record current vertex count as base for caps
	uint16_t sideVertexCount = static_cast<uint16_t>(mesh.positions.size());

	// --- top cap ---
	// center vertex
	mesh.positions.push_back({ 0.0f, height * 0.5f, 0.0f });
	mesh.normals.push_back({ 0.0f, 1.0f, 0.0f });
	mesh.texcoords.push_back({ 0.5f, 0.5f });
	uint16_t topCenterIndex = sideVertexCount;
	// rim vertices (duplicate seam to match side sampling)
	for (int slice = 0; slice <= num_slice; ++slice)
	{
		float u = static_cast<float>(slice) / static_cast<float>(num_slice);
		float angle = u * Math::TWO_PI;
		float cx = sinf(angle);
		float cz = cosf(angle);
		mesh.positions.push_back({ radius * cx, height * 0.5f, radius * cz });
		mesh.normals.push_back({ 0.0f, 1.0f, 0.0f });
		// map circle to [0,1] UV
		mesh.texcoords.push_back({ 0.5f + cx * 0.5f, 0.5f - cz * 0.5f });
	}
	uint16_t topRimStart = topCenterIndex + 1;
	// indices for top (center, i, i+1) with consistent winding
	for (int i = 0; i < num_slice; ++i)
	{
		uint16_t a = static_cast<uint16_t>(topRimStart + i);
		uint16_t b = static_cast<uint16_t>(topRimStart + i + 1);
		mesh.indices.push_back(topCenterIndex);
		mesh.indices.push_back(a);
		mesh.indices.push_back(b);
	}

	// --- bottom cap ---
	uint16_t bottomCenterIndex = static_cast<uint16_t>(mesh.positions.size());
	mesh.positions.push_back({ 0.0f, -height * 0.5f, 0.0f });
	mesh.normals.push_back({ 0.0f, -1.0f, 0.0f });
	mesh.texcoords.push_back({ 0.5f, 0.5f });
	// rim vertices
	for (int slice = 0; slice <= num_slice; ++slice)
	{
		float u = static_cast<float>(slice) / static_cast<float>(num_slice);
		float angle = u * Math::TWO_PI;
		float cx = sinf(angle);
		float cz = cosf(angle);
		mesh.positions.push_back({ radius * cx, -height * 0.5f, radius * cz });
		mesh.normals.push_back({ 0.0f, -1.0f, 0.0f });
		mesh.texcoords.push_back({ 0.5f + cx * 0.5f, 0.5f + cz * 0.5f }); // flip v for bottom
	}
	uint16_t bottomRimStart = bottomCenterIndex + 1;
	// indices for bottom (center, i+1, i) to flip winding (pointing down)
	for (int i = 0; i < num_slice; ++i)
	{
		uint16_t a = static_cast<uint16_t>(bottomRimStart + i);
		uint16_t b = static_cast<uint16_t>(bottomRimStart + i + 1);
		// order reversed so normal faces down
		mesh.indices.push_back(bottomCenterIndex);
		mesh.indices.push_back(b);
		mesh.indices.push_back(a);
	}
	ComputeTangents(mesh);
}

void Geometry::CreateSquareRing(float radius, float ring_width, float ring_bevel, MeshGeometry& mesh)
{
	mesh.positions.clear();
	mesh.normals.clear();
	mesh.texcoords.clear();
	mesh.indices.clear();

	const float width_half = ring_width * 0.5f;
	const float b = Math::Min(ring_bevel, width_half);

	Vector3 dirs[4] =
	{
		{ 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ -1.0f, 0.0f, 0.0f }
	};
	Vector3 path[4] =
	{
		dirs[0] * radius,
		dirs[1] * radius,
		dirs[2] * radius,
		dirs[3] * radius
	};

	// beveled square section
	Vector2 section[8] =
	{
		{ -width_half + b,  width_half },
		{  width_half - b,  width_half },
		{  width_half,  width_half - b },
		{  width_half, -width_half + b },
		{  width_half - b, -width_half },
		{ -width_half + b, -width_half },
		{ -width_half, -width_half + b },
		{ -width_half,  width_half - b }
	};

	const int path_segments = 4;
	const int sweep_segments = 8;
	for (int i = 0; i < path_segments; ++i)
	{
		const int i_next = (i + 1) % path_segments;
		Vector3 center_0 = path[i];
		Vector3 center_1 = path[(i + 1) % path_segments];

		const Vector3 up = { 0, 1, 0 };
		for (int j = 0; j < sweep_segments; ++j)
		{
			const int j_next = (j + 1) % sweep_segments;
			Vector3 v0 = center_0 - dirs[i] * section[j].x + up * section[j].y;
			Vector3 v1 = center_1 - dirs[i_next] * section[j].x + up * section[j].y;
			Vector3 v2 = center_0 - dirs[i] * section[j_next].x + up * section[j_next].y;
			Vector3 v3 = center_1 - dirs[i_next] * section[j_next].x + up * section[j_next].y;
			uint32_t base = (uint32_t)mesh.positions.size();

			mesh.positions.push_back(v0);
			mesh.positions.push_back(v1);
			mesh.positions.push_back(v2);
			mesh.positions.push_back(v3);

			Vector3 normal = (v2 - v0).CrossProduct(v1 - v0);
			normal.Normalize();
			for (int k = 0; k < 4; ++k)
			{
				mesh.normals.push_back(normal);
			}

			mesh.texcoords.push_back({ 0, 0 });
			mesh.texcoords.push_back({ 1, 0 });
			mesh.texcoords.push_back({ 0, 1 });
			mesh.texcoords.push_back({ 1, 1 });

			// two triangles
			mesh.indices.push_back(base + 0);
			mesh.indices.push_back(base + 1);
			mesh.indices.push_back(base + 2);

			mesh.indices.push_back(base + 1);
			mesh.indices.push_back(base + 3);
			mesh.indices.push_back(base + 2);
		}

	}

	ComputeTangents(mesh);
}

void Geometry::CreateSquarePillar(float height, float ring_width, float ring_bevel, MeshGeometry& mesh)
{
	mesh.positions.clear();
	mesh.normals.clear();
	mesh.texcoords.clear();
	mesh.indices.clear();

	const float width_half = ring_width * 0.5f;
	const float b = Math::Min(ring_bevel, width_half);
	const float h_half = height * 0.5f;

	Vector3 path[2] = {
		{ 0.0f, -h_half, 0.0f }, // Bottom
		{ 0.0f,  h_half, 0.0f }  // Top
	};

	Vector2 section[8] = {
		{ -width_half + b,  width_half },
		{  width_half - b,  width_half },
		{  width_half,      width_half - b },
		{  width_half,     -width_half + b },
		{  width_half - b, -width_half },
		{ -width_half + b, -width_half },
		{ -width_half,     -width_half + b },
		{ -width_half,      width_half - b }
	};

	const int sweep_segments = 8;
	for (int j = 0; j < sweep_segments; ++j)
	{
		const int j_next = (j + 1) % sweep_segments;
		const Vector3 v0{ section[j].x,      path[0].y, section[j].y };
		const Vector3 v1{ section[j_next].x, path[0].y, section[j_next].y };
		const Vector3 v2{ section[j].x,      path[1].y, section[j].y };
		const Vector3 v3{ section[j_next].x, path[1].y, section[j_next].y };

		uint32_t base = (uint32_t)mesh.positions.size();

		mesh.positions.push_back(v0);
		mesh.positions.push_back(v1);
		mesh.positions.push_back(v2);
		mesh.positions.push_back(v3);

		Vector3 normal = (v1 - v0).CrossProduct(v2 - v0);
		normal.Normalize();
		for (int k = 0; k < 4; ++k) mesh.normals.push_back(normal);

		mesh.texcoords.push_back({ (float)j / sweep_segments, 0 });
		mesh.texcoords.push_back({ (float)j / sweep_segments, 1 });
		mesh.texcoords.push_back({ (float)(j + 1) / sweep_segments, 0 });
		mesh.texcoords.push_back({ (float)(j + 1) / sweep_segments, 1 });

		mesh.indices.push_back(base + 0);
		mesh.indices.push_back(base + 1);
		mesh.indices.push_back(base + 2);
		mesh.indices.push_back(base + 1);
		mesh.indices.push_back(base + 3);
		mesh.indices.push_back(base + 2);
	}

	ComputeTangents(mesh);
}
void Geometry::CreateHexTileMap(const HexTileMap& tile_map, MeshGeometry& mesh)
{
	const auto& positions = tile_map.GetPositions();
	const auto& indices = tile_map.GetIndices();

	mesh.positions = positions;
	mesh.indices = indices;

	const size_t vertex_num = positions.size();
	// textcoords
	mesh.texcoords.clear();
	mesh.texcoords.reserve(vertex_num);
	const std::array<Vector2, 3> LOCAL_TRIANGLE_TEXCOORDS
	{
		Vector2{ 0.5f, std::sqrt(3.0f) * 0.5f },
		Vector2{ 0.0f, 0.0f },
		Vector2{ 1.0f, 0.0f }
	};
	for (int i = 0; i < vertex_num; i++)
	{
		mesh.texcoords.push_back(LOCAL_TRIANGLE_TEXCOORDS[i % 3]);
	}

	// normal
	// TODO: tangent

	mesh.normals.clear();
	mesh.normals.reserve(vertex_num);
	for (int i = 0; i < vertex_num; i++)
	{
		mesh.normals.push_back({ 0.0f, 1.0f, 0.0f });
	}

	mesh.colors = tile_map.GetColors();
	ComputeTangents(mesh);
}

// compute tangent with help of ai and learnopengl
void Geometry::ComputeTangents(MeshGeometry& mesh)
{
	size_t vertex_num = mesh.positions.size();
	size_t index_num = mesh.indices.size();

	mesh.tangents.resize(vertex_num);
	std::vector<Vector3> tangents_tmp(vertex_num, { 0, 0, 0 });
	std::vector<Vector3> bitangents_tmp(vertex_num, { 0, 0, 0 });

	// tangent per triangle
	for (size_t i = 0; i < index_num; i += 3) {
		const auto index_0 = mesh.indices[i];
		const auto index_1 = mesh.indices[i + 1];
		const auto index_2 = mesh.indices[i + 2];

		const Vector3& v_0 = mesh.positions[index_0];
		const Vector3& v_1 = mesh.positions[index_1];
		const Vector3& v_2 = mesh.positions[index_2];

		const Vector2& uv_0 = mesh.texcoords[index_0];
		const Vector2& uv_1 = mesh.texcoords[index_1];
		const Vector2& uv_2 = mesh.texcoords[index_2];

		const Vector3 edge_1 = v_1 - v_0;
		const Vector3 edge_2 = v_2 - v_0;
		const Vector2 delta_uv1 = uv_1 - uv_0;
		const Vector2 delta_uv2 = uv_2 - uv_0;
		// e1 = du1 * T + dv1 * B
		// e2 = du2 * T + dv2 * B
		// matrix form:
		// [ e1x e1y e1z ] = [ du1 dv1 ] [ Tx Ty Tz ]
		// [ e2x e2y e2z ] = [ du2 dv2 ] [ Bx By Bz ]
		// M = [ du1 dv1 ]
		//     [ du2 dv2 ]
		// M^-1 = 1/det * [ dv2 -dv1 ]
		// 			      [ -du2 du1 ]
		// det = du1 * dv2 - du2 * dv1
		float det = delta_uv1.x * delta_uv2.y - delta_uv2.x * delta_uv1.y;
		if (Math::IsZero(det))
		{
			continue;
		}
		float det_inv = 1.0f / det;
		const Vector3 tangent = (edge_1 * delta_uv2.y + edge_2 * -delta_uv1.y) * det_inv;
		const Vector3 bitangent = (edge_1 * -delta_uv2.x + edge_2 * delta_uv1.x ) * det_inv;

		// accumulate to each vertex
		tangents_tmp[index_0] += tangent;
		tangents_tmp[index_1] += tangent;
		tangents_tmp[index_2] += tangent;
		bitangents_tmp[index_0] += bitangent;
		bitangents_tmp[index_1] += bitangent;
		bitangents_tmp[index_2] += bitangent;
	}
	for (size_t i = 0; i < vertex_num; ++i)
	{
		const Vector3& normal = mesh.normals[i];
		const Vector3& tangent_tmp = tangents_tmp[i];
		const Vector3& bitangent_tmp = bitangents_tmp[i];

		// orthonormalize
		const float n_dot_t = Vector3::Dot(normal, tangent_tmp);
		const Vector3 tangent_xyz = (tangent_tmp - normal * n_dot_t).GetNormalized();

		// handedness
		const Vector3 n_cross_t = normal.CrossProduct(tangent_xyz);
		const float cross_dot_b = Vector3::Dot(n_cross_t, bitangent_tmp);
		const float w = cross_dot_b < 0.0f ? -1.0f : 1.0f;

		mesh.tangents[i] = Vector4{ tangent_xyz.x, tangent_xyz.y, tangent_xyz.z, w };
	}
}
