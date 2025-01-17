#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform float time;
uniform vec2 texScroll;
uniform vec2 texScroll2;
uniform float texFaceted;
uniform float texFaceted2;

out vec2 fragTexCoord;
out vec2 fragTexCoord2;

void main() {
    vec3 normal = normalize(vertexNormal);
    vec2 faceted = vec2((normal.x + 1.0) * 0.5, (normal.y + 1.0) * 0.5);
    fragTexCoord = texFaceted * faceted + (1.0 - texFaceted) * vertexTexCoord;
    fragTexCoord = fragTexCoord + fract(texScroll * time);
    fragTexCoord2 = texFaceted2 * faceted + (1.0 - texFaceted2) * vertexTexCoord;
    fragTexCoord2 = fragTexCoord2 + fract(texScroll2 * time);
    gl_Position = mvp*vec4(vertexPosition , 1.0);
}
