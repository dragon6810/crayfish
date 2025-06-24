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
    return 0.5f * std::abs
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
    int i, x, y, p;

    int mins[2], maxs[2];
    Eigen::Vector2f screentri[3];
    Eigen::Vector2f v;
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
        screentri[i].x() = ( this->points[i].x() / 2.0 + 0.5) * this->rendertarget->size[0];
        screentri[i].y() = (-this->points[i].y() / 2.0 + 0.5) * this->rendertarget->size[1];

        ws[i] = 1.0 / this->points[i][3];
        depths[i] = this->points[i][2] * ws[i];

        if(screentri[i].x() < mins[0] || !i)
            mins[0] = screentri[i].x();
        if(screentri[i].y() < mins[1] || !i)
            mins[1] = screentri[i].y();
        if(screentri[i].x() > maxs[0] || !i)
            maxs[0] = screentri[i].x();
        if(screentri[i].y() > maxs[1] || !i)
            maxs[1] = screentri[i].y();
    }

    //printf("(%d, %d), (%d %d)\n", mins[0], mins[1], maxs[0], maxs[1]);

    if(mins[0] < 0)
        mins[0] = 0;
    if(mins[1] < 0)
        mins[1] = 0;
    if(maxs[0] > this->rendertarget->size[0] - 1)
        maxs[0] = this->rendertarget->size[0] - 1;
    if(maxs[1] > this->rendertarget->size[1] - 1)
        maxs[1] = this->rendertarget->size[1] - 1;

    for(y=mins[1]; y<=maxs[1]; y++)
    {
        for(x=mins[0]; x<=maxs[0]; x++)
        {
            if(!this->PointInTriangle(Eigen::Vector2f(x, y), screentri))
                continue;

            p = this->rendertarget->size[0] * y + x;

            v = Eigen::Vector2f(x, y);
            barycentric = GetBarycentric(v, screentri);
            depth = barycentric.dot(depths) / barycentric.dot(ws);
            w = 1.0 / barycentric.dot(ws);
            screenpos[2] = depth;
            depth = depth / 2.0 + 0.5;
            screenpos[0] = v[0] / this->rendertarget->size[0] * 2.0 - 1.0;
            screenpos[1] = -(v[1] / this->rendertarget->size[1] * 2.0 - 1.0);
            screenpos[3] = w;

            normal = world = Eigen::Vector3f::Zero();
            for(i=0; i<3; i++)
            {
                normal += this->normals[i] * barycentric[i];
                world += this->world[i] * barycentric[i];
            }

            this->rendertarget->locks[p]->lock();
            if(depth > this->rendertarget->depths[p])
            {
                this->rendertarget->locks[p]->unlock();
                continue;
            }

            this->rendertarget->depths[p] = depth;
            if(frag)
                col = frag->Fragment(screenpos, world, Eigen::Vector3f::Zero(), normal);
            else
                col = Eigen::Vector3f(1.0, 0.0, 1.0);
            this->rendertarget->pixels[p] = 0xFF000000;
            this->rendertarget->pixels[p] |= ((uint32_t) (col[0] * 255.0)) << 16;
            this->rendertarget->pixels[p] |= ((uint32_t) (col[1] * 255.0)) <<  8;
            this->rendertarget->pixels[p] |= ((uint32_t) (col[2] * 255.0)) <<  0;

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