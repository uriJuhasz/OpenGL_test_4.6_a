#version 400

in vec3 fragmentPosition;
in vec3 fragmentNormal;
in vec2 fragmentUVCoord;

uniform vec3 light0Position;
uniform vec3 light1Position;
uniform vec3 viewerPosition;

out vec4 frag_color;

vec3 calculateLight(vec3 lightPosition, vec3 baseColor, vec3 normal)
{
	vec3 lightDirection = normalize(lightPosition-fragmentPosition);
	float diffuseLight = dot(lightDirection,normal);
	vec3 diffuseColor = clamp(diffuseLight * baseColor,0,1);

	const vec3 lightSpecularColor = vec3(1,1,1);
	const float specularExponent = 10;

	vec3 viewerDirection = normalize(viewerPosition-fragmentPosition);
	vec3 reflectedLight = 2*diffuseLight*normal - lightDirection;

	float specularIntensity = pow(clamp(dot(reflectedLight,viewerDirection),0,1),specularExponent);
	vec3 specularColor = specularIntensity*lightSpecularColor;

	return clamp(diffuseColor + specularColor,0,1);
}

vec3 calculateBaseColor()
{
	return
		vec3(1,0,0)*(clamp(1-fragmentUVCoord.x-fragmentUVCoord.y,0,1) + clamp(fragmentUVCoord.x-fragmentUVCoord.y,0,1)) +
		vec3(0,1,0)*(clamp(fragmentUVCoord.y-fragmentUVCoord.x,0,1) + clamp(fragmentUVCoord.x-fragmentUVCoord.y,0,1)) +
		vec3(0,0,1)*(clamp(fragmentUVCoord.x+fragmentUVCoord.y-1.0f,0,1))
	;
}

void main() {
	vec3 normal = normalize(fragmentNormal);
	vec3 baseColor = calculateBaseColor();

	vec3 light0Color = calculateLight(light0Position,baseColor, normal);
	vec3 light1Color = calculateLight(light1Position,baseColor, normal);
	frag_color = vec4(light0Color+light1Color,1);
}
