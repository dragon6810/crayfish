#include <stdio.h>

#include "RenderFrame.h"
#include "Triangle.h"

int main(int argc, char** argv)
{
    RenderFrame frame(64, 64, true);
    Triangle tri(frame);

    tri[1][0] = 63;
    tri[2][0] = 63;
    tri[2][1] = 63;
    tri.Draw();
    
    frame.WritePng("frame.png");
    printf("hello dave\n");
}