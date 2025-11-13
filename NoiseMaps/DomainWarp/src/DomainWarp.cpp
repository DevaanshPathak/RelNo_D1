// DomainWarp.cpp
// ---------------
// Implementation of domain warping and turbulence effects

#include "DomainWarp.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <random>

namespace Noise {

// Helper: Generate Perlin noise for displacement
namespace {
    class SimplePerlin {
    private:
        std::vector<int> p;
        
        static float fade(float t) {
            return t * t * t * (t * (t * 6 - 15) + 10);
        }
        
        static float lerp(float a, float b, float t) {
            return a + t * (b - a);
        }
        
        static float grad(int hash, float x, float y) {
            int h = hash & 3;
            float u = h < 2 ? x : y;
            float v = h < 2 ? y : x;
            return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
        }
        
    public:
        SimplePerlin(int seed) {
            std::mt19937 rng(seed);
            p.resize(512);
            for (int i = 0; i < 256; i++) p[i] = i;
            std::shuffle(p.begin(), p.begin() + 256, rng);
            for (int i = 0; i < 256; i++) p[256 + i] = p[i];
        }
        
        float noise(float x, float y) const {
            int X = static_cast<int>(std::floor(x)) & 255;
            int Y = static_cast<int>(std::floor(y)) & 255;
            
            x -= std::floor(x);
            y -= std::floor(y);
            
            float u = fade(x);
            float v = fade(y);
            
            int A = p[X] + Y;
            int B = p[X + 1] + Y;
            
            return lerp(
                lerp(grad(p[A], x, y), grad(p[B], x - 1, y), u),
                lerp(grad(p[A + 1], x, y - 1), grad(p[B + 1], x - 1, y - 1), u),
                v
            );
        }
    };
    
    float clamp(float value, float min, float max) {
        return std::max(min, std::min(max, value));
    }
}

// ============================================================================
// Domain Warping
// ============================================================================

std::vector<std::vector<float>> domain_warp(
    const std::vector<std::vector<float>>& map,
    float strength,
    int seed
) {
    if (seed == -1) seed = 12345;
    
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    // Generate displacement noise
    auto displaceX = generate_displacement_map(width, height, 0.02f, seed);
    auto displaceY = generate_displacement_map(width, height, 0.02f, seed + 1);
    
    return domain_warp_custom(map, displaceX, displaceY, strength);
}

std::vector<std::vector<float>> domain_warp_custom(
    const std::vector<std::vector<float>>& map,
    const std::vector<std::vector<float>>& displaceX,
    const std::vector<std::vector<float>>& displaceY,
    float strength
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Get displacement
            float dx = (displaceX[y][x] * 2.0f - 1.0f) * strength;
            float dy = (displaceY[y][x] * 2.0f - 1.0f) * strength;
            
            // Apply displacement
            int newX = static_cast<int>(x + dx);
            int newY = static_cast<int>(y + dy);
            
            // Clamp to bounds
            newX = clamp(newX, 0, width - 1);
            newY = clamp(newY, 0, height - 1);
            
            result[y][x] = map[newY][newX];
        }
    }
    
    return result;
}

std::vector<std::vector<float>> fractal_domain_warp(
    const std::vector<std::vector<float>>& map,
    float strength,
    int iterations,
    float decay,
    int seed
) {
    if (seed == -1) seed = 12345;
    
    auto result = map;
    float currentStrength = strength;
    
    for (int i = 0; i < iterations; i++) {
        result = domain_warp(result, currentStrength, seed + i);
        currentStrength *= decay;
    }
    
    return result;
}

// ============================================================================
// Turbulence
// ============================================================================

std::vector<std::vector<float>> apply_turbulence(
    const std::vector<std::vector<float>>& map,
    float strength,
    int octaves,
    int seed
) {
    if (seed == -1) seed = 12345;
    
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    SimplePerlin noise(seed);
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float turbulence = 0.0f;
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float maxValue = 0.0f;
            
            for (int octave = 0; octave < octaves; octave++) {
                float nx = x * 0.01f * frequency;
                float ny = y * 0.01f * frequency;
                
                // Turbulence uses absolute value of noise
                turbulence += std::abs(noise.noise(nx, ny)) * amplitude;
                maxValue += amplitude;
                
                amplitude *= 0.5f;
                frequency *= 2.0f;
            }
            
            turbulence /= maxValue;
            
            // Mix original with turbulence
            result[y][x] = map[y][x] + turbulence * strength;
            result[y][x] = clamp(result[y][x], 0.0f, 1.0f);
        }
    }
    
    return result;
}

std::vector<std::vector<float>> directional_turbulence(
    const std::vector<std::vector<float>>& map,
    float angle,
    float strength,
    int octaves,
    int seed
) {
    if (seed == -1) seed = 12345;
    
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    SimplePerlin noise(seed);
    
    float dirX = std::cos(angle);
    float dirY = std::sin(angle);
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float turbulence = 0.0f;
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float maxValue = 0.0f;
            
            for (int octave = 0; octave < octaves; octave++) {
                // Sample along direction vector
                float nx = (x + dirX * 50) * 0.01f * frequency;
                float ny = (y + dirY * 50) * 0.01f * frequency;
                
                turbulence += std::abs(noise.noise(nx, ny)) * amplitude;
                maxValue += amplitude;
                
                amplitude *= 0.5f;
                frequency *= 2.0f;
            }
            
            turbulence /= maxValue;
            
            result[y][x] = map[y][x] + turbulence * strength;
            result[y][x] = clamp(result[y][x], 0.0f, 1.0f);
        }
    }
    
    return result;
}

