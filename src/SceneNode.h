#pragma once

#include <stdlib.h>

#include <Eigen/Dense>

#include "RenderFrame.h"

class Scene;
class SceneNodeCamera;

class SceneNode
{
private:
    Scene *scene = NULL;

    Eigen::Vector3f position = Eigen::Vector3f::Zero();
    Eigen::Quaternionf rotation = Eigen::Quaternionf::Identity();
    Eigen::Vector3f scale = Eigen::Vector3f::Ones();

    Eigen::Matrix4f transform = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f invtransform = Eigen::Matrix4f::Identity();

    void BuildTransform(void);
public:
    SceneNode(void);
    SceneNode(Scene *scene);

    int parent = -1; // -1 means no parent

    Scene* GetScene(void) const;
    Eigen::Vector3f GetPosition(void) const;
    Eigen::Quaternionf GetRotation(void) const;
    Eigen::Vector3f GetScale(void) const;
    Eigen::Matrix4f GetTransform(void) const;
    Eigen::Matrix4f GetInvTransform(void) const;

    void SetScene(Scene *scene);
    void SetPosition(Eigen::Vector3f position);
    void SetRotation(Eigen::Quaternionf rotation);
    void SetScale(Eigen::Vector3f scale);

    virtual void Render(SceneNodeCamera *camera, RenderFrame* rendertarget);
};