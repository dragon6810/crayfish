#include "SceneNode.h"

#include <assert.h>

SceneNode::SceneNode(void)
{
    this->scene = NULL;
}

SceneNode::SceneNode(Scene *scene)
{
    this->scene = scene;
}

void SceneNode::BuildTransform(void)
{
    int i, j;

    Eigen::Matrix3f rotate;
    Eigen::Matrix4f rotate4, scale, translate, invrotate4, invscale, invtranslate;

    this->transform = Eigen::Matrix4f::Identity();

    rotate = this->rotation.toRotationMatrix();
    rotate4 = invrotate4 = Eigen::Matrix4f::Identity();
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            rotate4(i, j) = rotate(i, j);
            invrotate4(i, j) = rotate(j, i);
        }
    }
    
    scale = invscale = Eigen::Matrix4f::Identity();
    for(i=0; i<3; i++)
    {
        scale(i, i) = this->scale[i];
        invscale(i, i) = 1.0 / this->scale[i];
    }

    translate = invtranslate = Eigen::Matrix4f::Identity();
    for(i=0; i<3; i++)
    {
        translate(i, 3) = this->position[i];
        invtranslate(i, 3) = -this->position[i];
    }

    this->transform = translate * rotate4 * scale;
    this->invtransform = invscale * invrotate4 * invtranslate;
}

Scene* SceneNode::GetScene(void) const
{
    return this->scene;
}

Eigen::Vector3f SceneNode::GetPosition(void) const
{
    return this->position;
}

Eigen::Quaternionf SceneNode::GetRotation(void) const
{
    return this->rotation;
}

Eigen::Vector3f SceneNode::GetScale(void) const
{
    return this->scale;
}

Eigen::Matrix4f SceneNode::GetTransform(void) const
{
    return this->transform;
}

Eigen::Matrix4f SceneNode::GetInvTransform(void) const
{
    return this->invtransform;
}

void SceneNode::SetScene(Scene *scene)
{
    this->scene = scene;
}

void SceneNode::SetPosition(Eigen::Vector3f position)
{
    this->position = position;
    this->BuildTransform();
}

void SceneNode::SetRotation(Eigen::Quaternionf rotation)
{
    this->rotation = rotation;
    this->BuildTransform();
}

void SceneNode::SetScale(Eigen::Vector3f scale)
{
    this->scale = scale;
    this->BuildTransform();
}

void SceneNode::Render(SceneNodeCamera *camera, RenderFrame* rendertarget)
{
    assert(camera);

    // do nothing by default
}