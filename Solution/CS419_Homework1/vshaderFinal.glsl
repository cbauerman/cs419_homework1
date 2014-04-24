in  vec4 vPosition;
in  vec3 vNormal;

out vec3 N;
out vec3 E;
out vec3 L;

uniform mat4 ModelView, Projection, Trans;
uniform vec4 LightPosition;

void main() 
{   

	N = (ModelView * Trans  * vec4(vNormal, 0.0)).xyz; 
	E = -(ModelView * Trans * vPosition).xyz;
	L = (ModelView * LightPosition).xyz;

	if(LightPosition.w != 0.0)
	{
		L = L + E.xyz;
	}

	gl_Position = Projection * ModelView * Trans *  vPosition;
}
