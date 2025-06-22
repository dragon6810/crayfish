#pragma once

#include <Eigen/Dense>

#include "Camera.h"
#include "RenderFrame.h"
#include "Vertex.h"

class Model
{
private:
    Eigen::Vector3f position = Eigen::Vector3f(0, 0, 0);
    Eigen::Quaternionf rotation = Eigen::Quaternionf::Identity();
    Eigen::Vector3f scale = Eigen::Vector3f(1, 1, 1);

    Eigen::Matrix4f transform;

    void ComputeTransform(void);
    void DrawModelTri(int p1, int p2, int p3, const Camera* camera, RenderFrame* rendertarget, uint32_t color=0xFFFFFFFF);
public:
    std::vector<Vertex> points;
    std::vector<uint64_t> indices;

    Model(void);

    static Model PrimitiveCube(void);
    static Model LoadOBJ(const char* path);

    Eigen::Vector3f GetPosition(void);
    Eigen::Quaternionf GetRotation(void);
    Eigen::Vector3f GetScale(void);

    void SetPosition(Eigen::Vector3f position);
    void SetRotation(Eigen::Quaternionf rotation);
    void SetScale(Eigen::Vector3f scale);

    void Render(const Camera* camera, RenderFrame* rendertarget);
};