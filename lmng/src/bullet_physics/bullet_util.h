#pragma once

#include <LinearMath/btVector3.h>

inline btVector3 vec_to_bt(Eigen::Vector3f const &vec)
{
    return btVector3{vec[0], vec[1], vec[2]};
}

inline btQuaternion quat_to_bt(Eigen::Quaternionf const &quat)
{
    return btQuaternion{quat.x(), quat.y(), quat.z(), quat.w()};
}

inline Eigen::Quaternionf bt_to_quat(btQuaternion const &bt_quat)
{
    return {bt_quat.getW(), bt_quat.getX(), bt_quat.getY(), bt_quat.getZ()};
}

inline void set_from_bt(Eigen::Vector3f &dest, btVector3 const &src)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
}

inline void set_from_bt(Eigen::Quaternionf &dest, btQuaternion const &src)
{
    dest.x() = src.x();
    dest.y() = src.y();
    dest.z() = src.z();
    dest.w() = src.w();
}

class manifolds
{
  public:
    explicit manifolds(btDynamicsWorld *physics_world)
        : dispatcher{physics_world->getDispatcher()},
          n_manifolds{dispatcher->getNumManifolds()}
    {
    }

    struct iterator
    {
        using index_type = int;
        using reference = btPersistentManifold &;

        bool operator!=(iterator const &other) { return index != other.index; }

        reference operator*()
        {
            return *dispatcher->getManifoldByIndexInternal(index);
        }

        iterator &operator++()
        {
            index++;
            return *this;
        }

        btDispatcher *dispatcher;
        int index;
    };

    iterator begin() { return iterator{dispatcher, 0}; }

    iterator end() { return iterator{dispatcher, n_manifolds}; }

    btDispatcher *dispatcher;
    int n_manifolds;
};
