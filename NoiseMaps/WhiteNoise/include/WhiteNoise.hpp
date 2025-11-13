// WhiteNoise.hpp
// ----------------
// A simple, standalone C++ header for generating 2D white noise maps.
//
// Usage Example:
// ---------------
// #include "WhiteNoise.hpp"
// auto noise = Noise::WhiteNoise::generate(512, 512, 42);
// Noise::WhiteNoise::save(noise, "white_noise.png");
//

#pragma once
#include <vector>
#include <string>

namespace Noise {

    // Forward declare OutputMode (defined globally in Noise.hpp)
    enum class OutputMode;

    class WhiteNoise {
    public:
        static std::vector<std::vector<float>> generate(int width, int height, int seed = -1);
        static void show(const std::vector<std::vector<float>>& noise);

        // Save to grayscale PNG or JPEG (auto-detected from extension)
        // If outputDir is empty, uses default ImageOutput/ directory
        static void save(const std::vector<std::vector<float>>& noise,
            const std::string& filename = "white_noise.png",
            const std::string& outputDir = "");
    };

    // Single-value sampling API: Sample white noise at specific coordinates
    // Uses hash-based random number generation for deterministic results
    float sample_whitenoise(
        float x,
        float y,
        int seed = -1
    );

    // Chunk-based generation: Generate noise for a specific chunk of the world
    std::vector<std::vector<float>> generate_whitenoise_chunk(
        int chunkX,           // Chunk X coordinate in world space
        int chunkY,           // Chunk Y coordinate in world space
        int chunkSize,        // Size of each chunk (width = height)
        int seed = -1
    );

    // Wrapper
    std::vector<std::vector<float>> create_whitenoise(
        int width = 256,
        int height = 256,
        int seed = -1,
        OutputMode mode = OutputMode::Image,
        const std::string& filename = "white_noise.png",
        const std::string& outputDir = ""
    );

} // namespace Noise
