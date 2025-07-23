#pragma once

#include <Windows.h>

//Render °ü·Ã
#include <d3d11.h>                 // Direct3D 11
#include <dxgi1_6.h>                 // DXGI 1.6
#include <d2d1_3.h>                  // Direct2D 1.3
#include <d2d1_3helper.h>           
#include <dwrite_3.h>                // ÅØ½ºÆ® ¿£Áø
#include <wincodec.h>                // WIC
#include <d2d1helper.h>
#include <wrl/client.h>


//STL
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <sstream>

#include <array>


#include <stdexcept>
#include <DirectXMath.h>

namespace DX
{
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr) : result(hr) {}

        const char* what() const noexcept override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X",
                static_cast<unsigned int>(result));
            return s_str;
        }

    private:
        HRESULT result;
    };
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw com_exception(hr);
        }
    }
}