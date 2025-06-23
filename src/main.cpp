#include <stdio.h>

#include "CameraOrtho.h"
#include "CameraPerspective.h"
#include "RenderFrame.h"
#include "Scene.h"
#include "Triangle.h"

int main(int argc, char** argv)
{
    RenderFrame frame(256, 256, true);
    CameraPerspective camera;
    Scene scene;

    scene.SetRenderTarget(frame);
    scene.models.push_back(Model::LoadOBJ("teapot.obj"));
    scene.models[0].SetScale(Eigen::Vector3f(1.0, 1.0, 1.0));
    scene.models[0].SetPosition(Eigen::Vector3f(0.0, -1.0, 0.0));
    scene.models[0].SetRotation(Eigen::Quaternionf(Eigen::AngleAxisf(-M_PI / 4.0, Eigen::Vector3f::UnitY())));
    //camera.SetDimensions(Eigen::Vector2f(8, 8));
    camera.SetPosition(Eigen::Vector3f(0, 0, 8));

    scene.Render((Camera*)&camera);
    
    frame.WritePng("frame.png");
}