#version 330

// Lighting, part 2 (runs once per pixel). The idea from raylib's lighting
// example: a pixel is lit more when it faces the lamp (diffuse), and a shiny
// surface also shows a bright highlight where the lamp reflects (specular).
// Here the "lamp" is a long pool-table lamp with three warm bulbs that shine
// down in a cone, so the table is bright and the room round it is dark.

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;   // the texture (white for plain shapes)
uniform vec4 colDiffuse;      // the material's colour

uniform vec3 viewPos;         // where the camera is
uniform vec3 lampPos[3];      // the three bulbs
uniform vec3 lampColour;      // warm white
uniform vec3 ambient;         // the dim light that's everywhere in the room
uniform float shine;          // how strong the highlight is (0 = matte, like felt)
uniform float shinePower;     // how small and sharp the highlight is
uniform vec3 fogColour;       // far away things fade into the dark room

out vec4 finalColor;

void main() {
    vec4 base = texture(texture0, fragTexCoord) * colDiffuse * fragColor;
    vec3 normal = normalize(fragNormal);
    vec3 toCamera = normalize(viewPos - fragPosition);

    // Surfaces facing up get a little more of the room's light than ones facing down
    vec3 light = ambient * (0.65 + 0.35 * normal.y);
    vec3 highlight = vec3(0.0);

    for (int i = 0; i < 3; i++) {
        vec3 toLamp = lampPos[i] - fragPosition;
        float distance = length(toLamp);
        vec3 direction = toLamp / distance;

        // The lamp shade only lets light out downwards: full inside 40 degrees, none past 68
        float cone = smoothstep(0.37, 0.77, direction.y);
        float falloff = 1.0 / (1.0 + 0.09 * distance * distance);
        float strength = cone * falloff;

        float facing = max(dot(normal, direction), 0.0);   // diffuse
        light += lampColour * facing * strength;

        vec3 halfway = normalize(direction + toCamera);    // specular (Blinn-Phong)
        highlight += lampColour * pow(max(dot(normal, halfway), 0.0), shinePower) * shine * strength;
    }

    // Shiny things also catch a little light round their edges
    float edge = pow(1.0 - max(dot(normal, toCamera), 0.0), 3.0);
    highlight += vec3(1.0, 0.85, 0.65) * edge * shine * 0.12;

    vec3 colour = base.rgb * light + highlight;

    // Far away, everything fades into the dark of the room
    float fog = smoothstep(7.0, 18.0, length(viewPos - fragPosition));
    finalColor = vec4(mix(colour, fogColour, fog), base.a);
}
