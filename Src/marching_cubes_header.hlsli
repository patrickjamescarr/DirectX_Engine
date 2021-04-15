struct tri {
    float3 p[3];
};

struct gridCell {
    float3 p[8];
    float3 normal[8];
    float densityValue[8];
};