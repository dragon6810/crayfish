#include "Texture2d.h"

#include <png.h>

Texture2d::Texture2d(void)
{

}

Texture2d::Texture2d(const char* path)
{
    this->LoadPNG(path);
}

Eigen::Vector2i Texture2d::GetSize(void)
{
    return this->size;
}

void Texture2d::LoadPNG(const char* path)
{
    FILE* ptr = fopen(path, "rb");
    if (!ptr)
    {
        fprintf(stderr, "error: could not open png file for reading \"%s\"\n", path);
        return;
    }

    png_structp pngptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop pnginfoptr = png_create_info_struct(pngptr);
    if (!pngptr || !pnginfoptr || setjmp(png_jmpbuf(pngptr)))
    {
        fprintf(stderr, "error: libpng failed while loading \"%s\"\n", path);
        png_destroy_read_struct(&pngptr, &pnginfoptr, NULL);
        fclose(ptr);
        return;
    }

    png_init_io(pngptr, ptr);
    png_read_info(pngptr, pnginfoptr);

    int w = png_get_image_width(pngptr, pnginfoptr);
    int h = png_get_image_height(pngptr, pnginfoptr);
    int color_type = png_get_color_type(pngptr, pnginfoptr);
    int bit_depth = png_get_bit_depth(pngptr, pnginfoptr);

    if (bit_depth == 16)
        png_set_strip_16(pngptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(pngptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(pngptr);
    if (png_get_valid(pngptr, pnginfoptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(pngptr);
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY)
        png_set_add_alpha(pngptr, 0xFF, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(pngptr);

    png_read_update_info(pngptr, pnginfoptr);

    int rowbytes = png_get_rowbytes(pngptr, pnginfoptr);
    std::vector<png_byte> image_data(rowbytes * h);
    std::vector<png_bytep> row_pointers(h);
    for (int y = 0; y < h; y++)
        row_pointers[y] = &image_data[y * rowbytes];

    png_read_image(pngptr, row_pointers.data());

    this->data.clear();
    this->data.reserve(w * h);
    this->size[0] = w;
    this->size[1] = h;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            png_bytep px = &row_pointers[y][x * 4];
            this->data.emplace_back(Eigen::Vector4f(
                px[3] / 255.0f, // a
                px[0] / 255.0f, // r
                px[1] / 255.0f, // g
                px[2] / 255.0f  // b
            ));
        }
    }

    png_destroy_read_struct(&pngptr, &pnginfoptr, NULL);
    fclose(ptr);
}

Eigen::Vector4f Texture2d::Lookup(Eigen::Vector2f texcoord)
{
    int i;

    Eigen::Vector2i pixels;

    for(i=0; i<2; i++)
    {
        pixels[i] = texcoord[i] * this->size[i];
        while(pixels[i] < 0)
            pixels[i] += this->size[i];
        pixels[i] = pixels[i] % this->size[i];
    }
    pixels[1] = this->size[1] - pixels[1];

    return this->data[pixels[1] * this->size[0] + pixels[0]];
}