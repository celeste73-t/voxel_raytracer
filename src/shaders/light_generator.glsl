#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer LightBuffer {
    vec4 lights[];
};

uniform usampler3D voxel_texture;
uniform int light_count;
uniform uint seed;

uint hash_uint(uint value) {
    value ^= value >> 16;
    value *= 0x7feb352du;
    value ^= value >> 15;
    value *= 0x846ca68bu;
    value ^= value >> 16;
    return value;
}

uint random_uint(inout uint state) {
    state = hash_uint(state + 0x9e3779b9u);
    return state;
}

bool already_selected(ivec3 candidate, int count) {
    for (int index = 0; index < count; ++index) {
        ivec3 selected = ivec3(floor(lights[index].xyz));
        if (all(equal(candidate, selected))) {
            return true;
        }
    }
    return false;
}

void main() {
    ivec3 dimensions = textureSize(voxel_texture, 0);
    uint state = seed;
    int generated = 0;
    const int MAX_ATTEMPTS = 100000;

    for (int attempt = 0; attempt < MAX_ATTEMPTS && generated < light_count; ++attempt) {
        ivec3 candidate = ivec3(
            int(random_uint(state) % uint(dimensions.x)),
            int(random_uint(state) % uint(dimensions.y)),
            int(random_uint(state) % uint(dimensions.z))
        );

        if (texelFetch(voxel_texture, candidate, 0).r != 0u || already_selected(candidate, generated)) {
            continue;
        }

        lights[generated] = vec4(vec3(candidate) + vec3(0.5), 1.0);
        ++generated;
    }
}
