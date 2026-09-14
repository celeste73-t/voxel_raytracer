#version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout (r8ui, binding = 0) uniform writeonly uimage3D voxel_image;
layout (binding = 1, offset = 0) uniform atomic_uint occupied_voxels;

const float NOISE_SCALE = 0.05;
const float VOXEL_THRESHOLD = 0.0;

float hash(vec3 p) {
    p = fract(p * 0.3183099 + vec3(0.1, 0.2, 0.3));
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

vec3 gradient(vec3 cell) {
    float angle_a = hash(cell) * 6.2831853;
    float angle_b = hash(cell + 19.19) * 3.1415926;
    return vec3(
        cos(angle_a) * sin(angle_b),
        sin(angle_a) * sin(angle_b),
        cos(angle_b)
    );
}

float perlin_noise(vec3 position) {
    vec3 cell = floor(position);
    vec3 local = fract(position);
    vec3 fade = local * local * (3.0 - 2.0 * local);

    float n000 = dot(gradient(cell + vec3(0.0, 0.0, 0.0)), local - vec3(0.0, 0.0, 0.0));
    float n100 = dot(gradient(cell + vec3(1.0, 0.0, 0.0)), local - vec3(1.0, 0.0, 0.0));
    float n010 = dot(gradient(cell + vec3(0.0, 1.0, 0.0)), local - vec3(0.0, 1.0, 0.0));
    float n110 = dot(gradient(cell + vec3(1.0, 1.0, 0.0)), local - vec3(1.0, 1.0, 0.0));
    float n001 = dot(gradient(cell + vec3(0.0, 0.0, 1.0)), local - vec3(0.0, 0.0, 1.0));
    float n101 = dot(gradient(cell + vec3(1.0, 0.0, 1.0)), local - vec3(1.0, 0.0, 1.0));
    float n011 = dot(gradient(cell + vec3(0.0, 1.0, 1.0)), local - vec3(0.0, 1.0, 1.0));
    float n111 = dot(gradient(cell + vec3(1.0, 1.0, 1.0)), local - vec3(1.0, 1.0, 1.0));

    float x00 = mix(n000, n100, fade.x);
    float x10 = mix(n010, n110, fade.x);
    float x01 = mix(n001, n101, fade.x);
    float x11 = mix(n011, n111, fade.x);
    return mix(mix(x00, x10, fade.y), mix(x01, x11, fade.y), fade.z);
}

void main() {
    ivec3 coordinate = ivec3(gl_GlobalInvocationID.xyz);
    ivec3 dimensions = imageSize(voxel_image);
    if (any(greaterThanEqual(coordinate, dimensions))) {
        return;
    }

    vec3 position = vec3(coordinate) * NOISE_SCALE;
    float value = perlin_noise(position);
    uint voxel = value >= VOXEL_THRESHOLD ? 1u : 0u;
    imageStore(voxel_image, coordinate, uvec4(voxel, 0u, 0u, 1u));
    if (voxel != 0u) {
        atomicCounterIncrement(occupied_voxels);
    }
}
