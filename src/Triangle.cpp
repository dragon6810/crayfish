#include "Triangle.h"

Triangle::Triangle(void)
{
    int i;

    for(i=0; i<3; i++)
        this->points[i] = Eigen::Vector3f(0, 0, 0);
}

Triangle::Triangle(RenderFrame& rendertarget)
{
    int i;

    for(i=0; i<3; i++)
        this->points[i] = Eigen::Vector3f(0, 0, 0);
    this->rendertarget = &rendertarget;
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

void Triangle::Draw(void)
{
    int i, x, y, p;

    int mins[2], maxs[2];
    Eigen::Vector2f screentri[3];

    if(!this->rendertarget)
    {
        fprintf(stderr, "warning: attempting to draw triangle without assigned render target!\n");
        return;
    }

    for(i=0; i<3; i++)
    {
        screentri[i].x() = this->points[i].x();
        screentri[i].y() = this->points[i].y();

        if(screentri[i].x() < mins[0] || !i)
            mins[0] = screentri[i].x();
        if(screentri[i].y() < mins[1] || !i)
            mins[1] = screentri[i].y();
        if(screentri[i].x() > maxs[0] || !i)
            maxs[0] = screentri[i].x();
        if(screentri[i].y() > maxs[1] || !i)
            maxs[1] = screentri[i].y();
    }

    printf("(%d, %d), (%d %d)\n", mins[0], mins[1], maxs[0], maxs[1]);

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
            this->rendertarget->pixels[p] = this->color;
        }
    }
}

Eigen::Vector3f& Triangle::operator[](int i)
{
    assert(i >= 0);
    assert(i < 3);
    return this->points[i];
}