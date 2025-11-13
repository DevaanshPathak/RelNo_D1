#include "Noise.hpp"
#include "PinkNoise.hpp"
#include "stb_image_write.h"

#include <random>
#include <vector>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace Noise {

    // ---------------------------------------------------------
    // Constructor
    // ---------------------------------------------------------
    PinkNoise::PinkNoise(int seed) : seed_(seed) {}


    // ---------------------------------------------------------
    // Generate blurred white noise using block averaging
    // ---------------------------------------------------------
    std::vector<std::vector<float>> PinkNoise::make_blurred_white(
        int width,
        int height,
        int blockSize
    ) const
    {
        if (blockSize < 1) blockSize = 1;

        // --- RNG setup ---
        std::mt19937 rng;
        if (seed_ >= 0) rng.seed(seed_);
        else rng.seed(std::random_device{}());

        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        // --- Generate raw white noise ---
        std::vector<std::vector<float>> white(height, std::vector<float>(width));
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                white[y][x] = dist(rng);

        // --- Apply block averaging ---
        std::vector<std::vector<float>> blurred(height, std::vector<float>(width, 0.0f));

        for (int by = 0; by < height; by += blockSize)
        {
            for (int bx = 0; bx < width; bx += blockSize)
            {
                int ey = std::min(by + blockSize, height);
                int ex = std::min(bx + blockSize, width);

                float sum = 0.0f;
                int count = 0;

                for (int yy = by; yy < ey; ++yy)
                    for (int xx = bx; xx < ex; ++xx)
                    {
                        sum += white[yy][xx];
                        ++count;
                    }

                float avg = (count > 0) ? sum / count : 0.0f;

                for (int yy = by; yy < ey; ++yy)
                    for (int xx = bx; xx < ex; ++xx)
                        blurred[yy][xx] = avg;
            }
        }

        return blurred;
    }


    // ---------------------------------------------------------
    // Multi-octave pink noise map (1/f^alpha approximation)
    // ---------------------------------------------------------
    std::vector<std::vector<float>> generate_pink_map(
        int width,
        int height,
        int octaves,
        float alpha,
        int sampleRate,
        float amplitude,
        int seed
    ) {
        if (width <= 0 || height <= 0)
            throw std::invalid_argument("Pink noise: width/height must be > 0");

        if (octaves < 1)
            throw std::invalid_argument("Pink noise: octaves must be >= 1");

        if (alpha < 0.0f) alpha = 0.0f;      // allow only valid spectral slopes
        if (amplitude <= 0.0f) amplitude = 1.0f;
        if (sampleRate < 1) sampleRate = 44100;


        std::vector<std::vector<float>> acc(height, std::vector<float>(width, 0.0f));
        float totalWeight = 0.0f;

        // sampleRate affects "octave spacing"
        float baseSpacing = std::max(1.0f, std::sqrt(static_cast<float>(sampleRate) / 44100.0f));

        for (int o = 0; o < octaves; ++o)
        {
            int blockSize = static_cast<int>(baseSpacing * std::pow(2.0f, o));
            if (blockSize < 1) blockSize = 1;

            PinkNoise pn(seed >= 0 ? seed + o : -1);
            auto layer = pn.make_blurred_white(width, height, blockSize);

            // 1/f^alpha weighting
            float weight = 1.0f / std::pow(static_cast<float>(blockSize), alpha);

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                    acc[y][x] += layer[y][x] * weight;
            }

            totalWeight += weight;
        }

        // --- Normalize to [0, amplitude] ---
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                acc[y][x] = (acc[y][x] / totalWeight) * amplitude;

                acc[y][x] = std::clamp(acc[y][x], 0.0f, 1.0f);
            }
        }

        return acc;
    }


    // ---------------------------------------------------------
    // Save image
    // ---------------------------------------------------------
    void save_pink_image(
        const std::vector<std::vector<float>>& noise,
        const std::string& filename,
        const std::string& outputDir
    ) {
        if (noise.empty() || noise[0].empty())
            throw std::invalid_argument("Cannot save empty pink noise map.");

        int height = noise.size();
        int width = noise[0].size();

        std::vector<unsigned char> img(width * height);

        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                img[y * width + x] = static_cast<unsigned char>(noise[y][x] * 255.0f);

        std::filesystem::path outDir;

        if (outputDir.empty())
            outDir = std::filesystem::current_path().parent_path() / "ImageOutput";
        else
            outDir = outputDir;

        std::filesystem::create_directories(outDir);

        auto outputFile = outDir / filename;
        std::string ext = outputFile.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        int result = 0;

        if (ext == ".jpg" || ext == ".jpeg")
            result = stbi_write_jpg(outputFile.string().c_str(), width, height, 1, img.data(), 95);
        else
            result = stbi_write_png(outputFile.string().c_str(), width, height, 1, img.data(), width);

        if (result == 0)
            throw std::runtime_error("Failed to write pink noise image: " + outputFile.string());

        std::cout << "[OK] Pink noise saved at: " << outputFile.string() << "\n";
    }


    // ---------------------------------------------------------
    // Wrapper
    // ---------------------------------------------------------
    std::vector<std::vector<float>> create_pinknoise(
        int width,
        int height,
        int octaves,
        float alpha,
        int sampleRate,
        float amplitude,
        int seed,
        OutputMode mode,
        const std::string& filename,
        const std::string& outputDir
    ) {
        auto noise = generate_pink_map(width, height, octaves, alpha, sampleRate, amplitude, seed);

        if (mode == OutputMode::Image)
            save_pink_image(noise, filename, outputDir);

        return noise;
    }

} // namespace Noise
