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

struct EditorItem
{
    std::string label;
    std::vector<EditorProperty> properties;
};