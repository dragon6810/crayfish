#include <stdio.h>

#include "CameraOrtho.h"
#include "CameraPerspective.h"
#include "Config.h"
#include "RenderFrame.h"
#include "Scene.h"
#include "Triangle.h"

int main(int argc, char** argv)
{
    int i;

    int nthreads;

    RenderFrame frame(1024, 1024, true);
    CameraPerspective camera;
    Scene scene;

    for(i=1; i<argc; i++)
    {
        if(!strcmp(argv[i], "-nthreads"))
        {
            if(i == argc - 1)
            {
                fprintf(stderr, "error: expected argument after \"-nthreads\"\n");
                continue;
            }

            nthreads = atoi(argv[++i]);
            if(!nthreads)
            {
                fprintf(stderr, "error: expected non-zero thread count after \"-nthreads\"\n");
                continue;
            }

            Config::nthreads = nthreads;
        }
    }

    scene.SetRenderTarget(frame);
    scene.models.push_back(Model::LoadOBJ("teapot.obj"));
    scene.models[0].SetScale(Eigen::Vector3f(0.75, 0.75, 0.75));
    scene.models[0].SetPosition(Eigen::Vector3f(1.0, -2.0, 2.0));
    scene.models[0].SetRotation(Eigen::Quaternionf(Eigen::AngleAxisf(-M_PI / 4.0, Eigen::Vector3f::UnitY())));
    scene.models.push_back(Model::LoadOBJ("bunny.obj"));
    scene.models[1].SetScale(Eigen::Vector3f(32.0, 32.0, 32.0));
    scene.models[1].SetPosition(Eigen::Vector3f(-0.75, -3.0, 0.0));
    //camera.SetDimensions(Eigen::Vector2f(8, 8));
    camera.SetPosition(Eigen::Vector3f(0, 0, 8));

    scene.Render((Camera*)&camera);
    
    frame.WritePng("frame.png");
}