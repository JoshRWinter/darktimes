#version 330 core

struct Line
{
    int x1, y1, x2, y2;
};

uniform LineData
{
    Line lines[300];
};

uniform ivec2 light;
uniform vec3 lightcolor;
//uniform int viewport_width;
uniform float power;
uniform ivec2 range;

out vec4 color;

bool intersects(ivec2 a1, ivec2 a2, ivec2 b1, ivec2 b2)
{
    float poopoo = b2.x - b1.x;
    float peepee = b2.y - b1.y;
    float doodoo = a2.x - a1.x;
    float weewee = a2.y - a1.y;

    bool side1 = poopoo * (a1.y - b1.y) - peepee * (a1.x - b1.x) > 0;
    bool side2 = poopoo * (a2.y - b1.y) - peepee * (a2.x - b1.x) > 0;
    bool side3 = doodoo * (b1.y - a1.y) - weewee * (b1.x - a1.x) > 0;
    bool side4 = doodoo * (b2.y - a1.y) - weewee * (b2.x - a1.x) > 0;

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
