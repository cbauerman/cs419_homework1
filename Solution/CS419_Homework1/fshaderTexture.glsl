#version 430

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;

uniform sampler2D textureColor;

in vec3 N;
in vec3 E;
in vec3 L;
in vec2 texCoord;

out vec4 fragColor;

void main()
{

	vec3 fN = normalize(N);
	vec3 fE = normalize(E);
	vec3 fL = normalize(L);

	vec3 fH = normalize( fL + fE.xyz );

	//get texture color
	vec4 T = texture2D( textureColor, texCoord);

	vec4 ambient  = AmbientProduct * T;
	vec4 diffuse  = max(dot(fL, fN), 0.0) * DiffuseProduct * T;
	vec4 specular = pow(max(dot(fN, fH), 0.0), Shininess) * SpecularProduct;

	if( dot(fL, fN) < 0.0 )
	{
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}

	fragColor = vec4( (ambient + diffuse + specular).xyz, 1.0);

	
}
