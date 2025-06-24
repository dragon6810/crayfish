#pragma once

#include <Eigen/Dense>

class Vertex
{
public:
    Eigen::Vector3f pos;
    Eigen::Vector3f normal;
    Eigen::Vector2f tex;
    std::vector<uint32_t> faces;
};