//
// Created by jakob on 14.07.20.
//

#ifndef EOS_WINDOW_H
#define EOS_WINDOW_H


#include <string>
#include <glm/glm.hpp>
#include <SDL.h>

namespace eos {

    class Window {
    public:
        glm::ivec2 size_;

    public:
        Window(const std::string& title, int width, int height);

        virtual ~Window();

        void resize(SDL_WindowEvent event);

        void swap();
    private:
        SDL_Window* window_;

    private:
        void update_size();
    };
}

#endif //EOS_WINDOW_H
