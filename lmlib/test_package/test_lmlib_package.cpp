#include <iostream>

#include <lmlib/geometry.h>

int main()
{
    lm::Frustum frustum{1.5f, 16.f / 9.f, 0.1f, 1000.f};
    std::cout << frustum.perspective_transform().row(0) << std::endl;
}
