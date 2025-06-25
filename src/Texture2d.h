#pragma once

#include <vector>

#include <Eigen/Dense>

class Texture2d
{
private:
    std::vector<Eigen::Vector4f> data = {};
    Eigen::Vector2i size = Eigen::Vector2i(0, 0);
public:
    Texture2d(void);
    Texture2d(const char* path);

    Eigen::Vector2i GetSize(void);

    void LoadPNG(const char* path);
    Eigen::Vector4f Lookup(Eigen::Vector2f texcoord);
};