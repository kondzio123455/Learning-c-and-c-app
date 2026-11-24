#version 330

// Lighting, part 1 (runs once per corner of every triangle).
// Works out where each point is in the world and which way it faces,
// so lighting.fs can light it.

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;        // model -> screen (set by raylib)
uniform mat4 matModel;   // model -> world (set by raylib)

out vec3 fragPosition;   // where the point is in the world
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;     // which way it faces

void main() {
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    fragNormal = normalize(mat3(matModel) * vertexNormal);   // turned with the object (we never stretch objects)
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
