#version 430 core

out vec4 outFragColor;

in VertexData
{
	vec4 vColor;
	vec2 vUV;
	float vTextureStrength;
	float vOpacity;
	flat int vTexture;
} vData;

uniform sampler2D uTextures[16];

void main()
{
	vec4 texColor = texture(uTextures[vData.vTexture], vData.vUV);
	vec4 color = mix(vData.vColor, texColor, vData.vTextureStrength);
	color.a *= vData.vOpacity;
	if (color.a < 0.02) {
		discard;
	}
	outFragColor = color;
}
