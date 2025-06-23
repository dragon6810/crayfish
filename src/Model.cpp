#include "Model.h"

#include <thread>

#include "Random.h"
#include "Triangle.h"

void Model::ComputeTransform(void)
{
    int i, j;

    Eigen::Matrix3f rotate;
    Eigen::Matrix4f rotate4, scale, translate;

    this->transform = Eigen::Matrix4f::Identity();

    rotate = this->rotation.toRotationMatrix();
    rotate4 = Eigen::Matrix4f::Identity();
    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
            rotate4(i, j) = rotate(i, j);
    
    scale = Eigen::Matrix4f::Identity();
    for(i=0; i<3; i++)
        scale(i, i) = this->scale[i];

    translate = Eigen::Matrix4f::Identity();
    for(i=0; i<3; i++)
        translate(i, 3) = this->position[i];

    this->transform = translate * rotate4 * scale;
}

void Model::DrawModelTri(int p1, int p2, int p3, const Camera* camera, RenderFrame* rendertarget, uint32_t color)
{
    int i, j;

    float light;
    Eigen::Vector3f vcolor;
    Eigen::Vector3f a, b, normal;
    Eigen::Vector4f mdlpoints[3], worldpoints[3], viewpoints[3], screenpoints[3];
    Triangle tri(*rendertarget);

    for(i=0; i<3; i++)
    {
        mdlpoints[0][i] = this->points[p1].pos[i];
        mdlpoints[1][i] = this->points[p2].pos[i];
        mdlpoints[2][i] = this->points[p3].pos[i];
        mdlpoints[i][3] = 1.0;
    }

    for(i=0; i<3; i++)
        worldpoints[i] = this->transform * mdlpoints[i];

    for(i=0; i<3; i++)
        viewpoints[i] = camera->GetViewMatrix() * worldpoints[i];

    for(i=0; i<3; i++)
    {
        screenpoints[i] = camera->GetProjectionMatrix() * viewpoints[i];
        screenpoints[i][0] /= screenpoints[i][3];
        screenpoints[i][1] /= screenpoints[i][3];
        screenpoints[i][2] /= screenpoints[i][3];
        if(screenpoints[i][2] > 1 || screenpoints[i][2] < -1)
            return;
    }

    for(i=0; i<3; i++)
    {
        a[i] = worldpoints[1][i] - worldpoints[0][i];
        b[i] = worldpoints[2][i] - worldpoints[0][i];
    }
    normal = b.cross(a).normalized();
    light = normal.dot(Eigen::Vector3f(0, 1, 0)) / 2.0 + 0.5;
    if(light > 1)
        light = 1;
    vcolor[0] = (float) ((color & 0x00FF0000) >> 16) / 255.0;
    vcolor[1] = (float) ((color & 0x0000FF00) >>  8) / 255.0;
    vcolor[2] = (float) ((color & 0x000000FF) >>  0) / 255.0;
    vcolor *= light;
    #if 0
    for(i=0; i<3; i++)
    {
        vcolor[i] = normal[i];
        if(vcolor[i] < 0)
            vcolor[i] = 0;
    }
    #endif
    color = 0xFF000000;
    color |= ((uint32_t) (vcolor[0] * 255.0)) << 16;
    color |= ((uint32_t) (vcolor[1] * 255.0)) <<  8;
    color |= ((uint32_t) (vcolor[2] * 255.0)) <<  0;

    for(i=0; i<3; i++)
        tri[i] = screenpoints[i];
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
        {0, 4, 6, 2}, // Left (-X)
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
        model.points[i].pos = Eigen::Vector3f
        (
            (i & 1 ? 0.5f : -0.5f),
            (i & 2 ? 0.5f : -0.5f),
            (i & 4 ? 0.5f : -0.5f)
        );
        //printf("%d: (%f %f %f)\n", i, model.points[i].pos[0], model.points[i].pos[1], model.points[i].pos[2]);
    }

    model.indices.reserve(6 * 2 * 3);
    for (i=0; i<6; i++) 
    {
        for(j=0; j<4; j++)
            v[j] = faces[i][j];

        model.indices.push_back(v[0]);
        model.indices.push_back(v[1]);
        model.indices.push_back(v[2]);

        model.indices.push_back(v[0]);
        model.indices.push_back(v[2]);
        model.indices.push_back(v[3]);
    }

    return model;
}

Model Model::LoadOBJ(const char* path)
{
    char mode;
    Vertex v;
    Model m;
    uint64_t f[3];

    FILE *ptr;

    ptr = fopen(path, "r");
    if(!ptr)
    {
        fprintf(stderr, "error: can't open file \"%s\"\n", path);
        return Model();
    }

    m = Model();
    while(mode = fgetc(ptr))
    {
        while(mode <= 32 && mode != -1)
            mode = fgetc(ptr);
        
        if(mode == -1)
            break;
        
        if(mode == '#')
        {
            while(mode != '\n' && mode != -1)
                mode = fgetc(ptr);
            continue;
        }

        switch(mode)
        {
        case 'v':
            v = Vertex();
            if(fscanf(ptr, " %f %f %f", &v.pos[0], &v.pos[1], &v.pos[2]) != 3)
            {
                fprintf(stderr, "error in obj \"%s\": expected vertex definition.\n", path);
                break;
            }
            m.points.push_back(v);
            break;
        case 'f':
            if(fscanf(ptr, " %llu %llu %llu", &f[0], &f[1], &f[2]) != 3)
            {
                fprintf(stderr, "error in obj \"%s\": expected face definition.\n", path);
                break;
            }
            m.indices.push_back(f[2] - 1);
            m.indices.push_back(f[1] - 1);
            m.indices.push_back(f[0] - 1);
            break;
        default:
            fprintf(stderr, "error in obj \"%s\": expected vertex or face definition.\n", path);
            //printf("mode is %c\n", mode);
            while(mode != '\n' && mode != -1)
                mode = fgetc(ptr);
        }
    }

    fclose(ptr);
    return m;
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

const int nthreads = 8;

void Model::Render(const Camera* camera, RenderFrame* rendertarget)
{
    int i, t;

    uint64_t ntris, threadtris;
    std::vector<std::thread> threads;
    uint64_t start, end;

    assert(camera);
    assert(rendertarget);

    if(this->indices.size() % 3)
    {
        fprintf(stderr, "error: model must be made of tris\n");
        return;
    }

    ntris = this->indices.size() / 3;
    threadtris = (ntris + nthreads - 1) / nthreads;

    for(t=0; t<nthreads; t++)
    {
        start = t * threadtris;
        end = start + threadtris;
        if(end > ntris)
            end = ntris;
        
        if(start >= end)
            break;

        threads.emplace_back([this, camera, rendertarget, start, end, t]()
        {
            int i, j;

            int v[3];

            printf("thread %llu rendering tris %llu-%llu\n", t, start, end);

            for(i=start; i<end; i++)
            {
                for(j=0; j<3; j++)
                    v[j] = this->indices[i * 3 + j];
                this->DrawModelTri(v[0], v[1], v[2], camera, rendertarget, Random::UInt(i) | 0xFF000000);
            }

            printf("thread %llu done\n", t);
        });
    }

    for(t=0; t<nthreads; t++)
        threads[t].join();

    printf("model done\n");
}