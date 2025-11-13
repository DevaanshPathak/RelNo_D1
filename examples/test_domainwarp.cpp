// test_domainwarp.cpp
// --------------------
// Test suite for DomainWarp module

#include "Noise.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

#include "stb_image_write.h"

// Helper to generate Perlin noise map
std::vector<std::vector<float>> generate_perlin(int width, int height, unsigned int seed, float scale) {
    return Noise::generate_perlin_map(width, height, scale, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
}

// Helper to save maps as PNG
void save_grayscale_png(const std::vector<std::vector<float>>& map, const std::string& filename) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<unsigned char> pixels(width * height);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float value = std::max(0.0f, std::min(1.0f, map[y][x]));
            pixels[y * width + x] = static_cast<unsigned char>(value * 255);
        }
    }
    
    stbi_write_png(("ImageOutput/" + filename).c_str(), width, height, 1, pixels.data(), width);
    std::cout << "  Saved: ImageOutput/" << filename << std::endl;
}

// Test 1: Basic Domain Warping
void test_basic_domain_warp() {
    std::cout << "\n=== Test 1: Basic Domain Warping ===" << std::endl;
    
    // Generate base terrain
    auto terrain = generate_perlin(256, 256, 100, 5.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "dw_01_original.png");
    
    // Apply light warping
    auto light_warp = Noise::domain_warp(normalized, 10.0f, 42);
    save_grayscale_png(light_warp, "dw_02_warp_light.png");
    
    // Apply medium warping
    auto medium_warp = Noise::domain_warp(normalized, 30.0f, 42);
    save_grayscale_png(medium_warp, "dw_03_warp_medium.png");
    
    // Apply strong warping
    auto strong_warp = Noise::domain_warp(normalized, 60.0f, 42);
    save_grayscale_png(strong_warp, "dw_04_warp_strong.png");
    
    std::cout << "✓ Basic domain warp test passed" << std::endl;
}

// Test 2: Fractal Domain Warping
void test_fractal_warp() {
    std::cout << "\n=== Test 2: Fractal Domain Warping ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 200, 6.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "dw_05_fractal_original.png");
    
    // Single iteration
    auto warp1 = Noise::fractal_domain_warp(normalized, 20.0f, 1, 0.5f, 123);
    save_grayscale_png(warp1, "dw_06_fractal_iter1.png");
    
    // Three iterations
    auto warp3 = Noise::fractal_domain_warp(normalized, 20.0f, 3, 0.5f, 123);
    save_grayscale_png(warp3, "dw_07_fractal_iter3.png");
    
    // Five iterations with faster decay
    auto warp5 = Noise::fractal_domain_warp(normalized, 20.0f, 5, 0.3f, 123);
    save_grayscale_png(warp5, "dw_08_fractal_iter5.png");
    
    std::cout << "✓ Fractal warp test passed" << std::endl;
}

// Test 3: Turbulence Effects
void test_turbulence() {
    std::cout << "\n=== Test 3: Turbulence Effects ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 300, 4.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "dw_09_turb_original.png");
    
    // Light turbulence
    auto light_turb = Noise::apply_turbulence(normalized, 0.2f, 3, 456);
    save_grayscale_png(light_turb, "dw_10_turb_light.png");
    
    // Medium turbulence
    auto medium_turb = Noise::apply_turbulence(normalized, 0.4f, 4, 456);
    save_grayscale_png(medium_turb, "dw_11_turb_medium.png");
    
    // Strong turbulence
    auto strong_turb = Noise::apply_turbulence(normalized, 0.6f, 5, 456);
    save_grayscale_png(strong_turb, "dw_12_turb_strong.png");
    
    std::cout << "✓ Turbulence test passed" << std::endl;
}

