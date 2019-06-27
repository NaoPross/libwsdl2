#pragma once

/* wsdl2 ttf support extension header
 *
 * This header wraps the SDL2_ttf library allowing the read-only
 * loading and rendering of text to surfaces.
 *
 */

#ifndef WSDL2_TTF
#warning "libwrapsdl2 is complied without support for SDL2_ttf"
#endif

#ifdef WSDL2_TTF
#include "wsdl2/util.hpp"
#include "wsdl2/video.hpp"

#include <string>
#include <optional>


extern "C" {
#include <SDL2/SDL_ttf.h>
}

namespace wsdl2::ttf {
    enum class style : int {
        bold = TTF_STYLE_BOLD,
        italic = TTF_STYLE_ITALIC,
        normal = TTF_STYLE_NORMAL,
        underline = TTF_STYLE_UNDERLINE,
        strikethrough = TTF_STYLE_NORMAL,
    };

    enum class hinting : int {
        normal = TTF_HINTING_NORMAL,
        light = TTF_HINTING_LIGHT,
        mono = TTF_HINTING_MONO,
        none = TTF_HINTING_NONE,
    };

    namespace glyph {
        struct metrics {
            std::uint16_t ch;
            int min_x;
            int max_x;
            int min_y;
            int max_y;
            int advance;
        };
    }

    class font {
    public:
        font() = delete;
        font(const font& other) = delete;

        font(const std::string& path, int ptsize);
        font(const std::string& path, int ptsize, long index);

        font(font&& other);
        ~font();

        /// style
        inline int style() const { return TTF_GetFontStyle(sdl()); }
        inline void style(int styles) { TTF_SetFontStyle(sdl(), styles); }

        inline bool bold() const {
            return (style() & static_cast<int>(style::bold));
        }

        inline void bold(bool enable) {
            if (enable)
                m_style |= static_cast<int>(style::bold);
            else
                m_style &= ~static_cast<int>(style::bold);

            style(m_style);
        }


        inline bool italic() const {
            return (style() & static_cast<int>(style::italic));
        }

        inline void italic(bool enable) {
            if (enable)
                m_style |= static_cast<int>(style::italic);
            else
                m_style &= ~static_cast<int>(style::italic);

            style(m_style);
        }

        inline bool underline() const {
            return (style() & static_cast<int>(style::underline));
        }

        inline void underline(bool enable) {
            if (enable)
                m_style |= static_cast<int>(style::underline);
            else
                m_style &= ~static_cast<int>(style::underline);

            style(m_style);
        }

        inline bool strikethrough() const {
            return (style() & static_cast<int>(style::strikethrough));
        }

        inline void strikethrough(bool enable) {
            if (enable)
                m_style |= static_cast<int>(style::strikethrough);
            else
                m_style &= ~static_cast<int>(style::strikethrough);

            style(m_style);
        }

        /// outline
        inline int outline() { return TTF_GetFontOutline(sdl()); }
        inline void outline(int outline) { TTF_SetFontOutline(sdl(), outline); }

        // hinting
        inline hinting hint() {
            return static_cast<hinting>(TTF_GetFontHinting(sdl()));
        }

        inline void hint(hinting h) {
            // avoid flushing cache if not necessary
            if (hint() == h)
                return;

            TTF_SetFontHinting(sdl(), static_cast<int>(h));
        }

        /// kerning
        inline bool kerning() { return (TTF_GetFontKerning(sdl())); }
        inline void kerning(bool enable) { TTF_SetFontKerning(sdl(), enable); }

        /// size and metrics
        inline int height() { return TTF_FontHeight(sdl()); }
        inline int ascent() { return TTF_FontAscent(sdl()); }
        inline int descent() { return TTF_FontDescent(sdl()); }
        inline int lineskip() { return TTF_FontLineSkip(sdl()); }

        /// faces
        inline long faces() { return TTF_FontFaces(sdl()); }
        inline bool is_fixed_width() {
            return TTF_FontFaceIsFixedWidth(sdl()) > 0;
        }

        inline std::optional<std::string> face_family_name() {
            char *name = TTF_FontFaceFamilyName(sdl());
            if (name == NULL)
                return std::nullopt;

            return std::string(name);
        }

        inline std::optional<std::string> face_style_name() {
            char *name = TTF_FontFaceStyleName(sdl());
            if (name == NULL)
                return std::nullopt;

            return std::string(name);
        }

        /// glyphs

