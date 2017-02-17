#include "Image.h"

#include "raytrace.h"

int main(int, char**){

    /*
    Image image(512, 512);

        for (int i = 0; i < 512; ++ i)
        {
            for (int j = 0; j < 512; ++ j)
            {
                Pixel px;
                //Set the pixel with some colour....
                px.R = i / 2;
                px.G = j / 2;
                px.B = (i + j) / 4;
                px.A = 255;

                image(i, j) = px;
            }
        }

    image.show("GLFW3+Libpng Image Window Demo");
    image.save("demo.png");*/

    Image raytraceImage(1024, 1024);
    RayTraceSphere(&raytraceImage);
    raytraceImage.show("Ray Tracing Sphere Demo");
}
