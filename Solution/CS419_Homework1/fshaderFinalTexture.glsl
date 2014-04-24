#version 430

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;

uniform sampler2D textureColor;
uniform sampler2D textureBump;

in vec3 V;
in vec3 L;
in vec2 texCoord;

out vec4 fragColor;

void main()
{
	//unpack out normal from bump map
	vec3 N = texture2D(textureBump, texCoord).xyz;
	vec3 fN = normalize( 2.0 * N - 1.0);

	vec3 fL = normalize(L);
	vec3 fV = normalize(V);

	//create halfway vector
	vec3 fH = normalize( fL + fV );

	//get texture color
	vec4 T = texture2D( textureColor, texCoord);

	//compute phong illumination with the normal from the bump map
	vec4 ambient  = AmbientProduct * T;
	vec4 diffuse  = DiffuseProduct * max(dot(fN, fL), 0.0) * T;
	vec4 specular = pow(max(dot(fN, fH), 0.0), Shininess) * SpecularProduct;

	if( dot(fL, fN) < 0.0 )
	{ 
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}

	fragColor = vec4( (ambient + diffuse + specular).xyz, 1.0);

	
}
