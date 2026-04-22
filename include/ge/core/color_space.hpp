/// @file color.hpp
/// @brief Colorの定義
#pragma once

#include <ge/core/quantity.hpp>

#include <algorithm>
#include <type_traits>


namespace ge::core {

// 1.色空間Tagの定義
struct LinearColorSpace {};
struct SRGBColorSpace   {};

// 2.空間の定義（４次元）
using LinearRGBA = Space<4, LinearColorSpace>;
using SRGBRGBA   = Space<4, SRGBColorSpace>;

// 色の実体（データ構造）
/// GPU転送を考慮して、16バイト境界に整列
struct ColorData {
    float r, g, b, a;
    // 基本的な演算を定義（Quantity内での計算に使用）
    constexpr ColorData& operator+=(const ColorData& rhs) noexcept {
        r += rhs.r; g += rhs.g; b += rhs.b; a += rhs.a;
        return *this;
    }
    constexpr ColorData& operator*=(float s) noexcept {
        r *= s; g *= s; b *= s; a *= s;
        return *this;
    }
};
/// 空間 "LinearRGBA"における、大きさ（データの実体）Vector4の量をColorと定義する。

/// @struct Color
/// @brief 色空間 LinearRGBA/SRGBRGBAに属する、大きさColorDataの物理量
/// @tparam S 属する色空間 (LinearRGBA / SRGBRGBA)
template <typename S>
struct Color : public Quantity<ColorData, S> {
    static_assert(S::dimension == 4, "Color space must be 4D (RGBA)" );
    
    // 基底クラスのコンストラクタを継承
    using Quantity<ColorData, S>::Quantity;

    /// @brief デフォルトコンストラクタ
    constexpr Color() : Quantity<float[4], S>{{0.0f, 0.0f, 0.0f, 1.0f}} { }

    /// @brief 成分指定コンストラクタ
    constexpr Color(float r, float g, float b, float a = 1.0f)
        : Quantity<ColorData, S>{ColorData{r, g, b, a}} {}

    // Accessor (const)
    [[nodiscard]] constexpr auto red  () const noexcept -> float { return this -> value.r; }
    [[nodiscard]] constexpr auto green() const noexcept -> float { return this -> value.g; }
    [[nodiscard]] constexpr auto blue () const noexcept -> float { return this -> value.b; }
    [[nodiscard]] constexpr auto alpha() const noexcept -> float { return this -> value.a; }

    // Accessor (refernce)
    [[nodiscard]] constexpr auto red  () noexcept -> float { return this -> value.r; }
    [[nodiscard]] constexpr auto green() noexcept -> float { return this -> value.g; }
    [[nodiscard]] constexpr auto blue () noexcept -> float { return this -> value.b; }
    [[nodiscard]] constexpr auto alpha() noexcept -> float { return this -> value.a; }

    /// @brief クランプ処理
    [[nodiscard]] constexpr auto clamped() const noexcept -> Color<S> {
        return  {
            std::clamp(red(),   0.0f, 1.0f),
            std::clamp(green(), 0.0f, 1.0f),
            std::clamp(blue(),  0.0f, 1.0f),
            std::clamp(alpha(), 0.0f, 1.0f)
        };
    }

};

//（Colorとしての型を維持するためのオーバーロード）
template <typename S>
[[nodiscard]] constexpr auto operator+(Color<S> lhs, const Color<S>& rhs) -> Color<S> {
    // 基底の += を呼び出し、Colorとして返す
    lhs.value += rhs.value; 
    return lhs;
}

template <typename S>
[[nodiscard]] inline constexpr auto operator*(Color<S> lhs, const Color<S>& rhs) -> Color<S> {
    lhs.value *= rhs.value;
    return lhs;
}

/// @brief 線形補完 (C++17)
template <typename S>
[[nodiscard]] constexpr auto lerp(const Color<S>& start, const Color<S>& end, float t) noexcept -> Color<S> {
    // Quantity側の lerpを呼出し、Colorで再構築
    auto res_val = ge::core::lerp(
            static_cast<const Quantity<ColorData, S>&>(start),
            static_cast<const Quantity<ColorData, S>&>(end), t
        );
    return static_cast<Color<S>&>(res_val);
}

// 型エイリアス
using LinearColor = Color<LinearRGBA>;
using SRGBColor   = Color<SRGBRGBA>;

/// @namespace colors
/// @brief プリセット定数
namespace colors {
    inline constexpr LinearColor white { 1.0f, 1.0f, 1.0f, 1.0f };
    inline constexpr LinearColor black { 0.0f, 0.0f, 0.0f, 1.0f };
    inline constexpr LinearColor red   { 1.0f, 0.0f, 0.0f, 1.0f };
    inline constexpr LinearColor clear { 0.0f, 0.0f, 0.0f, 0.0f };    // ClearValue用
}

// 検証
/// @brief メモリサイズが float 4個分 (16byte)であることを確認
static_assert(sizeof(LinearColor) == 16);

/// @brief 継承等がなく、memcpyで安全にコピーできることを確認
static_assert(std::is_trivially_copyable_v<LinearColor>);

/// @brief メモリのアラインメントが16バイト境界であることを確認
static_assert(alignof(LinearColor) == 16);

} // namespace ge::core