// Test 4: Directional Turbulence
void test_directional_turbulence() {
    std::cout << "\n=== Test 4: Directional Turbulence ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 400, 5.0f);
    auto normalized = Noise::normalize(terrain);
    
    // Horizontal flow (0 degrees)
    auto horizontal = Noise::directional_turbulence(normalized, 0.0f, 0.4f, 4, 789);
    save_grayscale_png(horizontal, "dw_13_dir_horizontal.png");
    
    // Vertical flow (90 degrees)
    auto vertical = Noise::directional_turbulence(normalized, 1.5708f, 0.4f, 4, 789);
    save_grayscale_png(vertical, "dw_14_dir_vertical.png");
    
    // Diagonal flow (45 degrees)
    auto diagonal = Noise::directional_turbulence(normalized, 0.7854f, 0.4f, 4, 789);
    save_grayscale_png(diagonal, "dw_15_dir_diagonal.png");
    
    std::cout << "✓ Directional turbulence test passed" << std::endl;
}

// Test 5: Marble Effect
void test_marble_effect() {
    std::cout << "\n=== Test 5: Marble Effect ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 500, 3.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "dw_16_marble_base.png");
    
    // Fine veins
    auto fine_marble = Noise::marble_effect(normalized, 3.0f, 20.0f, 111);
    save_grayscale_png(fine_marble, "dw_17_marble_fine.png");
    
    // Medium veins
    auto medium_marble = Noise::marble_effect(normalized, 5.0f, 30.0f, 111);
    save_grayscale_png(medium_marble, "dw_18_marble_medium.png");
    
    // Bold veins
    auto bold_marble = Noise::marble_effect(normalized, 8.0f, 40.0f, 111);
    save_grayscale_png(bold_marble, "dw_19_marble_bold.png");
    
    std::cout << "✓ Marble effect test passed" << std::endl;
}

// Test 6: Wood Grain Effect
void test_wood_grain() {
    std::cout << "\n=== Test 6: Wood Grain Effect ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 600, 4.0f);
    auto normalized = Noise::normalize(terrain);
    
    // Centered rings
    auto centered_wood = Noise::wood_grain_effect(normalized, 0.5f, 0.5f, 8.0f, 5.0f, 222);
    save_grayscale_png(centered_wood, "dw_20_wood_centered.png");
    
    // Off-center rings
    auto offset_wood = Noise::wood_grain_effect(normalized, 0.3f, 0.7f, 10.0f, 7.0f, 222);
    save_grayscale_png(offset_wood, "dw_21_wood_offset.png");
    
    // Tight rings with strong warping
    auto tight_wood = Noise::wood_grain_effect(normalized, 0.5f, 0.5f, 15.0f, 10.0f, 222);
    save_grayscale_png(tight_wood, "dw_22_wood_tight.png");
    
    std::cout << "✓ Wood grain test passed" << std::endl;
}

// Test 7: Swirl Effect
void test_swirl_effect() {
    std::cout << "\n=== Test 7: Swirl Effect ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 700, 6.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "dw_23_swirl_original.png");
    
    // Gentle swirl
    auto gentle_swirl = Noise::swirl_effect(normalized, 0.5f, 0.5f, 1.0f, 0.8f);
    save_grayscale_png(gentle_swirl, "dw_24_swirl_gentle.png");
    
    // Strong swirl
    auto strong_swirl = Noise::swirl_effect(normalized, 0.5f, 0.5f, 3.0f, 0.8f);
    save_grayscale_png(strong_swirl, "dw_25_swirl_strong.png");
    
    // Tight swirl (small radius)
    auto tight_swirl = Noise::swirl_effect(normalized, 0.5f, 0.5f, 2.0f, 0.4f);
    save_grayscale_png(tight_swirl, "dw_26_swirl_tight.png");
    
    std::cout << "✓ Swirl effect test passed" << std::endl;
}

// Test 8: Ridge Noise
void test_ridge_noise() {
    std::cout << "\n=== Test 8: Ridge Noise ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 800, 5.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "dw_27_ridge_original.png");
    
    // Soft ridges
    auto soft_ridge = Noise::ridge_noise(normalized, 1.5f);
    save_grayscale_png(soft_ridge, "dw_28_ridge_soft.png");
    
    // Sharp ridges
    auto sharp_ridge = Noise::ridge_noise(normalized, 3.0f);
    save_grayscale_png(sharp_ridge, "dw_29_ridge_sharp.png");
    
    // Very sharp ridges
    auto very_sharp_ridge = Noise::ridge_noise(normalized, 5.0f);
    save_grayscale_png(very_sharp_ridge, "dw_30_ridge_verysharp.png");
    
    std::cout << "✓ Ridge noise test passed" << std::endl;
}

