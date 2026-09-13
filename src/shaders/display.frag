#version 430

out vec4 fragColor;

uniform sampler2D tex;

void main() {
    fragColor = texture(tex, gl_FragCoord.xy / vec2(textureSize(tex, 0)));
}
