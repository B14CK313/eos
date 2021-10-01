//
// Created by jakob on 05.09.20.
//

#include "eos/core/ColorHSV.hpp"
#include "eos/scene/resources/GradientText.hpp"

eos::GradientText::GradientText(std::experimental::observer_ptr<eos::Graphics> graphics, std::shared_ptr<Font> font)
		: Text(graphics, font, std::make_shared<Shader>("res/textGradient.vert", "res/textGradient.frag")) {}

void eos::GradientText::render(const std::string& text, glm::vec2 pos, eos::ColorHSV gradientStartColor,
                               eos::ColorHSV gradientStopColor) const {
	struct Vertex {
		glm::vec4 pos;
		glm::vec4 color;
	};

	struct Vertices {
		Vertex bl;
		Vertex br;
		Vertex tr;
		Vertex tl;
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
	std::vector<Vertices> vertices(text.length());
	std::vector<Indices> indices(text.length());

	ColorRGB leftColor = gradientStartColor;
	ColorHSV colorStep = (gradientStopColor - gradientStartColor) / text.length();

	for (size_t i = 0; i < characters.size(); ++i) {
		auto character = font_->get(characters[i]);

		gradientStartColor += colorStep;
		ColorRGB rightColor = gradientStartColor;

		vertices[i].bl = {
				{
						pos.x + character.bearing.x,
						pos.y - character.texture.pixels.y + character.bearing.y,
						character.texture.coords.x,
						character.texture.coords.y + character.texture.coords.w
				},
				leftColor
		};
		vertices[i].br = {
				{
						pos.x + character.bearing.x + character.texture.pixels.x,
						pos.y - character.texture.pixels.y + character.bearing.y,
						character.texture.coords.x + character.texture.coords.z,
						character.texture.coords.y + character.texture.coords.w
				},
				rightColor
		};
		vertices[i].tr = {
				{
						pos.x + character.bearing.x + character.texture.pixels.x,
						pos.y + character.bearing.y,
						character.texture.coords.x + character.texture.coords.z,
						character.texture.coords.y
				},
				rightColor
		};
		vertices[i].tl = {
				{
						pos.x + character.bearing.x,
						pos.y + character.bearing.y,
						character.texture.coords.x,
						character.texture.coords.y
				},
				leftColor
		};

		indices[i].bl1 = i * 4 + 0;
		indices[i].br1 = i * 4 + 1;
		indices[i].tr1 = i * 4 + 2;
		indices[i].bl2 = i * 4 + 0;
		indices[i].tr2 = i * 4 + 2;
		indices[i].tl2 = i * 4 + 3;

		pos.x += character.advance >> 6;
		leftColor = rightColor;
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2,
	                      reinterpret_cast<const void*>(sizeof(glm::vec4)));
	glBufferData(GL_ARRAY_BUFFER, text.length() * sizeof(Vertices), &vertices[0], GL_STATIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, text.length() * sizeof(Indices), &indices[0], GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, text.length() * 6, GL_UNSIGNED_SHORT, nullptr);
}