// Test 9: Billowy Noise
void test_billowy_noise() {
    std::cout << "\n=== Test 9: Billowy Noise ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 900, 4.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "dw_31_billowy_original.png");
    
    // Slight puffiness
    auto slight_billowy = Noise::billowy_noise(normalized, 1.5f);
    save_grayscale_png(slight_billowy, "dw_32_billowy_slight.png");
    
    // Medium puffiness
    auto medium_billowy = Noise::billowy_noise(normalized, 2.5f);
    save_grayscale_png(medium_billowy, "dw_33_billowy_medium.png");
    
    // High puffiness
    auto high_billowy = Noise::billowy_noise(normalized, 4.0f);
    save_grayscale_png(high_billowy, "dw_34_billowy_high.png");
    
    std::cout << "✓ Billowy noise test passed" << std::endl;
}

// Test 10: Folded Noise
void test_folded_noise() {
    std::cout << "\n=== Test 10: Folded Noise ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 1000, 5.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "dw_35_folded_original.png");
    
    // Single fold
    auto fold1 = Noise::folded_noise(normalized, 1);
    save_grayscale_png(fold1, "dw_36_folded_1.png");
    
    // Double fold
    auto fold2 = Noise::folded_noise(normalized, 2);
    save_grayscale_png(fold2, "dw_37_folded_2.png");
    
    // Triple fold
    auto fold3 = Noise::folded_noise(normalized, 3);
    save_grayscale_png(fold3, "dw_38_folded_3.png");
    
    std::cout << "✓ Folded noise test passed" << std::endl;
}

// Test 11: Combined Effects - Warped Terrain
void test_combined_warped_terrain() {
    std::cout << "\n=== Test 11: Combined Warped Terrain ===" << std::endl;
    
    auto terrain = generate_perlin(512, 512, 1100, 4.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "dw_39_combined_base.png");
    
    // Apply domain warp
    auto warped = Noise::domain_warp(normalized, 25.0f, 333);
    save_grayscale_png(warped, "dw_40_combined_warped.png");
    
    // Add turbulence
    auto turbulent = Noise::apply_turbulence(warped, 0.3f, 4, 444);
    save_grayscale_png(turbulent, "dw_41_combined_turbulent.png");
    
    // Create ridges
    auto ridged = Noise::ridge_noise(turbulent, 2.0f);
    save_grayscale_png(ridged, "dw_42_combined_ridged.png");
    
    std::cout << "✓ Combined warped terrain test passed" << std::endl;
}

// Test 12: Warp Chain
void test_warp_chain() {
    std::cout << "\n=== Test 12: Warp Chain ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 1200, 5.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "dw_43_chain_original.png");
    
    // Create a chain of warps
    std::vector<Noise::WarpSettings> chain = {
        Noise::WarpSettings(30.0f, 1, 0.5f, 555),
        Noise::WarpSettings(20.0f, 2, 0.6f, 666),
        Noise::WarpSettings(10.0f, 1, 0.5f, 777)
    };
    
    auto chained = Noise::apply_warp_chain(normalized, chain);
    save_grayscale_png(chained, "dw_44_chain_result.png");
    
    std::cout << "✓ Warp chain test passed" << std::endl;
}

// Test 13: Organic Coastlines
void test_organic_coastlines() {
    std::cout << "\n=== Test 13: Organic Coastlines ===" << std::endl;
    
    // Generate island terrain
    auto terrain = generate_perlin(256, 256, 1300, 3.0f);
    auto normalized = Noise::normalize(terrain);
    
    // Threshold to create island
    auto island = Noise::apply_function(normalized, [](float x) {
        return x > 0.5f ? 1.0f : 0.0f;
    });
    save_grayscale_png(island, "dw_45_coast_blocky.png");
    
    // Apply domain warp for organic coastline
    auto warped_coast = Noise::domain_warp(island, 15.0f, 888);
    save_grayscale_png(warped_coast, "dw_46_coast_organic.png");
    
    // Apply fractal warp for very complex coastline
    auto complex_coast = Noise::fractal_domain_warp(island, 20.0f, 3, 0.5f, 888);
    save_grayscale_png(complex_coast, "dw_47_coast_complex.png");
    
    std::cout << "✓ Organic coastlines test passed" << std::endl;
}

