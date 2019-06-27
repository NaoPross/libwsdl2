#ifdef WSDL2_TTF

#include "wsdl2/ttf.hpp"
#include "wsdl2/util.hpp"


using namespace wsdl2::ttf;

font::font(const std::string& path, int ptsize) {
    m_font = TTF_OpenFont(path.c_str(), ptsize);

    if (util::check_ttf(m_font != NULL)) {
        throw std::runtime_error("failed to load font: " + path);
    }
}

font::font(const std::string& path, int ptsize, long index) {
    m_font = TTF_OpenFontIndex(path.c_str(), ptsize, index);

    if (util::check_ttf(m_font != NULL)) {
        throw std::runtime_error("failed to load font: " + path);
    }
}

font::font(font&& other) {
    m_font = other.m_font;
    other.m_font = NULL;
}

font::~font() {
    if (m_font != NULL) {
        TTF_CloseFont(m_font);
        m_font = NULL;
    }
}



TTF_Font* font::sdl() {
#ifdef DEBUG
    if (m_font == NULL) {
        throw std::runtime_error(
            "attempted to call texture::sdl() when m_texture is NULL"
        );
    }
#endif

    return m_font;
}

const TTF_Font* font::sdl() const {
#ifdef DEBUG
    if (m_font == NULL) {
        throw std::runtime_error(
            "attempted to call texture::sdl() when m_texture is NULL"
        );
    }
#endif

    return m_font;
}

#endif
