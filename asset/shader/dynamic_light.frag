#version 330 core

struct Line
{
    int x1, y1, x2, y2;
};

uniform LineData
{
    Line lines[4096];
};

uniform ivec2 light;
uniform vec3 lightcolor;
//uniform int viewport_width;
uniform float power;
uniform ivec2 range;

out vec4 color;

bool intersects(ivec2 a1, ivec2 a2, ivec2 b1, ivec2 b2)
{
    bool side1 = (b2.x - b1.x) * (a1.y - b1.y) - (b2.y - b1.y) * (a1.x - b1.x) > 0;
    bool side2 = (b2.x - b1.x) * (a2.y - b1.y) - (b2.y - b1.y) * (a2.x - b1.x) > 0;
    bool side3 = (a2.x - a1.x) * (b1.y - a1.y) - (a2.y - a1.y) * (b1.x - a1.x) > 0;
    bool side4 = (a2.x - a1.x) * (b2.y - a1.y) - (a2.y - a1.y) * (b2.x - a1.x) > 0;
    return side1 != side2 && side3 != side4;
}

void main()
{
    for (int i = range.x; i < range.x + range.y; ++i)
    {
        if (intersects(ivec2(lines[i].x1, lines[i].y1), ivec2(lines[i].x2, lines[i].y2), light, ivec2(gl_FragCoord.xy)))
            discard;
    }

    float distance = distance(gl_FragCoord.xy, light);
    float intensity = power / (1.0 + (distance * distance));

    color = vec4(intensity * lightcolor.r, intensity * lightcolor.g, intensity * lightcolor.b, 1.0);
}
