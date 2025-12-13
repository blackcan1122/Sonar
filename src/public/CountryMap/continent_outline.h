#ifndef CONTINENT_OUTLINES_H
#define CONTINENT_OUTLINES_H
#include <vector>
#include <array>

struct LoopBoundingBox
{
    float MinX = 0.f, MinY = 0.f, MaxX = 0.f, MaxY = 0.f;
};

constexpr LoopBoundingBox ComputeBoundingBox(
    const float* vertices,
    unsigned int offset, 
    unsigned int count) 
{
    LoopBoundingBox box;
    box.MinX = box.MaxX = vertices[offset * 2];
    box.MinY = box.MaxY = vertices[offset * 2 + 1];
    
    for (unsigned int i = 0; i < count; ++i) {
        float x = vertices[(offset + i) * 2];
        float y = vertices[(offset + i) * 2 + 1];
        
        if (x < box.MinX) box.MinX = x;
        if (x > box.MaxX) box.MaxX = x;
        if (y < box.MinY) box.MinY = y;
        if (y > box.MaxY) box.MaxY = y;
    }
    
    return box;
}

template<std::size_t NumLoops>
std::array<LoopBoundingBox, NumLoops> ComputeAllBoundingBoxes(
    const float* vertices,
    const unsigned int* offsets,
    const unsigned int* counts)
{
    std::array<LoopBoundingBox, NumLoops> boxes{};
    for (std::size_t i = 0; i < NumLoops; ++i) {
        boxes[i] = ComputeBoundingBox(vertices, offsets[i], counts[i]);
    }
    return boxes;
}

// Generated continent outlines (extern declarations)
extern const std::vector<float> AsiaVertices;
extern const std::vector<unsigned int> AsiaOffsets;
extern const std::vector<unsigned int> AsiaCounts;
extern const std::vector<float> SouthAmericaVertices;
extern const std::vector<unsigned int> SouthAmericaOffsets;
extern const std::vector<unsigned int> SouthAmericaCounts;
extern const std::vector<float> AfricaVertices;
extern const std::vector<unsigned int> AfricaOffsets;
extern const std::vector<unsigned int> AfricaCounts;
extern const std::vector<float> EuropeVertices;
extern const std::vector<unsigned int> EuropeOffsets;
extern const std::vector<unsigned int> EuropeCounts;
extern const std::vector<float> NorthAmericaVertices;
extern const std::vector<unsigned int> NorthAmericaOffsets;
extern const std::vector<unsigned int> NorthAmericaCounts;
extern const std::vector<float> OceaniaVertices;
extern const std::vector<unsigned int> OceaniaOffsets;
extern const std::vector<unsigned int> OceaniaCounts;
extern const std::vector<float> AntarcticaVertices;
extern const std::vector<unsigned int> AntarcticaOffsets;
extern const std::vector<unsigned int> AntarcticaCounts;
extern const std::vector<float> SevenseasVertices;
extern const std::vector<unsigned int> SevenseasOffsets;
extern const std::vector<unsigned int> SevenseasCounts;

// Lazy initialization functions - thread-safe since C++11
inline const std::array<LoopBoundingBox, 858>& GetAsiaBB() {
    static const auto bb = ComputeAllBoundingBoxes<858>(
        AsiaVertices.data(), AsiaOffsets.data(), AsiaCounts.data());
    return bb;
}

inline const std::array<LoopBoundingBox, 271>& GetSouthAmericaBB() {
    static const auto bb = ComputeAllBoundingBoxes<271>(
        SouthAmericaVertices.data(), SouthAmericaOffsets.data(), SouthAmericaCounts.data());
    return bb;
}

inline const std::array<LoopBoundingBox, 70>& GetAfricaBB() {
    static const auto bb = ComputeAllBoundingBoxes<70>(
        AfricaVertices.data(), AfricaOffsets.data(), AfricaCounts.data());
    return bb;
}

inline const std::array<LoopBoundingBox, 586>& GetEuropeBB() {
    static const auto bb = ComputeAllBoundingBoxes<586>(
        EuropeVertices.data(), EuropeOffsets.data(), EuropeCounts.data());
    return bb;
}

inline const std::array<LoopBoundingBox, 1297>& GetNorthAmericaBB() {
    static const auto bb = ComputeAllBoundingBoxes<1297>(
        NorthAmericaVertices.data(), NorthAmericaOffsets.data(), NorthAmericaCounts.data());
    return bb;
}

inline const std::array<LoopBoundingBox, 269>& GetOceaniaBB() {
    static const auto bb = ComputeAllBoundingBoxes<269>(
        OceaniaVertices.data(), OceaniaOffsets.data(), OceaniaCounts.data());
    return bb;
}

inline const std::array<LoopBoundingBox, 280>& GetAntarcticaBB() {
    static const auto bb = ComputeAllBoundingBoxes<280>(
        AntarcticaVertices.data(), AntarcticaOffsets.data(), AntarcticaCounts.data());
    return bb;
}

inline const std::array<LoopBoundingBox, 430>& GetSevenseasBB() {
    static const auto bb = ComputeAllBoundingBoxes<430>(
        SevenseasVertices.data(), SevenseasOffsets.data(), SevenseasCounts.data());
    return bb;
}
    
#endif // CONTINENT_OUTLINES_H