#include <stdio.h>

#include "RenderFrame.h"

int main(int argc, char** argv)
{
    RenderFrame frame(64, 64, true);
    frame.WritePng("frame.png");
    printf("hello dave\n");
}