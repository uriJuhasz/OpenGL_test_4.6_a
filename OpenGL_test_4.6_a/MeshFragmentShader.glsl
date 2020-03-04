#version 410

struct VertexData
{
    vec3 position;
    vec3 normal;
    vec2 uvCoord;
};

layout (location=1) in VertexData geometryVertex;

uniform vec3 light0Position;
uniform vec3 light0Color;
uniform float light0SpecularExponent;
	
uniform vec3 light1Position;
uniform vec3 light1Color;
uniform float light1SpecularExponent;

uniform vec3 viewerPosition;

out vec4 frag_color;

vec3 calculateLight(vec3 lightPosition, vec3 lightColor, float lightSpecularExponent, vec3 baseColor, vec3 normal)
{
	vec3 lightDirection = normalize(lightPosition-geometryVertex.position);
	float diffuseLight = dot(lightDirection,normal);
	vec3 diffuseColor = clamp(diffuseLight * baseColor,0,1);

	vec3 viewerDirection = normalize(viewerPosition-geometryVertex.position);
	vec3 reflectedLight = 2*diffuseLight*normal - lightDirection;

	float specularIntensity = pow(clamp(dot(reflectedLight,viewerDirection),0,1),lightSpecularExponent);
	vec3 specularColor = specularIntensity*lightColor;

	return clamp(diffuseColor + specularColor,0,1);
}

vec3 calculateBaseColor()
{
	return
		vec3(1,0,0)*(clamp(1-geometryVertex.uvCoord.x-geometryVertex.uvCoord.y,0,1) + clamp(geometryVertex.uvCoord.x-geometryVertex.uvCoord.y,0,1)) +
		vec3(0,1,0)*(clamp(geometryVertex.uvCoord.y-geometryVertex.uvCoord.x,0,1) + clamp(geometryVertex.uvCoord.x-geometryVertex.uvCoord.y,0,1)) +
		vec3(0,0,1)*(clamp(geometryVertex.uvCoord.x+geometryVertex.uvCoord.y-1.0f,0,1))
	;
}

void main() {
	vec3 normal = normalize(geometryVertex.normal);
	vec3 baseColor = calculateBaseColor();

	vec3 light0Component = calculateLight(light0Position,light0Color,light0SpecularExponent,baseColor, normal);
	vec3 light1Component = calculateLight(light1Position,light1Color,light1SpecularExponent,baseColor, normal);
	frag_color = vec4(light0Component + light1Component ,1);
}
