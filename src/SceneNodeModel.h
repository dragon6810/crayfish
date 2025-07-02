#pragma once

#include "SceneNode.h"

#include <memory>

#include "SceneNodeCamera.h"
#include "Material.h"
#include "Vertex.h"

class Model : public SceneNode
{
private:
    void DrawModelTri(int indices[3], const SceneNodeCamera* camera, RenderFrame* rendertarget);
public:
    std::vector<Vertex> points;
    std::vector<uint64_t> indices;
    
    std::shared_ptr<Material> material = std::make_shared<Material>();

    Model(void);

    static Model LoadOBJ(const char* path);

    void Render(SceneNodeCamera* camera, RenderFrame* rendertarget) override;
};