#include "Triangle.h"

Triangle::Triangle(void)
{
    int i;

    for(i=0; i<3; i++)
        this->points[i] = Eigen::Vector4f(0, 0, 0, 1);
}

Triangle::Triangle(RenderFrame& rendertarget)
{
    int i;

    for(i=0; i<4; i++)
        this->points[i] = Eigen::Vector4f(0, 0, 0, 1);
    this->rendertarget = &rendertarget;
}

float Triangle::TriangleArea(Eigen::Vector2f tri[3])
{
    return 0.5f *
    (
        tri[0][0] * (tri[1][1] - tri[2][1]) +
        tri[1][0] * (tri[2][1] - tri[0][1]) +
        tri[2][0] * (tri[0][1] - tri[1][1])
    );
}

Eigen::Vector3f Triangle::GetBarycentric(Eigen::Vector2f p, Eigen::Vector2f tri[3])
{
    int i;

    float area;
    Eigen::Vector3f barycentric;
    Eigen::Vector2f curtri[3];

    area = TriangleArea(tri);
    for(i=0; i<3; i++)
    {
        curtri[0] = tri[(i + 2) % 3];
        curtri[1] = p;
        curtri[2] = tri[(i + 1) % 3];
        barycentric[i] = TriangleArea(curtri) / area;
    }

    return barycentric;
}

bool Triangle::PointInTriangle(Eigen::Vector2f p, Eigen::Vector2f tri[3])
{
    int i;
    Eigen::Vector2f e[2];
    
    Eigen::Vector2f r, rp;

    for(i=0; i<3; i++)
    {
        e[0] = tri[i];
        e[1] = tri[(i+1)%3];
        r = (e[1] - e[0]).normalized();
        rp = (p - e[0]).normalized();
        if((r.x() * rp.y() - rp.x() * r.y()) < 0)
            return false;
    }

    return true;
}

void Triangle::SetRenderTarget(RenderFrame& rendertarget)
{
    this->rendertarget = &rendertarget;
}

void Triangle::SetColor(uint32_t color)
{
    this->color = color;
}

void Triangle::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    this->color = 0;
    this->color |= ((uint32_t) r) << 24;
    this->color |= ((uint32_t) g) << 16;
    this->color |= ((uint32_t) b) <<  8;
    this->color |= ((uint32_t) a) <<  0;
}

void Triangle::Draw(ShaderFragment* frag)
{
    int i, j, x, y, p;

    int mins[2], maxs[2];
    Eigen::Vector2f pixels[3], v, texcoord;
    Eigen::Vector3f barycentric, depths, ws, col, world, normal;
    Eigen::Vector4f screenpos;
    float depth, w;

    if(!this->rendertarget)
    {
        fprintf(stderr, "warning: attempting to draw triangle without assigned render target!\n");
        return;
    }

    for(i=0; i<3; i++)
    {
        pixels[i][0] = ( this->points[i][0] / 2.0 + 0.5) * this->rendertarget->size[0];
        pixels[i][1] = (-this->points[i][1] / 2.0 + 0.5) * this->rendertarget->size[1];

        ws[i] = this->points[i][3];
        depths[i] = this->points[i][2] / ws[i];

        for(j=0; j<2; j++)
        {
            if(!i || pixels[i][j] < mins[j])
                mins[j] = pixels[i][j];
            if(!i || pixels[i][j] > maxs[j])
                maxs[j] = pixels[i][j];
        }
    }

    for(i=0; i<2; i++)
    {
        if(mins[i] < 0)
            mins[i] = 0;
        if(maxs[i] > this->rendertarget->size[i] - 1)
            maxs[i] = this->rendertarget->size[i] - 1;
    }

    for(y=mins[1]; y<=maxs[1]; y++)
    {
        p = this->rendertarget->size[0] * y + mins[0];
        for(x=mins[0]; x<=maxs[0]; x++, p++)
        {
            v = Eigen::Vector2f(x, y);

            if(!this->PointInTriangle(v, pixels))
                continue;

            barycentric = GetBarycentric(v, pixels);
            
            // interpolation
            w = 1.0 / barycentric.dot(ws.cwiseInverse());
            depth = barycentric.dot(depths) * w;

            // depth-testing and pixel writing
            depth = depth / 2.0 + 0.5;

            this->rendertarget->locks[p]->lock();
            if(depth > this->rendertarget->depths[p])
            {
                this->rendertarget->locks[p]->unlock();
                continue;
            }

            if(frag)
            {
                screenpos[0] =   v[0] / this->rendertarget->size[0] * 2.0 - 1.0;
                screenpos[1] = -(v[1] / this->rendertarget->size[1] * 2.0 - 1.0);
                screenpos[2] = depth;
                screenpos[3] = w;

                texcoord = Eigen::Vector2f::Zero();
                normal = world = Eigen::Vector3f::Zero();
                for(i=0; i<3; i++)
                {
                    normal += this->normals[i] / ws[i] * barycentric[i];
                    world += this->world[i] / ws[i] * barycentric[i];
                    texcoord += this->texcoords[i] / ws[i] * barycentric[i];
                }
                world *= w;
                texcoord *= w;

                col = frag->Fragment(screenpos, world, texcoord, normal);
            }
            else
            {
                col = Eigen::Vector3f(1.0, 0.0, 1.0); // uh-oh, no shader given! return pepto bismol.
            }

            this->rendertarget->depths[p] = depth;
            this->rendertarget->pixels[p] = Eigen::Vector4f(1.0, col[0], col[1], col[2]);

            this->rendertarget->locks[p]->unlock();
        }
    }
}

Eigen::Vector4f& Triangle::operator[](int i)
{
    assert(i >= 0);
    assert(i < 3);
    return this->points[i];
}