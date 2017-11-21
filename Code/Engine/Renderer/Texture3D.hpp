#pragma once

#include "Engine/Renderer/TextureBase.hpp"

class Texture3D : public TextureBase {
public:
    Texture3D() noexcept;
    Texture3D(const Texture3D& other) noexcept;
    Texture3D(Texture3D&& r_other) noexcept;
    Texture3D& operator=(const Texture3D& rhs) noexcept;
    Texture3D& operator=(Texture3D&& r_rhs) noexcept;
	virtual ~Texture3D() override;

    Texture3D(RHIDevice* device, ID3D11Texture3D* texture) noexcept;

    void SetDeviceAndTexture(RHIDevice* device, ID3D11Texture3D* texture) noexcept;

    virtual bool IsValid() const noexcept override;
    void SetDebugName(char const *name) noexcept;

    ID3D11Texture3D* GetDxResource() const noexcept;
private:
    ID3D11Texture3D* _dx_resource = nullptr;
};
