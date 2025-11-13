// DomainWarp.hpp
// ---------------
// Domain warping and turbulence effects for noise maps
// 
// Domain warping displaces the sampling coordinates of noise functions,
// creating more organic, flowing patterns. Perfect for rivers, coastlines,
// clouds, marble textures, and natural-looking terrain deformation.
//
// Usage:
//   auto terrain = Noise::generate_perlin_map(...);
//   auto warped = Noise::domain_warp(terrain, 30.0f, 5);
//   auto turbulent = Noise::apply_turbulence(terrain, 0.5f, 3);

#ifndef DOMAINWARP_HPP
#define DOMAINWARP_HPP

#pragma once
#include <vector>
#include <functional>

namespace Noise {

// ============================================================================
// Domain Warping - Displaces sampling coordinates for organic patterns
// ============================================================================

// Apply domain warping to an existing noise map
// Displaces each pixel by sampling noise at that location
std::vector<std::vector<float>> domain_warp(
    const std::vector<std::vector<float>>& map,
    float strength = 20.0f,      // Displacement strength in pixels
    int seed = -1
);

// Apply domain warping with separate X and Y displacement maps
std::vector<std::vector<float>> domain_warp_custom(
    const std::vector<std::vector<float>>& map,
    const std::vector<std::vector<float>>& displaceX,
    const std::vector<std::vector<float>>& displaceY,
    float strength = 20.0f
);

// Fractal domain warping - apply warping recursively for deeper distortion
std::vector<std::vector<float>> fractal_domain_warp(
    const std::vector<std::vector<float>>& map,
    float strength = 20.0f,
    int iterations = 3,
    float decay = 0.5f,         // Strength multiplier per iteration
    int seed = -1
);

// ============================================================================
// Turbulence - Adds fine detail and variation
// ============================================================================

// Apply turbulence (absolute value of noise) for billowy, cloudy effects
std::vector<std::vector<float>> apply_turbulence(
    const std::vector<std::vector<float>>& map,
    float strength = 0.3f,      // Turbulence intensity
    int octaves = 3,            // Detail levels
    int seed = -1
);

// Apply directional turbulence (flows in a specific direction)
std::vector<std::vector<float>> directional_turbulence(
    const std::vector<std::vector<float>>& map,
    float angle,                // Direction in radians
    float strength = 0.3f,
    int octaves = 3,
    int seed = -1
);

// ============================================================================
// Specialized Warping Effects
// ============================================================================

// Marble effect - creates flowing veins like marble stone
std::vector<std::vector<float>> marble_effect(
    const std::vector<std::vector<float>>& map,
    float frequency = 5.0f,     // Vein frequency
    float warp_strength = 30.0f,
    int seed = -1
);

// Wood grain effect - creates circular rings with warping
std::vector<std::vector<float>> wood_grain_effect(
    const std::vector<std::vector<float>>& map,
    float centerX = 0.5f,       // Ring center (normalized)
    float centerY = 0.5f,
    float frequency = 10.0f,
    float warp_strength = 5.0f,
    int seed = -1
);

// Swirl effect - rotational domain warping around a point
std::vector<std::vector<float>> swirl_effect(
    const std::vector<std::vector<float>>& map,
    float centerX = 0.5f,       // Swirl center (normalized)
    float centerY = 0.5f,
    float strength = 2.0f,      // Rotation strength (radians)
    float radius = 1.0f         // Affected radius (normalized)
);

// ============================================================================
// Advanced Transformations
// ============================================================================

// Ridge noise - creates sharp ridges by inverting valleys
std::vector<std::vector<float>> ridge_noise(
    const std::vector<std::vector<float>>& map,
    float sharpness = 2.0f      // Higher = sharper ridges
);

// Billowy noise - creates puffy cloud-like patterns
std::vector<std::vector<float>> billowy_noise(
    const std::vector<std::vector<float>>& map,
    float puffiness = 2.0f      // Higher = puffier
);

// Folded noise - creates complex, layered patterns
std::vector<std::vector<float>> folded_noise(
    const std::vector<std::vector<float>>& map,
    int folds = 2               // Number of folds
);

// ============================================================================
// Utility Functions
// ============================================================================

// Generate displacement map for custom warping
std::vector<std::vector<float>> generate_displacement_map(
    int width,
    int height,
    float scale,
    int seed = -1
);

// Apply custom warp function to coordinates
std::vector<std::vector<float>> apply_custom_warp(
    const std::vector<std::vector<float>>& map,
    std::function<void(float&, float&, int, int)> warp_func
);

// Warp settings structure for batch operations
struct WarpSettings {
    float strength = 20.0f;
    int iterations = 1;
    float decay = 0.5f;
    int seed = -1;
    
    WarpSettings() = default;
    WarpSettings(float str, int iter = 1, float dec = 0.5f, int s = -1)
        : strength(str), iterations(iter), decay(dec), seed(s) {}
};

// Apply multiple warp effects in sequence
std::vector<std::vector<float>> apply_warp_chain(
    const std::vector<std::vector<float>>& map,
    const std::vector<WarpSettings>& chain
);

} // namespace Noise

#endif // DOMAINWARP_HPP
