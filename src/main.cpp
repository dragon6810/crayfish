#include <stdio.h>

#include "RenderFrame.h"
#include "Triangle.h"

int main(int argc, char** argv)
{
    RenderFrame frame(64, 64, true);
    Triangle tri(frame);

    tri[1][0] = 64;
    tri[2][0] = 64;
    tri[2][1] = 64;
    tri.SetColor(0xFF00FFFF);
    tri.Draw();
    
    frame.WritePng("frame.png");
    printf("hello dave\n");
}