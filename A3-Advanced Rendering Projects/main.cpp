#include "Image.h"
#include "raytrace.h"

int main(int, char**){

    Image image(800, 800);
    RayTrace(&image);
    image.show(" CSC 305 RayTracer Demo ");
    image.save("D:\\Zihan\\courses\\CSC305\\");
}
