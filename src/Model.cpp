#include "Model.h"

#include "Triangle.h"

void Model::ComputeTransform(void)
{
    int i, j;

    Eigen::Matrix3f rotate;

    this->transform = Eigen::Matrix4f::Identity();

    rotate = this->rotation.toRotationMatrix();
    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
            this->transform(i, j) = rotate(i, j) * this->scale[j];
    for(i=0; i<3; i++)
        this->transform(i, 3) = this->position[i];
}

void Model::DrawModelTri(int p1, int p2, int p3, const Camera* camera, RenderFrame* rendertarget, uint32_t color)
{
    int i, j;

    Eigen::Vector4f mdlpoints[3], worldpoints[3], viewpoints[3], screenpoints[3];
    Triangle tri(*rendertarget);

    for(i=0; i<3; i++)
    {
        mdlpoints[0][i] = this->points[p1][i];
        mdlpoints[1][i] = this->points[p2][i];
        mdlpoints[2][i] = this->points[p3][i];
        mdlpoints[i][3] = 1.0;
    }

    for(i=0; i<3; i++)
        worldpoints[i] = this->transform * mdlpoints[i];

    for(i=0; i<3; i++)
        viewpoints[i] = camera->GetViewMatrix() * worldpoints[i];

    for(i=0; i<3; i++)
        screenpoints[i] = camera->GetProjectionMatrix() * viewpoints[i];

    for(i=0; i<3; i++)
        printf("(%f %f %f %f) ", screenpoints[i][0], screenpoints[i][1], screenpoints[i][2], screenpoints[i][3]);
    printf("\n");

    for(i=0; i<3; i++)
    {
        tri[i][0] = (screenpoints[i][0] / 2.0 + 0.5) * rendertarget->size[0];
        tri[i][1] = (screenpoints[i][1] / 2.0 + 0.5) * rendertarget->size[1];
    }
    tri.SetColor(color);
    tri.Draw();
}

Model::Model(void)
{
    this->ComputeTransform();
}

Model Model::PrimitiveCube(void)
{
    static const int faces[6][4] = 
    {
        {0, 1, 5, 4}, // Bottom (-Y)
        {2, 6, 7, 3}, // Top (+Y)
        {0, 2, 6, 4}, // Left (-X)
        {3, 7, 5, 1}, // Right (+X)
        {4, 5, 7, 6}, // Front (+Z)
        {1, 0, 2, 3}, // Back (-Z)
    };

    int i, j;

    Model model;
    int v[4];

    model.points.resize(8);
    for (i=0; i<8; i++)
    {
        model.points[i] = Eigen::Vector3f
        (
            (i & 1 ? 0.5f : -0.5f),
            (i & 2 ? 0.5f : -0.5f),
            (i & 4 ? 0.5f : -0.5f)
        );
    }

    model.indices.reserve(6 * 2 * 3);
    for (i=0; i<6; i++) 
    {
        for(j=0; j<4; j++)
            v[j] = faces[i][j];

        model.indices.push_back(v[0]);
        model.indices.push_back(v[1]);
        model.indices.push_back(v[2]);
        model.colors.push_back(rand() | 0xFF000000);

        model.indices.push_back(v[0]);
        model.indices.push_back(v[2]);
        model.indices.push_back(v[3]);
        model.colors.push_back(rand() | 0xFF000000);
    }

    return model;
}

Eigen::Vector3f Model::GetPosition(void)
{
    return this->position;
}

Eigen::Quaternionf Model::GetRotation(void)
{
    return this->rotation;
}

Eigen::Vector3f Model::GetScale(void)
{
    return this->scale;
}

void Model::SetPosition(Eigen::Vector3f position)
{
    this->position = position;
    this->ComputeTransform();
}

void Model::SetRotation(Eigen::Quaternionf rotation)
{
    this->rotation = rotation;
    this->ComputeTransform();
}

void Model::SetScale(Eigen::Vector3f scale)
{
    this->scale = scale;
    this->ComputeTransform();
}

void Model::Render(const Camera* camera, RenderFrame* rendertarget)
{
    int i;

    assert(camera);
    assert(rendertarget);

    if(this->indices.size() % 3)
    {
        fprintf(stderr, "error: model must be made of tris\n");
        return;
    }

    for(i=0; i<this->indices.size(); i+=3)
        this->DrawModelTri(this->indices[i], this->indices[i+1], this->indices[i+2], camera, rendertarget, this->colors[i/3]);
}