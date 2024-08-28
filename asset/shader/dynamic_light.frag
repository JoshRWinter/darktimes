#version 330 core

out vec4 color;

void main()
{
    ivec2 light = ivec2(960, 540);
    float dist = distance(gl_FragCoord.xy, light);
    float intensity = clamp(360 / (dist * dist), 0.0, 1.0);
    color = vec4(intensity, intensity, intensity, 1.0);
}
