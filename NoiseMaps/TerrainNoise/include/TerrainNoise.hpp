// TerrainNoise.hpp
// ----------------
// 1D terrain height generator specifically designed for 2D platformer games.
// Generates smooth, controllable terrain with hills, valleys, and plateaus.
//
// Usage:
//   #include "Noise.hpp"
//   auto params = Noise::TerrainParams::preset_rolling_hills();
//   float height = Noise::sample_terrain(playerX, params);

#pragma once
#include <vector>
#include <string>

namespace Noise {

    // Forward declare OutputMode (from Noise.hpp)
    enum class OutputMode;

    // Terrain generation parameters
    struct TerrainParams {
        float scale = 100.0f;          // Horizontal scale (larger = wider features)
        int octaves = 4;               // Detail layers (more = more detail)
        float persistence = 0.5f;      // Amplitude decay per octave
        float lacunarity = 2.0f;       // Frequency growth per octave
        float baseHeight = 0.5f;       // Baseline height [0,1]
        float amplitude = 0.3f;        // Vertical variation [0,1]
        float minHeight = 0.2f;        // Minimum terrain height [0,1]
        float maxHeight = 0.8f;        // Maximum terrain height [0,1]
        float maxSlope = 0.1f;         // Maximum slope per unit (0 = flat, 1 = vertical)
        bool enablePlateau = false;    // Create flat plateau regions
        float plateauThreshold = 0.7f; // Height threshold for plateaus
        float plateauWidth = 0.05f;    // Width of plateau flattening
        int seed = -1;                 // Random seed

        // Preset configurations for common terrain types
        static TerrainParams preset_rolling_hills();
        static TerrainParams preset_mountainous();
        static TerrainParams preset_gentle_plains();
        static TerrainParams preset_steep_cliffs();
        static TerrainParams preset_plateaus();
    };

    // Sample terrain height at a single X coordinate
    // Returns normalized height value [0,1]
    float sample_terrain(float x, const TerrainParams& params);

    // Chunk-based generation: Generate 1D terrain chunk
    // Returns array of heights for a specific horizontal chunk
    std::vector<float> generate_terrain_chunk(
        int chunkX,           // Chunk X coordinate (horizontal position)
        int chunkSize,        // Number of terrain samples per chunk
        const TerrainParams& params
    );

    // Generate 1D terrain profile (array of heights)
    // Useful for pre-generating or visualizing terrain sections
    std::vector<float> generate_terrain_profile(
        int width,
        float startX,
        float step,
        const TerrainParams& params
    );

    // Apply slope limiting to smooth out steep sections
    // Makes terrain more playable by ensuring slopes aren't too steep
    std::vector<float> apply_slope_limit(
        const std::vector<float>& heights,
        float maxSlope
    );

    // Apply plateau effect - flattens peaks above threshold
    std::vector<float> apply_plateaus(
        const std::vector<float>& heights,
        float threshold,
        float width
    );

    // Generate 2D heightmap suitable for visualization or collision
    // Each row is the same 1D terrain profile repeated
    std::vector<std::vector<float>> generate_terrain_heightmap(
        int width,
        int height,
        float startX,
        float step,
        const TerrainParams& params
    );

    // Save terrain profile as PNG/JPEG visualization
    // Shows terrain as a grayscale heightmap
    void save_terrain_image(
        const std::vector<std::vector<float>>& heightmap,
        const std::string& filename = "terrain.png",
        const std::string& outputDir = ""
    );

    // Wrapper function for easy terrain generation with image output
    std::vector<float> create_terrain(
        int width,
        float startX,
        float step,
        const TerrainParams& params,
        OutputMode mode = OutputMode::None,
        const std::string& filename = "terrain.png",
        const std::string& outputDir = ""
    );

} // namespace Noise
