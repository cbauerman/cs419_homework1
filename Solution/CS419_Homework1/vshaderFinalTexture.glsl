in  vec4 vPosition;
in  vec3 vNormal;
in  vec3 vTangent;
in  vec2 vTexCoord;

out vec3 L;
out vec3 V;
out vec2 texCoord;

uniform mat4 ModelView, Projection, Trans;
uniform vec4 LightPosition, Eye;


void main() 
{   
	//create our tangent space vectors
	vec3 N = normalize(ModelView * Trans * vec4(vNormal, 0.0));
	vec3 T = normalize(ModelView * Trans * vec4(vTangent, 0.0));
	vec3 B = cross(N, T);

	// find our normal positions for the eye and light
	vec3 eyePosition = (ModelView * Trans * vPosition).xyz;
	vec3 eyeLightPosition = (ModelView * LightPosition).xyz;

	//Create light vector in tangent space coordinates
	L.x = dot(T, eyeLightPosition - eyePosition);
	L.y = dot(B, eyeLightPosition - eyePosition);	
	L.z = dot(N, eyeLightPosition - eyePosition);

	//Create eye vector in  tangent space coordinates
	V.x = dot(T, -eyePosition);
	V.y = dot(B, -eyePosition);	
	V.z = dot(N, -eyePosition);

	//pass texture coordinates to fragment shader
	texCoord = vTexCoord;

	gl_Position = Projection * ModelView * Trans *  vPosition;
}
