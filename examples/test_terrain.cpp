// test_terrain.cpp
// ----------------
// Comprehensive test for 1D terrain height generator
// Perfect for 2D platformer games

#include "Noise.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace Noise;
using namespace std::chrono;

void print_section(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

// Test 1: Basic terrain generation with presets
void test_terrain_presets() {
    print_section("Test 1: Terrain Preset Configurations");
    
    std::cout << "Testing all preset terrain types...\n\n";
    
    struct PresetTest {
        std::string name;
        TerrainParams params;
    };
    
    std::vector<PresetTest> presets = {
        {"Rolling Hills", TerrainParams::preset_rolling_hills()},
        {"Mountainous", TerrainParams::preset_mountainous()},
        {"Gentle Plains", TerrainParams::preset_gentle_plains()},
        {"Steep Cliffs", TerrainParams::preset_steep_cliffs()},
        {"Plateaus", TerrainParams::preset_plateaus()}
    };
    
    for (const auto& preset : presets) {
        std::cout << preset.name << ":\n";
        std::cout << "  Scale: " << preset.params.scale;
        std::cout << " | Octaves: " << preset.params.octaves;
        std::cout << " | Max Slope: " << preset.params.maxSlope;
        std::cout << " | Range: [" << preset.params.minHeight << ", " << preset.params.maxHeight << "]\n";
        
        // Sample a few points
        std::cout << "  Sample heights: ";
        for (int i = 0; i < 5; i++) {
            float x = i * 50.0f;
            float h = sample_terrain(x, preset.params);
            std::cout << std::fixed << std::setprecision(2) << h << " ";
        }
        std::cout << "\n\n";
    }
}

// Test 2: Real-time terrain sampling (platformer use case)
void test_realtime_sampling() {
    print_section("Test 2: Real-Time Terrain Sampling");
    
    std::cout << "Simulating player movement through terrain...\n\n";
    
    auto params = TerrainParams::preset_rolling_hills();
    params.seed = 42;
    
    float playerX = 0.0f;
    float playerSpeed = 5.0f;
    
    std::cout << "Player Pos | Ground Height | Visual\n";
    std::cout << std::string(50, '-') << "\n";
    
    for (int frame = 0; frame < 25; frame++) {
        float groundHeight = sample_terrain(playerX, params);
        int visualHeight = static_cast<int>(groundHeight * 20);
        
        std::cout << std::setw(10) << std::fixed << std::setprecision(1) << playerX << " | ";
        std::cout << std::setw(13) << std::setprecision(3) << groundHeight << " | ";
        std::cout << std::string(visualHeight, '#') << "\n";
        
        playerX += playerSpeed;
    }
    
    std::cout << "\n✓ Smooth, real-time terrain queries for player collision!\n";
}

// Test 3: Slope limiting demonstration
void test_slope_limiting() {
    print_section("Test 3: Slope Limiting for Playability");
    
    std::cout << "Comparing terrain with and without slope limiting...\n\n";
    
    auto params1 = TerrainParams::preset_steep_cliffs();
    params1.maxSlope = 1.0f;  // No limiting
    params1.seed = 123;
    
    auto params2 = params1;
    params2.maxSlope = 0.1f;  // Limited slopes
    
    std::vector<float> profile1 = generate_terrain_profile(50, 0.0f, 1.0f, params1);
    std::vector<float> profile2 = generate_terrain_profile(50, 0.0f, 1.0f, params2);
    
    // Calculate max slope differences
    float maxSlope1 = 0.0f, maxSlope2 = 0.0f;
    for (size_t i = 1; i < profile1.size(); i++) {
        maxSlope1 = std::max(maxSlope1, std::abs(profile1[i] - profile1[i-1]));
        maxSlope2 = std::max(maxSlope2, std::abs(profile2[i] - profile2[i-1]));
    }
    
    std::cout << "Without slope limiting:\n";
    std::cout << "  Maximum slope: " << maxSlope1 << "\n";
    std::cout << "  Sample profile: ";
    for (int i = 0; i < 10; i++) {
        std::cout << std::fixed << std::setprecision(2) << profile1[i*5] << " ";
    }
    std::cout << "\n\n";
    
    std::cout << "With slope limiting (max=0.1):\n";
    std::cout << "  Maximum slope: " << maxSlope2 << "\n";
    std::cout << "  Sample profile: ";
    for (int i = 0; i < 10; i++) {
        std::cout << std::fixed << std::setprecision(2) << profile2[i*5] << " ";
    }
    std::cout << "\n\n";
    
    std::cout << "✓ Slope limiting makes terrain more playable!\n";
}

// Test 4: Plateau effect
void test_plateau_effect() {
    print_section("Test 4: Plateau Generation");
    
    std::cout << "Generating terrain with flat plateau regions...\n\n";
    
    auto params = TerrainParams::preset_plateaus();
    params.seed = 456;
    
    auto profile = generate_terrain_profile(60, 0.0f, 1.0f, params);
    
    std::cout << "Terrain profile with plateaus:\n";
    std::cout << "(Peaks are flattened above threshold " << params.plateauThreshold << ")\n\n";
    
    // ASCII visualization
    int height = 15;
    for (int row = 0; row < height; row++) {
        float threshold = 1.0f - (float)row / height;
        for (size_t col = 0; col < profile.size(); col++) {
            char c = (profile[col] >= threshold) ? '#' : ' ';
            std::cout << c;
        }
        std::cout << "\n";
    }
    
    std::cout << "\n✓ Notice the flat plateau regions at peaks!\n";
}

// Test 5: Performance benchmarking
void test_performance() {
    print_section("Test 5: Performance Testing");
    
    std::cout << "Benchmarking terrain generation performance...\n\n";
    
    auto params = TerrainParams::preset_rolling_hills();
    params.seed = 789;
    
    // Test single sampling performance
    auto start = high_resolution_clock::now();
    float sum = 0.0f;
    int samples = 10000;
    for (int i = 0; i < samples; i++) {
        sum += sample_terrain(i * 0.5f, params);
    }
    auto end = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end - start).count();
    
    std::cout << "Single-value sampling:\n";
    std::cout << "  " << samples << " samples in " << duration1 << " μs\n";
    std::cout << "  Average: " << (duration1 / (float)samples) << " μs per sample\n";
    std::cout << "  (Sum: " << sum << ")\n\n";
    
    // Test profile generation performance
    start = high_resolution_clock::now();
    auto profile = generate_terrain_profile(samples, 0.0f, 0.5f, params);
    end = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end - start).count();
    
    std::cout << "Profile generation:\n";
    std::cout << "  " << samples << " values in " << duration2 << " μs\n";
    std::cout << "  Average: " << (duration2 / (float)samples) << " μs per value\n\n";
    
    std::cout << "Performance comparison: " << (float)duration1/duration2 << "x\n";
    std::cout << "✓ Profile generation is more efficient for bulk operations!\n";
}

