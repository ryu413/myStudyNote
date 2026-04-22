/// @file quantity.hpp
/// @brief Quantityの定義
#pragma once

#include <ge/core/space.hpp>
#include <utility>

namespace ge::core {

/// @struct Quantity
/// @brief 物理量
/// @tparam T 数値型
/// @tparam S 属する空間
template <typename T, typename S>
struct Quantity {
    using value_type = T;
    using space_type = S;
    
    T value = T(0);

    constexpr Quantity() = default;
    constexpr explicit Quantity(T v) : value{v} {}

    constexpr auto operator==(const Quantity& rhs) const -> bool {
        return value == rhs.value;
    }
    
    constexpr auto operator!=(const Quantity& rhs) const -> bool {
        return !(*this == rhs);
    }

    // 同じ単位同士の加減
    constexpr auto operator+=(const Quantity& rhs) -> Quantity& {
        value += rhs.value;
        return *this;
    }

    constexpr auto operator-=(const Quantity& rhs) -> Quantity& {
        value -= rhs.value;
        return *this;
    }

    /// @brief スカラー倍
    constexpr auto operator*=(const T& scalar) -> Quantity& {
        value *= scalar;
        return value;
    }

    // 生の値を取り出す
    [[nodiscard]] constexpr auto get() const -> T { return value; }

    /// @brief 空間変換のための糖衣構文（シンタックスシュガー）
    /// 他の Spaceへの明示的な変換 (キャスト)
    template <typename ToSpace, typename... Args>
    [[nodiscard]] constexpr auto to(Args&&... args) const {
        // 実装は SpaceConverterに
        return SpaceConverter<ToSpace, S>::apply(*this, std::forward<Args>(args)...);
    }
};

// 二項演算子
template <typename T, typename S>
[[nodiscard]] constexpr auto operator+(Quantity<T, S>& lhs, const Quantity<T, S>& rhs) -> Quantity<T, S> {
    lhs += rhs;
    return lhs;
}

template <typename T, typename S>
[[nodiscard]] constexpr auto operator-(Quantity<T, S>& lhs, const Quantity<T, S>& rhs) -> Quantity<T, S> {
    lhs -= rhs;
    return lhs;
}

/// @brief 空間 S に属する二つの物理量を線形補間する
template <typename T, typename S>
[[nodiscard]] constexpr auto lerp(const Quantity<T, S>& start, const Quantity<T, S>& end, float t) noexcept -> Quantity<T, S> {
    // 物理量の中身（value）に対して補間を行う
    // ※ColorData や Vector4 が operator+, operator* を持っている前提
    return Quantity<T, S>{ start.value + (end.value - start.value) * t };
}

/// @namespace literals リテラル演算子
/// @details
///     inlineにすることで
///         using namespace ge::core::literals;
///     と書けば、100.0_pxのような記法を有効化。
inline namespace literals {

constexpr auto operator""_px(unsigned long long v) -> Quantity<float, Pixel2D> {
    return Quantity<float, Pixel2D>{ static_cast<float>(v) };
}

constexpr auto operator""_px(long double v) -> Quantity<float, Pixel2D> {
    return Quantity<float, Pixel2D>{ static_cast<float>(v) };
}


} // namespace literals

} // namespace ge::core