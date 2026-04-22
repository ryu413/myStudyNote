/// @file space_converter.hpp
/// @brief SpaceConverterの特殊化定義
#pragma once

#include <ge/core/space.hpp>
#include <ge/core/quantity.hpp>
#include <ge/core/color_space.hpp>

namespace ge::core {

// --- 特定の変換を特殊化で実装 ---

// Pixel2D -> NDC2D
template <>
struct SpaceConverter<NDC2D, Pixel2D> {
    static constexpr auto apply(const Quantity<float, Pixel2D>& q, float screen_res)
        -> Quantity<float, NDC2D> {
        // (px / res) * 2.0 - 1.0
        return Quantity<float, NDC2D>{(q.value / screen_res) * 2.0f - 1.0f};
    }
};


// --- 2. NDC2D -> Pixel2D ---
template <>
struct SpaceConverter<Pixel2D, NDC2D> {
    static constexpr auto apply(Quantity<float, NDC2D> q, float screen_res) -> Quantity<float, Pixel2D> {
        // NDC からピクセル座標へ
        return Quantity<float, Pixel2D>{ (q.value + 1.0f) * 0.5f * screen_res };
    }
};

// --- 3. Pixel2D -> UV2D ---
template <>
struct SpaceConverter<UV2D, Pixel2D> {
    static constexpr auto apply(Quantity<float, Pixel2D> q, float screen_res) -> Quantity<float, UV2D> {
        // テクスチャ座標 (0.0 ~ 1.0) へ。中心補正の 0.5 を入れるのが一般的
        return Quantity<float, UV2D>{ (q.value + 0.5f) / screen_res };
    }
};

// LinearRGBA -> SRGBRGBA への変換実装
template <>
struct SpaceConverter<SRGBRGBA, LinearRGBA> {
    static constexpr auto apply(const LinearColor& src) -> SRGBColor {
        auto gamma = [](float l) {
            return (l <= 0.0031308f) ? (l * 12.92f) : (1.055f * std::pow(l, 1.0f / 2.4f) - 0.055f);
        };
        return SRGBColor{ gamma(src.red()), gamma(src.g()), gamma(src.b()), src.a() };
    }
};

} // namespace ge::core