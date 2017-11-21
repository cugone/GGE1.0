#pragma once

#include <vector>

class Texture2D;
class SimpleRenderer;

class CubeMap {
public:
    CubeMap(const Texture2D* texture);
	CubeMap(const std::vector<Texture2D*>& textures);
    ~CubeMap();

    void Render(SimpleRenderer& renderer) const;
protected:
private:
	
};