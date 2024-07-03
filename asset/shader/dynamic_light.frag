#version 330 core

out vec4 color;

void main()
{
    float intensity = 1.0 - (distance(gl_FragCoord.xy, vec2(960, 540)) / 400.0);
    color = vec4(1.0, 1.0, 0.8, intensity / 1.0);
}
