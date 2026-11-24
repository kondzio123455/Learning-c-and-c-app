#version 330

// Post-processing: runs on the finished picture of the 3D scene.
// 1. Bloom: bright parts (the lamp, highlights on the balls) glow softly.
// 2. Vignette: the corners get darker, like an old photo of a pool hall.

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;   // the picture of the scene
uniform vec4 colDiffuse;
uniform vec2 texel;           // one step between samples (sized so the glow looks the same at any resolution)
uniform float vignette;       // how dark the corners get (0 to 1)
uniform float bloom;          // how strong the glow is

out vec4 finalColor;

// Only the bright parts of the picture glow
vec3 brightPart(vec2 uv) {
    vec3 colour = texture(texture0, uv).rgb;
    float brightness = max(colour.r, max(colour.g, colour.b));
    return colour * smoothstep(0.7, 1.0, brightness);
}

void main() {
    vec3 colour = texture(texture0, fragTexCoord).rgb;

    // Blur the bright parts: add up the samples round this pixel, closer ones counting more
    vec3 glow = vec3(0.0);
    float total = 0.0;
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            float weight = exp(-float(x * x + y * y) / 3.0);
            glow += brightPart(fragTexCoord + vec2(float(x), float(y)) * texel) * weight;
            total += weight;
        }
    }
    colour += glow / total * bloom;

    // Darker towards the corners
    vec2 fromCentre = fragTexCoord - vec2(0.5);
    float corner = smoothstep(0.25, 0.8, length(fromCentre * vec2(1.0, 1.15)));
    colour *= 1.0 - corner * vignette;

    finalColor = vec4(colour, 1.0) * colDiffuse * fragColor;
}
