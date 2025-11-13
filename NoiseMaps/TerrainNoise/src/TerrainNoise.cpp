// TerrainNoise.cpp
#include "Noise.hpp"
#include "TerrainNoise.hpp"
#include "PerlinNoise.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include "stb_image_write.h"

namespace Noise {

    // ---------------------------------------------------------
    // Preset configurations
    // ---------------------------------------------------------
    TerrainParams TerrainParams::preset_rolling_hills() {
        TerrainParams params;
        params.scale = 120.0f;
        params.octaves = 3;
        params.persistence = 0.5f;
        params.lacunarity = 2.0f;
        params.baseHeight = 0.5f;
        params.amplitude = 0.25f;
        params.minHeight = 0.3f;
        params.maxHeight = 0.75f;
        params.maxSlope = 0.08f;
        params.enablePlateau = false;
        return params;
    }

    TerrainParams TerrainParams::preset_mountainous() {
        TerrainParams params;
        params.scale = 80.0f;
        params.octaves = 5;
        params.persistence = 0.6f;
        params.lacunarity = 2.2f;
        params.baseHeight = 0.45f;
        params.amplitude = 0.4f;
        params.minHeight = 0.2f;
        params.maxHeight = 0.9f;
        params.maxSlope = 0.15f;
        params.enablePlateau = false;
        return params;
    }

    TerrainParams TerrainParams::preset_gentle_plains() {
        TerrainParams params;
        params.scale = 200.0f;
        params.octaves = 2;
        params.persistence = 0.4f;
        params.lacunarity = 2.0f;
        params.baseHeight = 0.5f;
        params.amplitude = 0.15f;
        params.minHeight = 0.4f;
        params.maxHeight = 0.65f;
        params.maxSlope = 0.05f;
        params.enablePlateau = false;
        return params;
    }

    TerrainParams TerrainParams::preset_steep_cliffs() {
        TerrainParams params;
        params.scale = 60.0f;
        params.octaves = 4;
        params.persistence = 0.65f;
        params.lacunarity = 2.5f;
        params.baseHeight = 0.5f;
        params.amplitude = 0.35f;
        params.minHeight = 0.15f;
        params.maxHeight = 0.85f;
        params.maxSlope = 0.25f;
        params.enablePlateau = false;
        return params;
    }

    TerrainParams TerrainParams::preset_plateaus() {
        TerrainParams params;
        params.scale = 100.0f;
        params.octaves = 4;
        params.persistence = 0.5f;
        params.lacunarity = 2.0f;
        params.baseHeight = 0.5f;
        params.amplitude = 0.3f;
        params.minHeight = 0.25f;
        params.maxHeight = 0.8f;
        params.maxSlope = 0.1f;
        params.enablePlateau = true;
        params.plateauThreshold = 0.65f;
        params.plateauWidth = 0.08f;
        return params;
    }

    // ---------------------------------------------------------
    // Sample terrain height at single X coordinate
    // ---------------------------------------------------------
    float sample_terrain(float x, const TerrainParams& params) {
        // Use Perlin noise for smooth terrain
        float noise = sample_perlin(
            x, 0.0f,  // Y coordinate doesn't matter for 1D terrain
            params.scale,
            params.octaves,
            1.0f,  // frequency
            params.persistence,
            params.lacunarity,
            0.0f,  // base
            params.seed
        );

        // Apply amplitude and base height
        float height = params.baseHeight + (noise - 0.5f) * params.amplitude * 2.0f;

        // Apply plateau effect if enabled
        if (params.enablePlateau && height > params.plateauThreshold) {
            float overshoot = height - params.plateauThreshold;
            float factor = std::min(overshoot / params.plateauWidth, 1.0f);
            height = params.plateauThreshold + overshoot * (1.0f - factor * 0.8f);
        }

        // Clamp to min/max heights
        height = std::clamp(height, params.minHeight, params.maxHeight);

        return height;
    }

    // ---------------------------------------------------------
    // Generate 1D terrain profile
    // ---------------------------------------------------------
    std::vector<float> generate_terrain_profile(
        int width,
        float startX,
        float step,
        const TerrainParams& params
    ) {
        if (width <= 0) {
            throw std::invalid_argument("width must be > 0, got: " + std::to_string(width));
        }
        if (step <= 0.0f) {
            throw std::invalid_argument("step must be > 0, got: " + std::to_string(step));
        }

        std::vector<float> profile(width);

        // Generate raw heights
        for (int i = 0; i < width; ++i) {
            float x = startX + i * step;
            profile[i] = sample_terrain(x, params);
        }

        // Apply slope limiting if specified
        if (params.maxSlope > 0.0f && params.maxSlope < 1.0f) {
            profile = apply_slope_limit(profile, params.maxSlope);
        }

        return profile;
    }

    // ---------------------------------------------------------
    // Apply slope limiting to terrain profile
    // ---------------------------------------------------------
    std::vector<float> apply_slope_limit(
        const std::vector<float>& heights,
        float maxSlope
    ) {
        if (heights.empty()) return heights;
        if (maxSlope <= 0.0f || maxSlope >= 1.0f) return heights;

        std::vector<float> limited = heights;
        int size = static_cast<int>(heights.size());

        // Forward pass - limit upward slopes
        for (int i = 1; i < size; ++i) {
            float diff = limited[i] - limited[i - 1];
            if (diff > maxSlope) {
                limited[i] = limited[i - 1] + maxSlope;
            }
            else if (diff < -maxSlope) {
                limited[i] = limited[i - 1] - maxSlope;
            }
        }

        // Backward pass - smooth out any remaining issues
        for (int i = size - 2; i >= 0; --i) {
            float diff = limited[i] - limited[i + 1];
            if (diff > maxSlope) {
                limited[i] = limited[i + 1] + maxSlope;
            }
            else if (diff < -maxSlope) {
                limited[i] = limited[i + 1] - maxSlope;
            }
        }

        return limited;
    }

