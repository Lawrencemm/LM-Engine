#pragma once

#include <array>

#include <Eigen/Eigen>

namespace lm
{
template <typename scalar_type> struct size2
{
    using Vector = Eigen::Matrix<scalar_type, 2, 1>;
    scalar_type width, height;

    size2() noexcept = default;
    size2(scalar_type width, scalar_type height) noexcept
        : width{width}, height{height}
    {
    }
    size2(std::array<scalar_type, 2> arr) : width{arr[0]}, height{arr[1]} {}
    size2(const size2 &other) = default;
    operator Vector() const { return {width, height}; }

    struct Ratio
    {
        scalar_type num, den;
    };
    size2 proportion(Ratio ratio_x, Ratio ratio_y)
    {
        return size2{(width * ratio_x.num) / ratio_x.den,
                     (height * ratio_y.num) / ratio_y.den};
    }

    size2 proportion(Ratio ratio) { return proportion(ratio, ratio); }

    size2 operator-(const size2 &rhs) const
    {
        return {width - rhs.width, height - rhs.height};
    }

    operator std::array<scalar_type, 2>() { return {width, height}; }
};

using size2i = size2<int>;
using size2u = size2<unsigned>;
using size2f = size2<float>;

template <typename scalar_type> struct point2
{
    using Vector = Eigen::Matrix<scalar_type, 2, 1>;
    scalar_type x, y;

    point2() : x{0}, y{0} {};
    point2(scalar_type x, scalar_type y) : x{x}, y{y} {}
    point2(std::array<scalar_type, 2> arr) : x{arr[0]}, y{arr[1]} {}

    operator std::array<scalar_type, 2>() const { return {x, y}; }
    operator Vector() const { return {x, y}; }
    bool operator==(const point2 &rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    inline size2<scalar_type> operator-(const point2 &rhs) const
    {
        return {x - rhs.x, y - rhs.y};
    }
    point2 operator+(const Vector &rhs) const
    {
        return {x + rhs[0], y + rhs[1]};
    }
};

template <typename scalar_type>
std::ostream &operator<<(std::ostream &stream, point2<scalar_type> size2d)
{
    stream << "(" << size2d.x << ", " << size2d.y << ")";
    return stream;
}

using point2i = point2<int>;
using point2l = point2<long>;
using point2f = point2<float>;

template <typename scalar_type> struct size3
{
    scalar_type width, height, depth;

    size3() = default;
    size3(const size2<scalar_type> &size2, scalar_type depth)
        : width{size2.width}, height{size2.height}, depth{depth}
    {
    }
};

using size3u = size3<unsigned>;

template <typename scalar_type> struct point3
{
    using vector_type = Eigen::Matrix<scalar_type, 3, 1>;

    vector_type vector() { return {x, y, z}; }

    point3 &operator+=(const vector_type &vector)
    {
        x += vector[0];
        y += vector[1];
        z += vector[2];
        return *this;
    }

    scalar_type x, y, z;
};

using point3f = point3<float>;

template <typename scalar_type> struct rect2
{
    using Vector = Eigen::Matrix<scalar_type, 2, 1>;

    scalar_type left, right, top, bottom;

    explicit rect2(const std::pair<const Vector &, const Vector &> &points)
        : left{std::min(points.first[0], points.second[0])},
          right{std::max(points.first[0], points.second[0])},
          top{std::max(points.first[1], points.second[1])},
          bottom{std::min(points.first[1], points.second[1])}
    {
    }

    bool encloses(const Vector &point)
    {
        return point[0] > left && point[0] < right && point[1] < top &&
               point[1] > bottom;
    }
};

using rect2f = rect2<float>;

struct frustum
{
  public:
    frustum() {}
    frustum(float fov, float aspect, float near, float farClip);

    float left;
    float top;
    float nearClip;
    float farClip;

    Eigen::Matrix4f perspective_transform();
    Eigen::Vector4f perspective_z_transform();
};

template <typename scalar_type>
point2<scalar_type> as_point(size2<scalar_type> const &size)
{
    return {size.width, size.height};
}
} // namespace lm

template <typename scalar_type>
std::ostream &
  operator<<(std::ostream &stream, const lm::size2<scalar_type> &size)
{
    stream << size.width << ", " << size.height;
    return stream;
}
