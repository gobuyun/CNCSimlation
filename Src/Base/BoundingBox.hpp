#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QtMath>

class Boundingbox
{
public:
    Boundingbox(glm::vec3 vec1, glm::vec3 vec2)
        :maxCorner(vec1)
        ,minCorner(vec2)
    {}
    explicit Boundingbox()
    {
        Boundingbox(glm::vec3{-9999, -9999, -9999}, glm::vec3{9999, 9999, 9999});
    }
    glm::vec3 maxCorner;
    glm::vec3 minCorner;

    void updateMaxCornerMinCornerIfNeed(glm::vec3 pos)
    {
        // x
        if (pos.x > maxCorner.x)
            maxCorner.x = pos.x;
        else if (pos.x < minCorner.x)
            minCorner.x = pos.x;
        // y
        if (pos.y > maxCorner.y)
            maxCorner.y = pos.y;
        else if (pos.y < minCorner.y)
            minCorner.y = pos.y;
        // z
        if (pos.z > maxCorner.z)
            maxCorner.z = pos.z;
        else if (pos.z < minCorner.z)
            minCorner.z = pos.z;
    }

    glm::vec3 center()
    {
        return glm::vec3(minCorner.x + (maxCorner.x - minCorner.x)/2,
                         minCorner.y + (maxCorner.y - minCorner.y)/2,
                         minCorner.z + (maxCorner.z - minCorner.z)/2);

    }
    double diagonalLength()
    {
        return qAbs(glm::distance(maxCorner, minCorner));
    }

    Boundingbox& operator+(Boundingbox other)
    {
        updateMaxCornerMinCornerIfNeed(other.maxCorner);
        updateMaxCornerMinCornerIfNeed(other.minCorner);
        return *this;
    }
};

#endif // BOUNDINGBOX_HPP
