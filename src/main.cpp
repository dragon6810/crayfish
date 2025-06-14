#include <stdio.h>

#include "CameraOrtho.h"
#include "RenderFrame.h"
#include "Scene.h"
#include "Triangle.h"

int main(int argc, char** argv)
{
    RenderFrame frame(64, 64, true);
    CameraOrtho camera;
    Scene scene;

    scene.SetRenderTarget(frame);
    scene.models.push_back(Model::PrimitiveCube());
    scene.models[0].SetRotation(Eigen::Quaternionf(Eigen::AngleAxisf(M_PI * 0.25, Eigen::Vector3f::UnitY())) * Eigen::Quaternionf(Eigen::AngleAxisf(M_PI * 0.15, Eigen::Vector3f::UnitX())));
    scene.models[0].SetScale(Eigen::Vector3f(4.0, 4.0, 4.0));
    camera.SetDimensions(Eigen::Vector2f(8, 8));
    camera.SetPosition(Eigen::Vector3f(0, 0, -10));

    scene.Render((Camera*)&camera);
    
    frame.WritePng("frame.png");
}