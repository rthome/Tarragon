#pragma once

#include <glm/matrix.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "component.h"

namespace tarragon
{
    class Camera : public UpdateComponent
    {
    private:
        glm::mat4 m_view;
        glm::mat4 m_projection;

    public:
        glm::mat4 const& view() const;
        glm::mat4 const& projection() const;

        virtual void update(Clock const& clock, Input const& input) override;
    };
}
