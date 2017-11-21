#include "Engine/Renderer/CubeMap.hpp"

#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/SimpleRenderer.hpp"

CubeMap::CubeMap(const Texture2D* /*texture*/)
{
    /* DO NOTHING */
}

CubeMap::CubeMap(const std::vector<Texture2D*>& /*textures*/)
{
    /* DO NOTHING */
}

CubeMap::~CubeMap() {
    /* DO NOTHING */
}

void CubeMap::Render(SimpleRenderer& /*renderer*/) const {
    //renderer.RenderCubeMap(*this);
}