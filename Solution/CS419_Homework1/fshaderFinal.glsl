#version 430

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;

in vec3 N;
in vec3 E;
in vec3 L;

out vec4 fColor;

void main()
{
	vec3 fN = normalize(N);
	vec3 fE = normalize(E);
	vec3 fL = normalize(L);

	vec3 fH = normalize( fL + fE.xyz );

	vec4 ambient  = AmbientProduct;
	vec4 diffuse  = max(dot(fL, fN), 0.0) * DiffuseProduct;
	vec4 specular = pow(max(dot(fN, fH), 0.0), Shininess) * SpecularProduct;

	if( dot(fL, fN) < 0.0 )
	{
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}

	fColor = vec4( (ambient + diffuse + specular).xyz, 1.0);
}
