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

void Triangle::SetRenderTarget(RenderFrame& rendertarget)
{
    this->rendertarget = &rendertarget;
}

void Triangle::Draw(void)
{
    int i, x, y, p;

    if(!this->rendertarget)
    {
        fprintf(stderr, "warning: attempting to draw triangle without assigned render target!\n");
        return;
    }

    for(i=0; i<3; i++)
    {
        x = this->points[i].x();
        y = this->points[i].y();

        printf("(%d, %d)\n", x, y);

        if(x >= this->rendertarget->size[0] || x < 0)
            continue;
        if(y >= this->rendertarget->size[1] || y < 0)
            continue;

        p = this->rendertarget->size[0] * y + x;
        this->rendertarget->pixels[p] = 0xFFFFFFFF;
    }
}

Eigen::Vector3f& Triangle::operator[](int i)
{
    assert(i > 0);
    assert(i < 3);
    return this->points[i];
}