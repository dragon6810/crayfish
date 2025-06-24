#include "Model.h"

#include <set>
#include <thread>

#include "Config.h"
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

void Model::DrawModelTri(int indices[3], const Camera* camera, RenderFrame* rendertarget)
{
    int i, j;

    Eigen::Vector4f mdlpoints[3], normals[3], worldpoints[3], viewpoints[3], screenpoints[3];
    Eigen::Vector2f screen2[3];
    Triangle tri(*rendertarget);

    for(i=0; i<3; i++)
    {
        mdlpoints[i].head<3>() = this->points[indices[i]].pos;
        normals[i].head<3>()   = this->points[indices[i]].normal;
        mdlpoints[i][3] = 1.0;
        normals[i][3] = 0.0;
        
        worldpoints[i] = this->transform * mdlpoints[i];
        normals[i] = (this->transform * normals[i]).normalized();

        viewpoints[i] = camera->GetViewMatrix() * worldpoints[i];
        
        screenpoints[i] = camera->GetProjectionMatrix() * viewpoints[i];
        screenpoints[i].head<3>() /= screenpoints[i][3];
        
        if(screenpoints[i][2] > 1 || screenpoints[i][2] < -1)
            return;

        screen2[i][0] = screenpoints[i][0];
        screen2[i][1] = screenpoints[i][1];

        tri[i] = screenpoints[i];
        for(j=0; j<3; j++)
        {
            tri.normals[i][j] = normals[i][j];
            tri.world[i][j] = worldpoints[i][j];
        }
    }

    if(Triangle::TriangleArea(screen2) > 0)
        return;

    tri.Draw(this->material->GetFragmentShader());
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
    struct verthash 
    {
        std::size_t operator()(const std::tuple<int64_t, int64_t, int64_t>& t) const noexcept 
        {
            std::size_t h1 = std::hash<int64_t>{}(std::get<0>(t));
            std::size_t h2 = std::hash<int64_t>{}(std::get<1>(t));
            std::size_t h3 = std::hash<int64_t>{}(std::get<2>(t));

            std::size_t combined = h1;
            combined ^= h2 + 0x9e3779b9 + (combined << 6) + (combined >> 2);
            combined ^= h3 + 0x9e3779b9 + (combined << 6) + (combined >> 2);
            return combined;
        }
    };

    int iv, i, j;

    char mode;
    Vertex v;
    Model m;
    std::tuple<int64_t, int64_t, int64_t> f[3];
    Eigen::Vector3f pos, n, a, b, fn;
    Eigen::Vector2f texcoord;
    std::vector<Eigen::Vector3f> positions;
    std::vector<Eigen::Vector3f> normals;
    std::vector<Eigen::Vector2f> texcoords;
    std::unordered_map<std::tuple<int64_t, int64_t, int64_t>, int64_t, verthash> uniqueverts;
    uint64_t tri[3];

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
            mode = fgetc(ptr);
            switch(mode)
            {
            case 't':
                if(fscanf(ptr, " %f %f", &texcoord[0], &texcoord[1]) != 2)
                {
                    fprintf(stderr, "error in obj \"%s\": expected texcoord definition.\n", path);
                    break;
                }
                texcoords.push_back(texcoord);
                
                break;
            case 'n':
                if(fscanf(ptr, " %f %f %f", &n[0], &n[1], &n[2]) != 3)
                {
                    fprintf(stderr, "error in obj \"%s\": expected normal definition.\n", path);
                    break;
                }
                normals.push_back(n);
                
                break;
            default:
                fseek(ptr, -1, SEEK_CUR);
                
                if(fscanf(ptr, " %f %f %f", &pos[0], &pos[1], &pos[2]) != 3)
                {
                    fprintf(stderr, "error in obj \"%s\": expected vertex definition.\n", path);
                    break;
                }
                positions.push_back(pos);
                
                break;
            }
            break;
        case 'f':
            for(i=0; i<3; i++)
            {
                std::get<0>(f[i]) = 0;
                std::get<1>(f[i]) = 0;
                std::get<2>(f[i]) = 0;
            }

            if
            (
                fscanf
                (
                    ptr, " %llu %llu %llu", 
                    &std::get<0>(f[0]), 
                    &std::get<0>(f[1]), 
                    &std::get<0>(f[2])
                ) == 3 ||
                fscanf
                (
                    ptr, " /%llu %llu/%llu %llu/%llu", 
                                        &std::get<1>(f[0]), 
                    &std::get<0>(f[1]), &std::get<1>(f[1]), 
                    &std::get<0>(f[2]), &std::get<1>(f[2])
                ) == 5 ||
                fscanf
                (
                    ptr, " /%llu %llu/%llu/%llu %llu/%llu/%llu", 
                                                            &std::get<2>(f[0]), 
                    &std::get<0>(f[1]), &std::get<1>(f[1]), &std::get<2>(f[1]),
                    &std::get<0>(f[2]), &std::get<1>(f[2]), &std::get<2>(f[2])
                ) == 7
            )
            {
                for(i=0; i<3; i++)
                {
                    std::get<0>(f[i])--;
                    std::get<1>(f[i])--;
                    std::get<2>(f[i])--;
                    
                    if(uniqueverts.find(f[i]) != uniqueverts.end())
                    {
                        tri[i] = uniqueverts.at(f[i]);
                        continue;
                    }

                    // need to make this vert
                    v = Vertex();
                    if(std::get<0>(f[i]) >= 0)
                    {
                        if(std::get<0>(f[i]) >= positions.size())
                            fprintf(stderr, "error: vertex position index out of bounds in obj file \"%s\"\n", path);
                        else
                            v.pos = positions[std::get<0>(f[i])];
                    }
                    if(std::get<1>(f[i]) >= 0)
                    {
                        if(std::get<1>(f[i]) >= texcoords.size())
                            fprintf(stderr, "error: vertex texcoord index out of bounds in obj file \"%s\"\n", path);
                        else
                            v.tex = texcoords[std::get<1>(f[i])];
                    }
                    if(std::get<2>(f[i]) >= 0)
                    {
                        if(std::get<2>(f[i]) >= normals.size())
                            fprintf(stderr, "error: vertex normal index out of bounds in obj file \"%s\"\n", path);
                        else
                            v.normal = normals[std::get<2>(f[i])];
                    }

                    tri[i] = m.points.size();
                    uniqueverts.emplace(f[i], tri[i]);
                    m.points.push_back(v);
                }
                m.points[tri[2]].faces.push_back(m.indices.size() / 3);
                m.points[tri[1]].faces.push_back(m.indices.size() / 3);
                m.points[tri[0]].faces.push_back(m.indices.size() / 3);
                m.indices.push_back(tri[2]);
                m.indices.push_back(tri[1]);
                m.indices.push_back(tri[0]);
                
                break;
            }

            fprintf(stderr, "error in obj \"%s\": expected face definition.\n", path);
            break;
        default:
            fprintf(stderr, "error in obj \"%s\": expected vertex or face definition.\n", path);
            while(mode != '\n' && mode != -1)
                mode = fgetc(ptr);
        }
    }

    fclose(ptr);

    for(iv=0; iv<m.points.size(); iv++)
    {
        if(m.points[iv].normal != Eigen::Vector3f::Zero())
            continue;

        n = Eigen::Vector3f::Zero();
        for(i=0; i<m.points[iv].faces.size(); i++)
        {
            for(j=0; j<3; j++)
                tri[j] = m.indices[m.points[iv].faces[i] * 3 + j];
            a = (m.points[tri[1]].pos - m.points[tri[0]].pos).normalized();
            b = (m.points[tri[2]].pos - m.points[tri[0]].pos).normalized();
            fn = a.cross(b);
            n += fn;
        }
        n.normalize();
        m.points[iv].normal = -n; // I need to flip this. I don't know why, but I don't like it.
    }

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
    threadtris = (ntris + Config::nthreads - 1) / Config::nthreads;

    for(t=0; t<Config::nthreads; t++)
    {
        start = t * threadtris;
        end = start + threadtris;
        if(end > ntris)
            end = ntris;
        
        if(start >= end)
            break;

        threads.emplace_back([this, camera, rendertarget, start, end]()
        {
            int i, j;

            int v[3];

            for(i=start; i<end; i++)
            {
                for(j=0; j<3; j++)
                    v[j] = this->indices[i * 3 + j];
                this->DrawModelTri(v, camera, rendertarget);
            }
        });
    }

    for(t=0; t<threads.size(); t++)
        threads[t].join();
}