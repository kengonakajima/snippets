#include <metal_stdlib>
using namespace metal;

struct Vertex {
    float2 position [[attribute(0)]];
    float4 color [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

struct Uniforms {
    float angle;
};

vertex VertexOut vertex_main(Vertex in [[stage_in]],
                             constant Uniforms &uniforms [[buffer(1)]]) {
    float c = cos(uniforms.angle);
    float s = sin(uniforms.angle);

    float2x2 rotation = float2x2(c, -s, s, c);
    float2 rotated = rotation * in.position;

    VertexOut out;
    out.position = float4(rotated, 0.0, 1.0);
    out.color = in.color;
    return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]]) {
    return in.color;
}
