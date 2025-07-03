#include "SceneNodeModel.h"

#include <set>
#include <thread>

#include "Config.h"
#include "Random.h"
#include "Triangle.h"

void Model::DrawModelTri(int indices[3], const SceneNodeCamera* camera, RenderFrame* rendertarget)
{
    int i, j;

    Eigen::Vector4f modelspace, normal, worldspace, viewspace, clipspace;
    Eigen::Vector2f clip2[3];
    Triangle tri;

    tri = Triangle(*rendertarget);
    for(i=0; i<3; i++)
    {
        modelspace.head<3>() = this->points[indices[i]].pos;
        normal.head<3>() = this->points[indices[i]].normal;
        modelspace[3] = 1.0;
        normal[3] = 0.0;
        
        worldspace = this->GetTransform() * modelspace;
        normal = (this->GetTransform() * normal).normalized();

        viewspace = camera->GetViewMatrix() * worldspace;
        
        clipspace = camera->GetProjectionMatrix() * viewspace;
        clipspace.head<3>() /= clipspace[3];
        
        // TODO: Clip triangle to frustum
        if(clipspace[2] > 1 || clipspace[2] < -1)
            return;

        clip2[i] = clipspace.head<2>();

        tri[i] = clipspace;
        tri.texcoords[i] = this->points[indices[i]].tex;
        tri.normals[i] = normal.head<3>();
        tri.world[i] = worldspace.head<3>();
    }

    if(Triangle::TriangleArea(clip2) >= 0) // this culls cw tris, but i feel like it should be culling ccw. whats going on?
        return;

    tri.Draw(this->material->GetFragmentShader());
}

Model::Model(void)
{
    
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

void Model::Render(SceneNodeCamera* camera, RenderFrame* rendertarget)
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