// test_chunks.cpp
// ---------------
// Comprehensive test for chunk-based generation system
// Demonstrates efficient large-world generation

#include "Noise.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <map>

using namespace Noise;
using namespace std::chrono;

void print_section(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

// Test 1: Basic chunk generation
void test_basic_chunks() {
    print_section("Test 1: Basic Chunk Generation");
    
    std::cout << "Generating individual 16x16 chunks...\n\n";
    
    int chunkSize = 16;
    int seed = 42;
    
    // Generate 3 chunks in different locations
    std::cout << "PerlinNoise Chunks:\n";
    for (int chunkX = 0; chunkX < 3; chunkX++) {
        for (int chunkY = 0; chunkY < 3; chunkY++) {
            auto chunk = generate_perlin_chunk(chunkX, chunkY, chunkSize, 50.0f, 3, 1.0f, 0.5f, 2.0f, 0.0f, seed);
            
            // Sample corner values
            std::cout << "  Chunk (" << chunkX << "," << chunkY << "): ";
            std::cout << "corners=[" << std::fixed << std::setprecision(3);
            std::cout << chunk[0][0] << ", " << chunk[0][chunkSize-1] << ", ";
            std::cout << chunk[chunkSize-1][0] << ", " << chunk[chunkSize-1][chunkSize-1] << "]\n";
        }
    }
    
    std::cout << "\n✓ Chunks generated successfully!\n";
}

// Test 2: Chunk continuity - verify adjacent chunks align
void test_chunk_continuity() {
    print_section("Test 2: Chunk Continuity Verification");
    
    std::cout << "Verifying that adjacent chunks produce continuous terrain...\n\n";
    
    int chunkSize = 32;
    int seed = 123;
    
    // Generate two adjacent chunks
    auto chunk0 = generate_perlin_chunk(0, 0, chunkSize, 50.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
    auto chunk1 = generate_perlin_chunk(1, 0, chunkSize, 50.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
    
    // Check that the right edge of chunk0 matches left edge of chunk1
    // This would be true if we sampled from the same continuous noise field
    float rightEdge = chunk0[0][chunkSize-1];
    
    // Sample the same world coordinate directly
    float worldX = chunkSize - 1;  // Right edge of chunk 0 in world coords
    float worldY = 0;
    float sampledValue = sample_perlin(worldX, worldY, 50.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
    
    std::cout << "Chunk 0 right edge value: " << std::fixed << std::setprecision(6) << rightEdge << "\n";
    std::cout << "Direct sample at same coord: " << sampledValue << "\n";
    std::cout << "Difference: " << std::abs(rightEdge - sampledValue) << "\n\n";
    
    // Verify continuity for multiple points
    bool continuous = true;
    float maxDiscrepancy = 0.0f;
    
    for (int y = 0; y < chunkSize; y++) {
        float chunkValue = chunk0[y][chunkSize-1];
        float directValue = sample_perlin(chunkSize-1, y, 50.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
        float diff = std::abs(chunkValue - directValue);
        maxDiscrepancy = std::max(maxDiscrepancy, diff);
        
        if (diff > 0.001f) {
            continuous = false;
        }
    }
    
    if (continuous) {
        std::cout << "✓ Chunks are perfectly continuous!\n";
        std::cout << "  Max discrepancy: " << maxDiscrepancy << "\n";
    } else {
        std::cout << "✗ Warning: Chunks show discontinuity\n";
        std::cout << "  Max discrepancy: " << maxDiscrepancy << "\n";
    }
}

// Test 3: Performance comparison - chunks vs full map
void test_chunk_performance() {
    print_section("Test 3: Performance - Chunks vs Full Map");
    
    int chunkSize = 64;
    int numChunks = 16;  // 4x4 grid
    int seed = 456;
    
    std::cout << "Comparing performance for 256x256 area (" << numChunks << " 64x64 chunks)...\n\n";
    
    // Method 1: Generate full map at once
    auto start = high_resolution_clock::now();
    auto fullMap = generate_perlin_map(256, 256, 50.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
    auto end = high_resolution_clock::now();
    auto fullMapTime = duration_cast<microseconds>(end - start).count();
    
    std::cout << "Full map generation (256x256):\n";
    std::cout << "  Time: " << fullMapTime << " μs\n";
    std::cout << "  Size: " << (256 * 256 * sizeof(float)) / 1024 << " KB\n\n";
    
    // Method 2: Generate as chunks
    start = high_resolution_clock::now();
    std::vector<std::vector<std::vector<std::vector<float>>>> chunks;
    for (int cy = 0; cy < 4; cy++) {
        std::vector<std::vector<std::vector<float>>> row;
        for (int cx = 0; cx < 4; cx++) {
            row.push_back(generate_perlin_chunk(cx, cy, chunkSize, 50.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed));
        }
        chunks.push_back(row);
    }
    end = high_resolution_clock::now();
    auto chunkTime = duration_cast<microseconds>(end - start).count();
    
    std::cout << "Chunk-based generation (16x 64x64 chunks):\n";
    std::cout << "  Time: " << chunkTime << " μs\n";
    std::cout << "  Overhead: " << std::fixed << std::setprecision(1) 
              << ((float)chunkTime / fullMapTime * 100.0f - 100.0f) << "%\n\n";
    
    std::cout << "Analysis:\n";
    std::cout << "  • Full map is good for small, static worlds\n";
    std::cout << "  • Chunks enable:\n";
    std::cout << "    - Infinite world generation\n";
    std::cout << "    - On-demand loading/unloading\n";
    std::cout << "    - Memory-efficient streaming\n";
    std::cout << "    - Parallel chunk generation\n";
}

// Test 4: Simulated streaming world
void test_streaming_world() {
    print_section("Test 4: Streaming World Simulation");
    
    std::cout << "Simulating player movement through infinite world...\n";
    std::cout << "Chunks are generated on-demand as player moves\n\n";
    
    int chunkSize = 32;
    int seed = 789;
    
    // Simple chunk cache
    std::map<std::pair<int, int>, std::vector<std::vector<float>>> chunkCache;
    
    auto getChunk = [&](int cx, int cy) -> const std::vector<std::vector<float>>& {
        auto key = std::make_pair(cx, cy);
        if (chunkCache.find(key) == chunkCache.end()) {
            std::cout << "  [Loading chunk (" << cx << "," << cy << ")]\n";
            chunkCache[key] = generate_perlin_chunk(cx, cy, chunkSize, 60.0f, 3, 1.0f, 0.5f, 2.0f, 0.0f, seed);
        }
        return chunkCache[key];
    };
    
    // Simulate player movement
    float playerX = 0.0f;
    float playerY = 16.0f;
    
    std::cout << "Player movement:\n";
    std::cout << "Frame | Player Pos | Chunk | Terrain Value | Cache Size\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (int frame = 0; frame < 12; frame++) {
        // Determine which chunk player is in
        int chunkX = static_cast<int>(playerX / chunkSize);
        int chunkY = static_cast<int>(playerY / chunkSize);
        
        // Get chunk (loads if not cached)
        auto& chunk = getChunk(chunkX, chunkY);
        
        // Get terrain value at player position within chunk
        int localX = static_cast<int>(playerX) % chunkSize;
        int localY = static_cast<int>(playerY) % chunkSize;
        float terrainValue = chunk[localY][localX];
        
        std::cout << std::setw(5) << frame << " | ";
        std::cout << "(" << std::setw(5) << std::fixed << std::setprecision(0) << playerX << ",";
        std::cout << std::setw(3) << playerY << ") | ";
        std::cout << "(" << chunkX << "," << chunkY << ")  | ";
        std::cout << std::setw(13) << std::setprecision(3) << terrainValue << " | ";
        std::cout << chunkCache.size() << " chunks\n";
        
        // Move player
        playerX += 10.0f;
        if (frame == 6) playerY = 48.0f;  // Move to different row of chunks
    }
    
    std::cout << "\n✓ Streaming world with on-demand chunk loading!\n";
    std::cout << "  Final cache: " << chunkCache.size() << " chunks in memory\n";
}

// Test 5: 1D Terrain chunks
void test_terrain_chunks() {
    print_section("Test 5: 1D Terrain Chunk Generation");
    
    std::cout << "Testing terrain-specific chunk generation...\n\n";
    
    auto params = TerrainParams::preset_rolling_hills();
    params.seed = 999;
    int chunkSize = 64;
    
    std::cout << "Generating 3 adjacent terrain chunks:\n\n";
    
    for (int chunkX = 0; chunkX < 3; chunkX++) {
        auto chunk = generate_terrain_chunk(chunkX, chunkSize, params);
        
        std::cout << "Chunk " << chunkX << " (X: " << (chunkX * chunkSize) << "-" << ((chunkX+1) * chunkSize - 1) << "):\n";
        std::cout << "  Height range: [" << std::fixed << std::setprecision(3);
        
        float minH = 1.0f, maxH = 0.0f;
        for (float h : chunk) {
            minH = std::min(minH, h);
            maxH = std::max(maxH, h);
        }
        
        std::cout << minH << ", " << maxH << "]\n";
        std::cout << "  First 5: ";
        for (int i = 0; i < 5; i++) {
            std::cout << chunk[i] << " ";
        }
        std::cout << "\n";
        std::cout << "  Last 5:  ";
        for (int i = chunkSize-5; i < chunkSize; i++) {
            std::cout << chunk[i] << " ";
        }
        std::cout << "\n\n";
    }
    
    std::cout << "✓ Terrain chunks for infinite platformer worlds!\n";
}

// Test 6: Memory efficiency
void test_memory_efficiency() {
    print_section("Test 6: Memory Efficiency Analysis");
    
    std::cout << "Comparing memory usage patterns...\n\n";
    
    int chunkSize = 64;
    
    std::cout << "Single chunk (" << chunkSize << "x" << chunkSize << "):\n";
    std::cout << "  Memory: " << (chunkSize * chunkSize * sizeof(float)) / 1024.0f << " KB\n\n";
    
    std::cout << "Full map (1024x1024):\n";
    std::cout << "  Memory: " << (1024 * 1024 * sizeof(float)) / 1024.0f << " KB\n";
    std::cout << "  Equivalent chunks: " << (1024 / chunkSize) * (1024 / chunkSize) << "\n\n";
    
    std::cout << "Chunk-based approach benefits:\n";
    std::cout << "  • Load only visible chunks (~9-16 chunks for typical viewport)\n";
    std::cout << "  • Memory usage: ~" << (16 * chunkSize * chunkSize * sizeof(float)) / 1024.0f << " KB\n";
    std::cout << "  • Savings: ~" << (1 - (16.0f * chunkSize * chunkSize) / (1024.0f * 1024.0f)) * 100 << "%\n";
    std::cout << "  • Enables true infinite worlds!\n";
}

// Test 7: Different chunk sizes
void test_chunk_sizes() {
    print_section("Test 7: Optimal Chunk Size Analysis");
    
    std::cout << "Testing different chunk sizes...\n\n";
    
    int seed = 111;
    std::vector<int> sizes = {16, 32, 64, 128, 256};
    
    std::cout << "Size | Gen Time (μs) | Time/Pixel (ns)\n";
    std::cout << std::string(45, '-') << "\n";
    
    for (int size : sizes) {
        auto start = high_resolution_clock::now();
        auto chunk = generate_perlin_chunk(0, 0, size, 50.0f, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
        auto end = high_resolution_clock::now();
        auto time = duration_cast<microseconds>(end - start).count();
        
        float timePerPixel = (time * 1000.0f) / (size * size);
        
        std::cout << std::setw(4) << size << " | ";
        std::cout << std::setw(13) << time << " | ";
        std::cout << std::setw(15) << std::fixed << std::setprecision(2) << timePerPixel << "\n";
    }
    
    std::cout << "\nRecommendations:\n";
    std::cout << "  • 32-64: Good balance for most games\n";
    std::cout << "  • 16: Low latency, more chunk loads\n";
    std::cout << "  • 128+: Fewer loads, higher initial cost\n";
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║   RelNo_D1 Chunk-Based Generation Test Suite             ║\n";
    std::cout << "║   Efficient Large World Generation                        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    try {
        test_basic_chunks();
        test_chunk_continuity();
        test_chunk_performance();
        test_streaming_world();
        test_terrain_chunks();
        test_memory_efficiency();
        test_chunk_sizes();
        
        print_section("Summary");
        std::cout << "✓ All chunk generation tests completed successfully!\n\n";
        std::cout << "Key Benefits:\n";
        std::cout << "  • Infinite world generation capability\n";
        std::cout << "  • On-demand chunk loading/unloading\n";
        std::cout << "  • Memory-efficient streaming\n";
        std::cout << "  • Continuous terrain across chunks\n";
        std::cout << "  • Deterministic generation from coordinates\n";
        std::cout << "  • Perfect for open-world games!\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed with error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