// ============================================================================
// Specialized Warping Effects
// ============================================================================

std::vector<std::vector<float>> marble_effect(
    const std::vector<std::vector<float>>& map,
    float frequency,
    float warp_strength,
    int seed
) {
    if (seed == -1) seed = 12345;
    
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    SimplePerlin noise(seed);
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Apply domain warping
            float nx = x * 0.01f;
            float ny = y * 0.01f;
            
            float warpX = noise.noise(nx * 2.0f, ny * 2.0f) * warp_strength;
            float warpY = noise.noise(nx * 2.0f + 5.2f, ny * 2.0f + 1.3f) * warp_strength;
            
            // Create marble veins using sine wave
            float veinValue = std::sin((x + warpX) * frequency * 0.1f + 
                                      (y + warpY) * frequency * 0.05f);
            
            // Convert to [0, 1] and mix with original
            veinValue = (veinValue + 1.0f) * 0.5f;
            result[y][x] = map[y][x] * 0.3f + veinValue * 0.7f;
        }
    }
    
    return result;
}

std::vector<std::vector<float>> wood_grain_effect(
    const std::vector<std::vector<float>>& map,
    float centerX,
    float centerY,
    float frequency,
    float warp_strength,
    int seed
) {
    if (seed == -1) seed = 12345;
    
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    SimplePerlin noise(seed);
    
    float cx = centerX * width;
    float cy = centerY * height;
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Distance from center
            float dx = x - cx;
            float dy = y - cy;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            // Add warping
            float nx = x * 0.01f;
            float ny = y * 0.01f;
            float warp = noise.noise(nx, ny) * warp_strength;
            
            // Create circular rings
            float rings = std::sin((distance + warp) * frequency * 0.1f);
            rings = (rings + 1.0f) * 0.5f;
            
            result[y][x] = map[y][x] * 0.4f + rings * 0.6f;
        }
    }
    
    return result;
}

std::vector<std::vector<float>> swirl_effect(
    const std::vector<std::vector<float>>& map,
    float centerX,
    float centerY,
    float strength,
    float radius
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    float cx = centerX * width;
    float cy = centerY * height;
    float maxRadius = radius * std::min(width, height);
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float dx = x - cx;
            float dy = y - cy;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance < maxRadius) {
                // Calculate rotation based on distance
                float factor = 1.0f - (distance / maxRadius);
                float angle = factor * factor * strength;
                
                // Rotate coordinates
                float cosA = std::cos(angle);
                float sinA = std::sin(angle);
                
                float rotX = dx * cosA - dy * sinA;
                float rotY = dx * sinA + dy * cosA;
                
                int newX = static_cast<int>(cx + rotX);
                int newY = static_cast<int>(cy + rotY);
                
                newX = clamp(newX, 0, width - 1);
                newY = clamp(newY, 0, height - 1);
                
                result[y][x] = map[newY][newX];
            } else {
                result[y][x] = map[y][x];
            }
        }
    }
    
    return result;
}

// ============================================================================
// Advanced Transformations
// ============================================================================

std::vector<std::vector<float>> ridge_noise(
    const std::vector<std::vector<float>>& map,
    float sharpness
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Invert and sharpen
            float value = 1.0f - std::abs(map[y][x] * 2.0f - 1.0f);
            result[y][x] = std::pow(value, sharpness);
        }
    }
    
    return result;
}

std::vector<std::vector<float>> billowy_noise(
    const std::vector<std::vector<float>>& map,
    float puffiness
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Take absolute value to create billowy effect
            float value = std::abs(map[y][x] * 2.0f - 1.0f);
            result[y][x] = std::pow(value, 1.0f / puffiness);
        }
    }
    
    return result;
}

std::vector<std::vector<float>> folded_noise(
    const std::vector<std::vector<float>>& map,
    int folds
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float value = map[y][x];
            
            for (int i = 0; i < folds; i++) {
                value = std::abs(value * 2.0f - 1.0f);
            }
            
            result[y][x] = value;
        }
    }
    
    return result;
}

// ============================================================================
// Utility Functions
// ============================================================================

std::vector<std::vector<float>> generate_displacement_map(
    int width,
    int height,
    float scale,
    int seed
) {
    if (seed == -1) seed = 12345;
    
    SimplePerlin noise(seed);
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float value = noise.noise(x * scale, y * scale);
            result[y][x] = (value + 1.0f) * 0.5f;  // Normalize to [0, 1]
        }
    }
    
    return result;
}

std::vector<std::vector<float>> apply_custom_warp(
    const std::vector<std::vector<float>>& map,
    std::function<void(float&, float&, int, int)> warp_func
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float fx = static_cast<float>(x);
            float fy = static_cast<float>(y);
            
            warp_func(fx, fy, width, height);
            
            int newX = static_cast<int>(fx);
            int newY = static_cast<int>(fy);
            
            newX = clamp(newX, 0, width - 1);
            newY = clamp(newY, 0, height - 1);
            
            result[y][x] = map[newY][newX];
        }
    }
    
    return result;
}

std::vector<std::vector<float>> apply_warp_chain(
    const std::vector<std::vector<float>>& map,
    const std::vector<WarpSettings>& chain
) {
    auto result = map;
    
    for (const auto& settings : chain) {
        result = fractal_domain_warp(
            result,
            settings.strength,
            settings.iterations,
            settings.decay,
            settings.seed
        );
    }
    
    return result;
}

} // namespace Noise
