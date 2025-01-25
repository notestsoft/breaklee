#version 330

precision highp float;

in vec3 fragPosition;
in vec3 fragCameraPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec2 fragTexCoord2;

uniform sampler2D texture0;
uniform sampler2D texture1;
//uniform sampler2D texture2;

uniform float texBlendFactor;

//uniform vec3 lightPosition;  // Light position in world space
uniform vec3 lightDir;  // Light position in world space
uniform float lightIntensity; // Intensity of the light

// Uniform for material properties
uniform vec4 materialAmbient;  // Ambient color (RGBA)
uniform vec4 materialDiffuse;  // Diffuse color (RGBA)
uniform vec4 materialSpecular; // Specular color (RGBA)
uniform vec4 materialEmission; // Emission color (RGBA)
uniform float materialStrength; // Emission strength

out vec4 finalColor;

void main() {
    vec4 color0 = texture(texture0, fragTexCoord);
    vec4 color1 = texture(texture1, fragTexCoord2);
    vec4 combinedTextureColor = color0 + color1 * vec4(texBlendFactor);
    if (combinedTextureColor.a < 0.005) discard;

        // Normalize the normal vector
    vec3 normal = normalize(fragNormal);
    
    // Calculate light direction
    //vec3 lightDir = normalize(lightPosition - fragPosition);
    vec3 nlightDir = normalize(lightDir);
    // Ambient lighting
    vec3 ambient = materialAmbient.rgb * lightIntensity * materialAmbient.a;
    
    // Diffuse lighting
    float diff = max(dot(normal, nlightDir), 0.0);
    vec3 diffuse = materialDiffuse.rgb * diff * lightIntensity * materialDiffuse.a;
    
    // Specular lighting
    vec3 viewDir = normalize(fragCameraPosition - fragPosition);
    vec3 reflectDir = reflect(-nlightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 5.0);  // Use material shininess or define another parameter
    vec3 specular = materialSpecular.rgb * spec * lightIntensity * materialSpecular.a;
    
    // Emission
    vec3 emission = materialEmission.rgb * materialEmission.a * materialStrength;
    
    // Combine all lighting components
    vec3 lighting = ambient + diffuse + specular + emission;

    // Apply lighting to the texture color (multiply lighting with texture color)
    vec3 finalLightingColor = combinedTextureColor.rgb * lighting;
    // Final color is the combined texture color with the lighting applied
    finalColor = vec4(finalLightingColor, combinedTextureColor.a);
}