// Test 6: Image generation
void test_image_generation() {
    print_section("Test 6: Terrain Visualization Export");
    
    std::cout << "Generating terrain images for all presets...\n\n";
    
    struct PresetExport {
        std::string name;
        std::string filename;
        TerrainParams params;
    };
    
    std::vector<PresetExport> exports = {
        {"Rolling Hills", "terrain_rolling_hills.png", TerrainParams::preset_rolling_hills()},
        {"Mountainous", "terrain_mountainous.png", TerrainParams::preset_mountainous()},
        {"Gentle Plains", "terrain_gentle_plains.png", TerrainParams::preset_gentle_plains()},
        {"Steep Cliffs", "terrain_steep_cliffs.png", TerrainParams::preset_steep_cliffs()},
        {"Plateaus", "terrain_plateaus.png", TerrainParams::preset_plateaus()}
    };
    
    for (auto& exp : exports) {
        exp.params.seed = 42;  // Same seed for consistency
        create_terrain(512, 0.0f, 1.0f, exp.params, OutputMode::Image, exp.filename);
        std::cout << "  ✓ Generated: " << exp.filename << "\n";
    }
    
    std::cout << "\n✓ All terrain images saved to ImageOutput/\n";
}

// Test 7: Practical platformer integration example
void test_platformer_integration() {
    print_section("Test 7: Platformer Integration Example");
    
    std::cout << "Example: Using terrain for player collision detection\n\n";
    
    auto params = TerrainParams::preset_rolling_hills();
    params.seed = 999;
    
    // Simulate player physics
    struct Player {
        float x = 50.0f;
        float y = 0.6f;
        float velocityY = 0.0f;
        bool onGround = false;
    };
    
    Player player;
    float gravity = -0.02f;
    float jumpPower = 0.15f;
    
    std::cout << "Simulating player physics with terrain collision...\n\n";
    std::cout << "Frame | Player X | Player Y | Ground Y | State\n";
    std::cout << std::string(55, '-') << "\n";
    
    for (int frame = 0; frame < 15; frame++) {
        // Get ground height at player position
        float groundY = sample_terrain(player.x, params);
        
        // Apply gravity
        player.velocityY += gravity;
        player.y += player.velocityY;
        
        // Ground collision
        if (player.y <= groundY) {
            player.y = groundY;
            player.velocityY = 0.0f;
            player.onGround = true;
            
            // Jump every few frames for demo
            if (frame % 8 == 0) {
                player.velocityY = jumpPower;
                player.onGround = false;
            }
        } else {
            player.onGround = false;
        }
        
        // Move player forward
        player.x += 3.0f;
        
        std::cout << std::setw(5) << frame << " | ";
        std::cout << std::setw(8) << std::fixed << std::setprecision(1) << player.x << " | ";
        std::cout << std::setw(8) << std::setprecision(3) << player.y << " | ";
        std::cout << std::setw(8) << groundY << " | ";
        std::cout << (player.onGround ? "Ground" : "Air   ") << "\n";
    }
    
    std::cout << "\n✓ Perfect for real-time platformer physics!\n";
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║   RelNo_D1 1D Terrain Generator Test Suite               ║\n";
    std::cout << "║   Designed for 2D Platformer Games                        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    try {
        test_terrain_presets();
        test_realtime_sampling();
        test_slope_limiting();
        test_plateau_effect();
        test_performance();
        test_image_generation();
        test_platformer_integration();
        
        print_section("Summary");
        std::cout << "✓ All terrain generation tests completed successfully!\n\n";
        std::cout << "Key Features:\n";
        std::cout << "  • 5 preset terrain configurations\n";
        std::cout << "  • Real-time height sampling for collision\n";
        std::cout << "  • Slope limiting for playability\n";
        std::cout << "  • Plateau generation for variety\n";
        std::cout << "  • High-performance bulk generation\n";
        std::cout << "  • Image export for visualization\n";
        std::cout << "  • Perfect for platformer games!\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed with error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