    // ---------------------------------------------------------
    // Apply plateau effect to terrain profile
    // ---------------------------------------------------------
    std::vector<float> apply_plateaus(
        const std::vector<float>& heights,
        float threshold,
        float width
    ) {
        if (heights.empty()) return heights;

        std::vector<float> plateaued = heights;

        for (size_t i = 0; i < heights.size(); ++i) {
            if (heights[i] > threshold) {
                float overshoot = heights[i] - threshold;
                float factor = std::min(overshoot / width, 1.0f);
                plateaued[i] = threshold + overshoot * (1.0f - factor * 0.9f);
            }
        }

        return plateaued;
    }

    // ---------------------------------------------------------
    // Generate 2D heightmap from 1D terrain profile
    // ---------------------------------------------------------
    std::vector<std::vector<float>> generate_terrain_heightmap(
        int width,
        int height,
        float startX,
        float step,
        const TerrainParams& params
    ) {
        if (width <= 0) {
            throw std::invalid_argument("width must be > 0, got: " + std::to_string(width));
        }
        if (height <= 0) {
            throw std::invalid_argument("height must be > 0, got: " + std::to_string(height));
        }

        // Generate 1D terrain profile
        std::vector<float> profile = generate_terrain_profile(width, startX, step, params);

        // Create 2D heightmap visualization
        // Each pixel represents distance from terrain surface
        std::vector<std::vector<float>> heightmap(height, std::vector<float>(width, 0.0f));

        for (int x = 0; x < width; ++x) {
            int terrainY = static_cast<int>((1.0f - profile[x]) * height);
            terrainY = std::clamp(terrainY, 0, height - 1);

            for (int y = 0; y < height; ++y) {
                if (y >= terrainY) {
                    // Below terrain - solid
                    heightmap[y][x] = 1.0f;
                }
                else {
                    // Above terrain - empty (gradient for visualization)
                    float dist = static_cast<float>(terrainY - y) / height;
                    heightmap[y][x] = std::max(0.0f, 1.0f - dist * 2.0f);
                }
            }
        }

        return heightmap;
    }

    // ---------------------------------------------------------
    // Save terrain visualization to image
    // ---------------------------------------------------------
    void save_terrain_image(
        const std::vector<std::vector<float>>& heightmap,
        const std::string& filename,
        const std::string& outputDir
    ) {
        if (heightmap.empty() || heightmap[0].empty()) {
            throw std::invalid_argument("Cannot save empty heightmap.");
        }

        int height = static_cast<int>(heightmap.size());
        int width = static_cast<int>(heightmap[0].size());

        std::vector<unsigned char> imgData(width * height);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                imgData[y * width + x] = static_cast<unsigned char>(heightmap[y][x] * 255.0f);
            }
        }

        // Determine output directory
        std::filesystem::path outDir;
        if (outputDir.empty()) {
            outDir = std::filesystem::current_path().parent_path() / "ImageOutput";
        }
        else {
            outDir = outputDir;
        }
        std::filesystem::create_directories(outDir);

        std::filesystem::path outputFile = outDir / filename;
        std::string extension = outputFile.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        int result = 0;
        if (extension == ".jpg" || extension == ".jpeg") {
            result = stbi_write_jpg(outputFile.string().c_str(), width, height, 1, imgData.data(), 90);
        }
        else {
            result = stbi_write_png(outputFile.string().c_str(), width, height, 1, imgData.data(), width);
        }

        if (result == 0) {
            throw std::runtime_error("Failed to write image file: " + outputFile.string());
        }

        std::cout << "[OK] Terrain image saved at: " << outputFile.string() << "\n";
    }

    // ---------------------------------------------------------
    // Wrapper function for easy terrain generation
    // ---------------------------------------------------------
    std::vector<float> create_terrain(
        int width,
        float startX,
        float step,
        const TerrainParams& params,
        OutputMode mode,
        const std::string& filename,
        const std::string& outputDir
    ) {
        auto profile = generate_terrain_profile(width, startX, step, params);

        switch (mode) {
        case OutputMode::Image:
        {
            // Generate 2D visualization
            int imageHeight = 256;  // Default visualization height
            auto heightmap = generate_terrain_heightmap(width, imageHeight, startX, step, params);
            save_terrain_image(heightmap, filename, outputDir);
            break;
        }
        case OutputMode::None:
            // Just return the profile
            break;
        case OutputMode::Map:
            // Print ASCII preview of terrain
            std::cout << "\n[Terrain Profile Preview]\n";
            std::cout << "Width: " << width << ", Start X: " << startX << ", Step: " << step << "\n";
            std::cout << "Height range: [" << params.minHeight << ", " << params.maxHeight << "]\n\n";

            // Print simplified ASCII visualization
            int previewWidth = std::min(width, 80);
            int previewHeight = 20;
            for (int row = 0; row < previewHeight; ++row) {
                float threshold = 1.0f - (float)row / previewHeight;
                for (int col = 0; col < previewWidth; ++col) {
                    int idx = col * width / previewWidth;
                    char c = (profile[idx] >= threshold) ? '#' : ' ';
                    std::cout << c;
                }
                std::cout << "\n";
            }
            break;
        }

        return profile;
    }

} // namespace Noise
