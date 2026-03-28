#pragma once
#include <string>
#include <vector>

enum class EditorPropertyType
{
    INT,
    FLOAT,
    FLOAT2,
    FLOAT3
};

struct EditorProperty {
    std::string label{};
    EditorPropertyType type{};
    void* data_ptr{};
    float min{};
    float max{};
};

struct EditorImage
{
    std::string label{};
	void* srv_ptr{};
	float aspect_ratio{ 1.0f };
};

struct EditorItem
{
    std::string label{};
    float time{ -1.0f };
	std::vector<EditorImage> images;
    std::vector<EditorProperty> properties;
};
