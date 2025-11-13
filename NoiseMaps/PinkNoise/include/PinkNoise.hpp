#pragma once
#include <vector>
#include <string>
#include "Noise.hpp" // for OutputMode (gives full enum definition)

namespace Noise {

    enum class OutputMode;

    // PinkNoise generator using multi-octave block-filtered white noise
    class PinkNoise {
    public:
        explicit PinkNoise(int seed = -1);

        // Generate a block-averaged white noise layer (0 to 1)
        std::vector<std::vector<float>> make_blurred_white(
            int width,
            int height,
            int blockSize
        ) const;

    private:
        int seed_;
    };

    // Multi-octave Pink noise map generator (approx. 1/f^alpha)
    std::vector<std::vector<float>> generate_pink_map(
        int width,
        int height,
        int octaves = 6,
        float alpha = 1.0f,          // 1 = pink, 2 = brown/red, <1 = blue-ish
        int sampleRate = 44100,      // used to scale octave spacing
        float amplitude = 1.0f,      // output scaling
        int seed = -1
    );

    // Save as PNG / JPEG
    void save_pink_image(
        const std::vector<std::vector<float>>& noise,
        const std::string& filename = "pink_noise.png",
        const std::string& outputDir = ""
    );

    // Main wrapper
    std::vector<std::vector<float>> create_pinknoise(
        int width,
        int height,
        int octaves = 6,
        float alpha = 1.0f,
        int sampleRate = 44100,
        float amplitude = 1.0f,
        int seed = -1,
        OutputMode mode = OutputMode::Image,
        const std::string& filename = "pink_noise.png",
        const std::string& outputDir = ""
    );

} // namespace Noise
