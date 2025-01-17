#version 330

precision highp float;

in vec2 fragTexCoord;
in vec2 fragTexCoord2;

uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 finalColor;

void main() {
    vec4 color0 = texture(texture0, fragTexCoord);
    vec4 color1 = texture(texture1, fragTexCoord2);

    finalColor = color0 + color1;
}
