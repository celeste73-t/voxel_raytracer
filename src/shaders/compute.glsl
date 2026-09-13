#version 430

layout (local_size_x = 8, local_size_y = 8) in;

layout (rgba32f, binding = 0) uniform writeonly image2D out_image;

uniform int frame;

void main() {
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);

    vec2 uv = vec2(pix) / vec2(imageSize(out_image));

    vec3 color = vec3(uv, abs(sin(frame * 0.01)));

    imageStore(out_image, pix, vec4(color, 1.0));
}
