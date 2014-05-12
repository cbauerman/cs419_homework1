#version 130

in  vec4 vPosition;
in  vec3 vNormal;
in  vec2 vTexCoord;

out vec3 N;
out vec3 E;
out vec3 L;
out vec2 texCoord;

uniform mat4 ModelView, Projection;
uniform vec4 LightPosition, Rot;


vec4 q_multiply(vec4 a, vec4 b){
	return vec4(a.x * b.x - dot(a.yzw, b.yzw), a.x * b.yzw + b.x * a.yzw + cross(a.yzw, b.yzw));
}

vec4 q_inverse(vec4 q){
	return 1/length(q) * vec4( q.x, -q.yzw);
}

vec4 q_rot(vec4 q, vec4 v){
	return vec4(q_multiply(q_multiply(q, vec4(0, v.xyz)), q_inverse(q)).yzw, v.w);
}



void main() 
{   

	vec4 rPosition = q_rot(Rot, vPosition);
	vec3 rNormal =  q_rot(Rot, vec4(vNormal, 0.0)).xyz;

	N = (ModelView * vec4(rNormal, 0.0)).xyz; 
	E = -(ModelView * rPosition).xyz;
	L = (ModelView * LightPosition).xyz;

	if(LightPosition.w != 0.0)
	{
		L = L + E.xyz;
	}

	//pass texture coordinates to fragment shader
	texCoord = vTexCoord;

	

	gl_Position = Projection * ModelView * rPosition;
}
