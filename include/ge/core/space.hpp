/// @file space.hpp
/// @brief SpaceTag, SpaceConverterの定義
#pragma once

/// @namespace ge::core graphicsengine:core
namespace ge::core {

// 座標系の定義 (Coordinate Space)
struct PixelSpace   {};   // 物理ピクセル
struct UVSpace      {};   // テクスチャ座標
struct NDCSpace     {};   // 正規化デバイス座標


/// @struct Space
/// @brief 空間の定義
/// @tparam Dim    次数 "Dimension"
/// @tparam Kind   空間の種類 "Coordinte Space"
template <int Dim, typename Kind>
struct Space {
    static constexpr int dimension = Dim;
    using kind = Kind;
};

// よく使う空間のエイリアス
using Pixel2D = Space<2, PixelSpace>;
using NDC2D   = Space<2, NDCSpace>;
using UV2D    = Space<2, UVSpace>;

/// @struct SpaceConverter
/// @brief 空間変換のテンプレート宣言
template <typename ToSpace, typename FromSpace>
struct SpaceConverter;

} // namespace ge::core
