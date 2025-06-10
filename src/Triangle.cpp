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

void Triangle::Draw(void)
{
    int i, x, y, p;

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
    }

    for(y=0; y<this->rendertarget->size[1]; y++)
    {
        for(x=0; x<this->rendertarget->size[0]; x++)
        {
            if(!this->PointInTriangle(Eigen::Vector2f(x, y), screentri))
                continue;

            p = this->rendertarget->size[0] * y + x;
            this->rendertarget->pixels[p] = 0xFFFFFFFF;
        }
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
        this->rendertarget->pixels[p] = 0xFF0000FF;
    }
}

Eigen::Vector3f& Triangle::operator[](int i)
{
    assert(i > 0);
    assert(i < 3);
    return this->points[i];
}