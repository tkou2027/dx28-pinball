#pragma once
#include <vector>
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"

struct MeshGeometry
{
    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
	std::vector<Vector4> tangents; // w for handedness
	std::vector<Vector4> colors; // and other additional info
    std::vector<uint32_t> indices;
};

class Geometry
{
public:
    static void CreateBox(float width, float height, float depth, MeshGeometry& mesh);
    static void SetBoxUV(MeshGeometry& mesh, bool top);
    static void CreatePlane(float width, float height, MeshGeometry& mesh);
    static void CreatePlaneXY(float width, float height, MeshGeometry& mesh);
    static void CreateCylinderSide(
        float radius, float height, int num_slice, int num_stack, bool reverse_normal, MeshGeometry& mesh);
    static void CreateCylinder(float radius, float height, int num_slice, int num_stack, MeshGeometry& mesh);
    static void CreateSquareRing(float radius, float ring_width, float ring_bevel, MeshGeometry& mesh);
    static void CreateSquarePillar(float height, float ring_width, float ring_bevel, MeshGeometry& mesh);

    static void CreateHexTileMap(const class HexTileMap& tile_map, MeshGeometry& mesh);
    // static void CreateSphere(float radius = 1.0f, uint32_t levels = 20, uint32_t slices = 20, MeshGeometry& mesh);

    // generate tangents
    static void ComputeTangents(MeshGeometry& mesh);
};