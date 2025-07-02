#include <stdio.h>

#include "SceneNodeCameraPerspective.h"
#include "Config.h"
#include "MaterialLit.h"
#include "RenderFrame.h"
#include "Scene.h"
#include "Triangle.h"

int main(int argc, char** argv)
{
    int i;

    int nthreads;

    RenderFrame frame(1024, 1024, true);
    SceneNodeCameraPerspective camera;
    Scene scene;
    Model teapot, bunny, cake;
    MaterialLit mat;
    std::shared_ptr<Material> matptr;

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

    mat = MaterialLit();
    mat.Texture().LoadPNG("cake.png");
    mat.Texture().filter = Texture2d::FILTERTYPE_LINEAR;
    matptr = std::make_shared<MaterialLit>(mat);
    assert(matptr->GetFragmentShader());
    scene.materials.push_back(matptr);

    teapot = Model::LoadOBJ("teapot.obj");
    teapot.SetScale(Eigen::Vector3f(0.75, 0.75, 0.75));
    teapot.SetPosition(Eigen::Vector3f(1.0, -2.0, 2.0));
    teapot.SetRotation(Eigen::Quaternionf(Eigen::AngleAxisf(-M_PI / 4.0, Eigen::Vector3f::UnitY())));
    teapot.material = matptr;
    //scene.models.push_back(teapot);

    bunny = Model::LoadOBJ("bunny.obj");
    bunny.SetScale(Eigen::Vector3f(32.0, 32.0, 32.0));
    bunny.SetPosition(Eigen::Vector3f(-0.75, -3.0, 0.0));
    bunny.material = matptr;
    //scene.models.push_back(bunny);
    
    cake = Model::LoadOBJ("cake.obj");
    cake.SetScale(0.15 * Eigen::Vector3f::Ones());
    cake.SetPosition(Eigen::Vector3f(0.0, -4.0, 0.0));
    cake.material = matptr;
    scene.models.push_back(cake);
    
    camera.SetPosition(Eigen::Vector3f(0, 0, 8));

    scene.Render((SceneNodeCamera*)&camera);
    
    frame.WritePng("frame.png");
}