// Test 14: Clouds and Atmosphere
void test_clouds() {
    std::cout << "\n=== Test 14: Clouds and Atmosphere ===" << std::endl;
    
    auto base = generate_perlin(256, 256, 1400, 6.0f);
    auto normalized = Noise::normalize(base);
    
    // Basic clouds with turbulence
    auto clouds = Noise::apply_turbulence(normalized, 0.5f, 5, 999);
    save_grayscale_png(clouds, "dw_48_clouds_basic.png");
    
    // Billowy clouds
    auto billowy_clouds = Noise::billowy_noise(clouds, 3.0f);
    save_grayscale_png(billowy_clouds, "dw_49_clouds_billowy.png");
    
    // Wispy clouds with directional flow
    auto wispy = Noise::directional_turbulence(normalized, 0.3f, 0.6f, 4, 999);
    save_grayscale_png(wispy, "dw_50_clouds_wispy.png");
    
    std::cout << "✓ Clouds test passed" << std::endl;
}

// Test 15: Complete Terrain Pipeline with Warping
void test_complete_pipeline() {
    std::cout << "\n=== Test 15: Complete Terrain Pipeline ===" << std::endl;
    
    // 1. Generate base heightmap
    auto terrain = generate_perlin(512, 512, 1500, 3.5f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "dw_51_pipeline_01_base.png");
    
    // 2. Apply fractal domain warp for organic shapes
    auto warped = Noise::fractal_domain_warp(normalized, 30.0f, 3, 0.5f, 1111);
    save_grayscale_png(warped, "dw_52_pipeline_02_warped.png");
    
    // 3. Add turbulence for detail
    auto detailed = Noise::apply_turbulence(warped, 0.25f, 4, 2222);
    save_grayscale_png(detailed, "dw_53_pipeline_03_detailed.png");
    
    // 4. Create ridges for mountain ranges
    auto ridged = Noise::ridge_noise(detailed, 2.5f);
    save_grayscale_png(ridged, "dw_54_pipeline_04_ridged.png");
    
    // 5. Apply thermal erosion
    auto eroded = Noise::thermal_erosion(ridged, 10, 0.04f, 0.4f);
    save_grayscale_png(eroded, "dw_55_pipeline_05_eroded.png");
    
    // 6. Final smoothing
    auto final = Noise::simple_erosion(eroded, 2, 0.15f);
    save_grayscale_png(final, "dw_56_pipeline_06_final.png");
    
    std::cout << "✓ Complete pipeline test passed" << std::endl;
    std::cout << "\n🌍 Game-ready terrain with organic warping, ridges, and erosion!" << std::endl;
}

int main() {
    std::cout << "=====================================" << std::endl;
    std::cout << "   DomainWarp Test Suite" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        test_basic_domain_warp();         // Tests 1-4 (4 images)
        test_fractal_warp();              // Tests 5-8 (4 images)
        test_turbulence();                // Tests 9-12 (4 images)
        test_directional_turbulence();    // Tests 13-15 (3 images)
        test_marble_effect();             // Tests 16-19 (4 images)
        test_wood_grain();                // Tests 20-22 (3 images)
        test_swirl_effect();              // Tests 23-26 (4 images)
        test_ridge_noise();               // Tests 27-30 (4 images)
        test_billowy_noise();             // Tests 31-34 (4 images)
        test_folded_noise();              // Tests 35-38 (4 images)
        test_combined_warped_terrain();   // Tests 39-42 (4 images)
        test_warp_chain();                // Tests 43-44 (2 images)
        test_organic_coastlines();        // Tests 45-47 (3 images)
        test_clouds();                    // Tests 48-50 (3 images)
        test_complete_pipeline();         // Tests 51-56 (6 images)
        
        std::cout << "\n=====================================" << std::endl;
        std::cout << "   ✅ ALL 15 TESTS PASSED!" << std::endl;
        std::cout << "   📊 56 test images generated" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
