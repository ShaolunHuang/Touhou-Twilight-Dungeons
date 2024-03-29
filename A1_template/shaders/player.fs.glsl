#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform vec2 end_pos;
uniform vec2 scale;
uniform float focus_mode_alpha;
uniform float invul_timer;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
    // Calculate offset based on the desired range
    float offsetX = end_pos.x-scale.x;
    float offsetY = end_pos.y-scale.y;

    // Apply scale and offset to texture coordinates
    vec2 constrainedTexcoord = vec2(texcoord.x * scale.x + offsetX, texcoord.y * scale.y + offsetY);

    // Invulnerability timer alpha
    float invul_alpha = invul_timer <= 0.0 ? 1.0 : 0.5 * (1.0 + cos(invul_timer / 50.0));

    // Sample the texture with constrained coordinates
    if (fcolor.x < 0.0 && fcolor.y < 0.0 && fcolor.z < 0.0) {
        // white color
        color = vec4(1.0, 1.0, 1.0, texture(sampler0, constrainedTexcoord).a * focus_mode_alpha * invul_alpha);
    } else {
        color = vec4(fcolor, focus_mode_alpha * invul_alpha) * texture(sampler0, constrainedTexcoord);
    }
}