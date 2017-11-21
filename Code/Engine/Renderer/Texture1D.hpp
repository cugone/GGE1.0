#pragma once

#include "Engine/Renderer/TextureBase.hpp"

class Texture1D : public TextureBase {
public:
    Texture1D() noexcept;
    Texture1D(const Texture1D& other) noexcept;
    Texture1D(Texture1D&& r_other) noexcept;
    Texture1D& operator=(const Texture1D& rhs) noexcept;
    Texture1D& operator=(Texture1D&& r_rhs) noexcept;
    virtual ~Texture1D() override;

    Texture1D(RHIDevice* device, ID3D11Texture1D* texture) noexcept;

    void SetDeviceAndTexture(RHIDevice* device, ID3D11Texture1D* texture) noexcept;

    virtual bool IsValid() const noexcept override;
    void SetDebugName(char const *name) noexcept;

    ID3D11Texture1D* GetDxResource() const noexcept;
private:
    ID3D11Texture1D* _dx_resource = nullptr;
};
