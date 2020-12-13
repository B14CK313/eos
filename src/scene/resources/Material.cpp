//
// Created by jakob on 09.09.20.
//

#include <glad/glad.h>
#include "eos/scene/resources/Material.hpp"

eos::Material::Material(const eos::ColorRGB& ambient, const eos::ColorRGB& diffuse, const eos::ColorRGB& specular,
                        float shininess) : ambient_(ambient), diffuse_(diffuse), specular_(specular),
                                           shininess_(shininess) {
    glGenBuffers(1, &ubo_);
}

unsigned int eos::Material::use() const {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec3) * 3 + sizeof(float), &*this, GL_STATIC_DRAW);
    return ubo_;
}
