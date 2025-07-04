#include "RenderFrame.h"

#include <assert.h>
#include <math.h>

#include <png.h>

RenderFrame::RenderFrame(uint64_t width, uint64_t height, bool clear)
{
    int i;

    this->size[0] = width;
    this->size[1] = height;

    this->pixels.resize(width * height);
    this->depths.resize(width * height);
    this->locks.resize(width * height);
    if(clear)
    {
        memset(this->pixels.data(), 0, width * height * sizeof(this->pixels[0]));
        for(i=0; i<width*height; i++)
        {
            this->pixels[i] = Eigen::Vector4f::Zero();
            this->depths[i] = 1.0;
            locks[i] = std::make_unique<std::mutex>();
        }
    }
}

void RenderFrame::WritePng(const char* name)
{
    int i, j;

    png_structp pngptr;
    png_infop infoptr;
    png_bytepp rowptrs;
    FILE *ptr;
    std::vector<uint32_t> intpixels;
    Eigen::Vector4f curp;
    
    intpixels.resize(this->size[0] * this->size[1]);
    for(i=0; i<this->size[0] * this->size[1]; i++)
    {
        curp = this->pixels[i];
        for(j=0; j<4; j++)
        {
            if(curp[j] < 0)
                curp[j] = 0;
            if(curp[j] > 1)
                curp[j] = 1;
        }

        intpixels[i]  = 0;
        intpixels[i] |= ((uint32_t) (curp[0] * 255.0)) << 24;
        intpixels[i] |= ((uint32_t) (curp[3] * 255.0)) << 16;
        intpixels[i] |= ((uint32_t) (curp[2] * 255.0)) <<  8;
        intpixels[i] |= ((uint32_t) (curp[1] * 255.0)) <<  0;
    }

    pngptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(pngptr);

    infoptr = png_create_info_struct(pngptr);
    assert(infoptr);

    ptr = fopen(name, "wb");
    assert(ptr);

    png_init_io(pngptr, ptr);

    png_set_IHDR(pngptr, infoptr, this->size[0], this->size[1], 8, PNG_COLOR_TYPE_RGBA,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(pngptr, infoptr);

    rowptrs = (png_bytepp) malloc(sizeof(png_bytep) * this->size[1]);
    for (i=0; i<this->size[1]; i++) 
    {
        rowptrs[i] = (png_bytep) malloc(sizeof(png_byte) * this->size[0] * 4);
        memcpy(rowptrs[i], intpixels.data() + i * this->size[0], sizeof(intpixels[0]) * this->size[0]);
    }

    png_write_image(pngptr, rowptrs);
    png_write_end(pngptr, infoptr);

    for(i=0; i<this->size[1]; i++)
        free(rowptrs[i]);

    free(rowptrs);
    fclose(ptr);
    png_destroy_write_struct(&pngptr, &infoptr);
}