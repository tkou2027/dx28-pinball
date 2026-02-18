#pragma once

#define NOMINMAX // Windows.hのmin/maxマクロ回避　Windows.hの直前にいれて
#include <Windows.h> // Windows API
#include <d3d11.h>   // DirectX (Driect3D11)
#include <wrl/client.h> // comptr

#define SAFE_RELEASE(o) if(o) { (o)->Release(); o = nullptr; }