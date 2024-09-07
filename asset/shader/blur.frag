#version 330 core

uniform sampler2D tex;
uniform bool horizontal;
in vec2 ftexcoord;
out vec4 frag;

const float weight[] = float[] (0.2, 0.0666, 0.0666, 0.0666, 0.0666, 0.0666, 0.0666);
const int samples = weight.length();

void main()
{
    vec2 offset = 1.0 / textureSize(tex, 0);
    vec3 f = texture(tex, ftexcoord).rgb * weight[0];

    if (horizontal)
    {
        for (int i = 1; i < samples; ++i)
        {
            f += texture(tex, ftexcoord + vec2(offset.x * i, 0.0)).rgb * weight[i];
            f += texture(tex, ftexcoord - vec2(offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < samples; ++i)
        {
            f += texture(tex, ftexcoord + vec2(0.0, offset.y * i)).rgb * weight[i];
            f += texture(tex, ftexcoord - vec2(0.0, offset.y * i)).rgb * weight[i];
        }

        f = pow(f, vec3(1.0 / 2.2));
    }

    frag = vec4(f.rgb, 1.0);
}
