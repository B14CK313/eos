//
// Created by jakob on 30.08.20.
//

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include "eos/scene/resources/TextureAtlas.hpp"


eos::TextureAtlas::TextureAtlas(glm::uvec2 atlasDim, int type, int format, int internalformat) : atlasDim_(atlasDim),
                                                                                                 type_(type),
                                                                                                 format_(format) {
    if (internalformat == -1) internalformat_ = format;

    glActiveTexture(GL_TEXTURE0 + 1); //TODO: request free texture slot
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat_, atlasDim_.x, atlasDim_.y, 0, format_, type_, nullptr);

#ifdef DEBUG
    const float vertices[]{
            // positions          // texture coords
            -0.9f, -0.9f, 0.0f, 0.0f, 1.0f, // bottom left
            0.9f, -0.9f, 0.0f, 1.0f, 1.0f, // bottom right
            0.9f, 0.9f, 0.0f, 1.0f, 0.0f, // top right
            0.9f, 0.9f, 0.0f, 1.0f, 0.0f, // top right
            -0.9f, 0.9f, 0.0f, 0.0f, 0.0f, // top left
            -0.9f, -0.9f, 0.0f, 0.0f, 1.0f  //bottom left
    };
    shader_ = std::make_shared<Shader>("res/textureRect.vert", "res/textureRect.frag");
    shader_->use();
    shader_->set_int_uniform("textureSampler", 1);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
#endif //DEBUG
}

eos::TextureAtlas::operator GLuint() const {
    return texture_;
}


void eos::TextureAtlas::render(glm::uvec2 pos, glm::vec2 scale) const {
#ifdef DEBUG

    shader_->use();
    //shader_->set_vec2_uniform("pos", pos);
    //shader_->set_float_uniform("scale", scale);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, texture_);
    // Bind to sampler name zero == the currently bound texture's sampler state becomes active (no dedicated sampler)
    glBindSampler(1, 0);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
#endif //DEBUG
}
