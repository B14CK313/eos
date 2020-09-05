//
// Created by jakob on 05.09.20.
//

#include <utf8.h>
#include <glm/gtc/matrix_transform.hpp>
#include <eos/core/ServiceProvider.h>
#include "eos/scene/resources/Text.hpp"

eos::Text::Text(std::shared_ptr<Font> font) : Text(font, std::make_shared<Shader>("res/text.vert", "res/text.frag")) {}

eos::Text::Text(std::shared_ptr<Font> font, std::shared_ptr<Shader> shader) : font_(font), shader_(shader) {
    shader_->use();
    shader_->set_int_uniform("textureAtlas", 1);
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
}

eos::Text::~Text() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

void eos::Text::render(const std::string& text, glm::vec2 pos, eos::ColorRGB color) const {
    struct Vertices {
        glm::vec4 bl;
        glm::vec4 br;
        glm::vec4 tr;
        glm::vec4 tl;
    };

    struct Indices {
        unsigned short bl1;
        unsigned short br1;
        unsigned short tr1;

        unsigned short bl2;
        unsigned short tr2;
        unsigned short tl2;
    };


    auto characters = setup_render(text);
    std::vector<Vertices> vertices(characters.size());
    std::vector<Indices> indices(characters.size());

    for (size_t i = 0; i < characters.size(); ++i) {
        auto character = font_->get(characters[i]);
        vertices[i].bl = {
                pos.x + character.bearing.x,
                pos.y - character.texture.pixels.y + character.bearing.y,
                character.texture.coords.x,
                character.texture.coords.y + character.texture.coords.w
        };
        vertices[i].br = {
                pos.x + character.bearing.x + character.texture.pixels.x,
                pos.y - character.texture.pixels.y + character.bearing.y,
                character.texture.coords.x + character.texture.coords.z,
                character.texture.coords.y + character.texture.coords.w
        };
        vertices[i].tr = {
                pos.x + character.bearing.x + character.texture.pixels.x,
                pos.y + character.bearing.y,
                character.texture.coords.x + character.texture.coords.z,
                character.texture.coords.y
        };
        vertices[i].tl = {
                pos.x + character.bearing.x,
                pos.y + character.bearing.y,
                character.texture.coords.x,
                character.texture.coords.y
        };

        indices[i].bl1 = i * 4 + 0;
        indices[i].br1 = i * 4 + 1;
        indices[i].tr1 = i * 4 + 2;
        indices[i].bl2 = i * 4 + 0;
        indices[i].tr2 = i * 4 + 2;
        indices[i].tl2 = i * 4 + 3;

        pos.x += character.advance >> 6;
    }

    shader_->set_vec4_uniform("color", color);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
    glBufferData(GL_ARRAY_BUFFER, text.length() * sizeof(Vertices), &vertices[0], GL_STATIC_DRAW);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, text.length() * sizeof(Indices), &indices[0], GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, text.length() * 6, GL_UNSIGNED_SHORT, nullptr);
}

std::u32string eos::Text::setup_render(const std::string& text) const {
    shader_->use();
    glm::vec2 windowDim = eos::ServiceProvider::getWindow().get_size();
    glm::mat4 projection = glm::ortho(0.0f, windowDim.x, 0.0f, windowDim.y);
    shader_->set_mat4_uniform("projection", projection);

    font_->use();

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

    return utf8::utf8to32(text);
}
