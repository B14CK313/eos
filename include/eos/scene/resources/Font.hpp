//
// Created by jakob on 26.08.20.
//

#ifndef EOS_FONT_HPP
#define EOS_FONT_HPP

#include <hb.h>
#include <freetype/fttypes.h>
#include <glm/glm.hpp>
#include <string_view>
#include <map>
#include <freetype/freetype.h>
#include "Text.hpp"

namespace eos {

    class Font {
    public:
        Font(std::string_view path, size_t pixelSize, size_t cacheSize);

        ~Font();

        float cache(const std::string_view string);

        void cache(FT_UInt codepoint);

        float cacheUsage() const;

        Text assemble(const std::string_view string) const;

        void draw_cache(glm::vec2 pos, glm::vec2 scale);

    private:
        struct GlyphVertex {
            float x;
            float y;
            float s;
            float t;
        };

        struct GlyphVertices {
            GlyphVertex bl_;
            GlyphVertex br_;
            GlyphVertex tl_;
            GlyphVertex tr_;
        };

        struct GlyphIndices {
            unsigned short bl1_;
            unsigned short br1_;
            unsigned short tl1_;
            unsigned short br2_;
            unsigned short tr2_;
            unsigned short tl2_;
        };

        typedef std::map<FT_UInt, size_t> GlyphIdxMap;
        typedef std::vector<GlyphVertices> GlyphVertVector;
        typedef std::vector<GlyphIndices> GlyphIdxVector;

        std::string_view path_;

        size_t cacheLineHeight_;
        glm::vec<2, size_t, glm::defaultp> cacheDim_;
        glm::vec<2, size_t, glm::defaultp> cacheFreeSlotPos_;

        GlyphIdxMap cacheGlyphIdxMap_;
        GlyphVertVector cacheGlyphVertList_;

        FT_Face face_;
        hb_font_t* font_;

        std::shared_ptr<eos::Shader> shader_;

        unsigned int cacheTexture_;
        // for cache debug only
        unsigned int cacheVAO_;
        unsigned int cacheVBO_;
        unsigned int cacheIBO_;
    };
}

#endif //EOS_FONT_HPP
