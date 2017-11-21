#pragma once

#include "Engine/Renderer/TextureBase.hpp"

class Texture2D : public TextureBase {
public:
    Texture2D() noexcept;
    Texture2D(const Texture2D& other) noexcept;
    Texture2D(Texture2D&& r_other) noexcept;
    Texture2D& operator=(const Texture2D& rhs) noexcept;
    Texture2D& operator=(Texture2D&& r_rhs) noexcept;
	virtual ~Texture2D() override;

    Texture2D(RHIDevice* device, ID3D11Texture2D* texture) noexcept;

    void SetDeviceAndTexture(RHIDevice* device, ID3D11Texture2D* texture) noexcept;

    virtual bool IsValid() const noexcept override;
    void SetDebugName(char const *name) noexcept;

    ID3D11Texture2D* GetDxResource() const noexcept;
private:
    ID3D11Texture2D* _dx_resource = nullptr;
};
