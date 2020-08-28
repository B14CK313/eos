//
// Created by jakob on 27.08.20.
//

#include <glad/glad.h>
#include "eos/scene/resources/Text.hpp"

eos::Text::Text(size_t length, std::shared_ptr<eos::Shader> shader, size_t textVAO, size_t textVBO, size_t textIBO, unsigned int cacheTexture) : length_(length), shader_(std::move(shader)), textVAO_(textVAO), textVBO_(textVBO), textIBO_(textIBO), cacheTexture_(cacheTexture){
}

eos::Text::~Text() {
    glDeleteVertexArrays(1, &textVAO_);
    glDeleteBuffers(1, &textVBO_);
    glDeleteBuffers(1, &textIBO_);
}

void eos::Text::set_position(glm::vec3 pos) {

}

void eos::Text::render() {
    shader_->set_vec2_uniform("offset", -450.0f, 450.0f);
    shader_->set_vec2_uniform("scale", 1/512.0f, 1/512.0f);
    shader_->set_vec3_uniform("color", 1.0f, 1.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0 + 15);
    glBindTexture(GL_TEXTURE_2D, cacheTexture_);

    glBindSampler(15, 0);
    glBindVertexArray(textVAO_);
    glDrawElements(GL_TRIANGLES, length_ * 6, GL_UNSIGNED_SHORT, nullptr);
}
