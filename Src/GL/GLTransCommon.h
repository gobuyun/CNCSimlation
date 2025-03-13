#pragma once


#include <glm/glm.hpp>
#include "../Math/kdl/kinfam_io.hpp"
#include "../Math/kdl/frames_io.hpp"



inline KDL::Vector KV2GlV(glm::vec3 in)
{
    return KDL::Vector(in.x,in.y,in.z);
}

inline glm::vec3 GlV2KV(KDL::Vector in)
{
    glm::vec3 out;
    out.x = in.x();
    out.y = in.y();
    out.z = in.z();
    return out;
}