        inline std::optional<int> is_glyph_provided(std::uint16_t ch) {
            int index = TTF_GlyphIsProvided(sdl(), ch);
            if (index == 0)
                return std::nullopt;

            return index;
        }

        inline std::optional<glyph::metrics> glyph_metrics(std::uint16_t ch) {
            glyph::metrics m;
            int rv = TTF_GlyphMetrics(sdl(), ch,
                                      &m.min_x, &m.max_x,
                                      &m.min_y, &m.max_y,
                                      &m.advance);

            if (util::check_ttf(-1 == rv)) {
                return std::nullopt;
            }

            return m;
        }

        /// size

        inline std::optional<std::pair<int, int>> text_size(const std::string& s) {
            int w, h;
            if (util::check_ttf(-1 == TTF_SizeText(sdl(), s.c_str(), &w, &h))) {
                return std::nullopt;
            }

            return std::make_pair(w, h);
        }

        inline std::optional<std::pair<int, int>> utf8_size(const std::string& s) {
            int w, h;
            if (util::check_ttf(-1 == TTF_SizeUTF8(sdl(), s.c_str(), &w, &h))) {
                return std::nullopt;
            }

            return std::make_pair(w, h);
        }
        
        inline std::optional<std::pair<int, int>> unicode_size(const std::basic_string<std::uint16_t>& s) {
            int w, h;
            if (util::check_ttf(-1 == TTF_SizeUNICODE(sdl(), s.c_str(), &w, &h))) {
                return std::nullopt;
            }

            return std::make_pair(w, h);
        }

        /// solid (crappy) rendering

        std::optional<surface> render_text_solid(const std::string& s, color fg) {
            SDL_Surface *sdlsurf = TTF_RenderText_Solid(sdl(), s.c_str(), fg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }
            
        std::optional<surface> render_utf8_solid(const std::string& s, color fg) {
            SDL_Surface *sdlsurf = TTF_RenderUTF8_Solid(sdl(), s.c_str(), fg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }
        
        std::optional<surface> render_unicode_solid(const std::basic_string<std::uint16_t>& s, color fg) {
            SDL_Surface *sdlsurf = TTF_RenderUNICODE_Solid(sdl(), s.c_str(), fg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }

        std::optional<surface> render_glyph_solid(std::uint16_t ch, color fg) {
            SDL_Surface *sdlsurf = TTF_RenderGlyph_Solid(sdl(), ch, fg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }

        /// shaded rendering

        std::optional<surface> render_text_shaded(const std::string& s, color fg, color bg) {
            SDL_Surface *sdlsurf = TTF_RenderText_Shaded(sdl(), s.c_str(), fg, bg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }
            
        std::optional<surface> render_utf8_shaded(const std::string& s, color fg, color bg) {
            SDL_Surface *sdlsurf = TTF_RenderUTF8_Shaded(sdl(), s.c_str(), fg, bg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }
        
        std::optional<surface> render_unicode_shaded(const std::basic_string<std::uint16_t>& s, color fg, color bg) {
            SDL_Surface *sdlsurf = TTF_RenderUNICODE_Shaded(sdl(), s.c_str(), fg, bg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }

        std::optional<surface> render_glyph_shaded(std::uint16_t ch, color fg, color bg) {
            SDL_Surface *sdlsurf = TTF_RenderGlyph_Shaded(sdl(), ch, fg, bg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }

        /// blended (slow but good) rendering

        std::optional<surface> render_text_blended(const std::string& s, color fg) {
            SDL_Surface *sdlsurf = TTF_RenderText_Blended(sdl(), s.c_str(), fg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }
            
        std::optional<surface> render_utf8_blended(const std::string& s, color fg) {
            SDL_Surface *sdlsurf = TTF_RenderUTF8_Blended(sdl(), s.c_str(), fg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }
        
        std::optional<surface> render_unicode_blended(const std::basic_string<std::uint16_t>& s, color fg) {
            SDL_Surface *sdlsurf = TTF_RenderUNICODE_Blended(sdl(), s.c_str(), fg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }

        std::optional<surface> render_glyph_blended(std::uint16_t ch, color fg) {
            SDL_Surface *sdlsurf = TTF_RenderGlyph_Blended(sdl(), ch, fg);
            if (util::check_ttf(sdlsurf == NULL))
                return std::nullopt;

            return surface(sdlsurf);
        }

    private:
        int m_style = static_cast<int>(style::normal);
        TTF_Font *m_font;

        TTF_Font* sdl();
        const TTF_Font* sdl() const;
    };
}

#endif
