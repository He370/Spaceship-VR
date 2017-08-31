#version 400            
uniform mat4 PV;
uniform mat4 M;
uniform int pass;
uniform vec3 eye;
uniform float time;
uniform int mode;

uniform float S_scale;
uniform float S_seperate;
uniform float S_pos_x;
uniform float S_pos_y;
uniform float S_pos_z;
uniform float S_radius;

in vec3 pos_attrib;
in vec2 tex_coord_attrib;
in vec3 normal_attrib;
in vec3 transform_attrib;

out vec2 tex_coord;  
out vec3 position;
out vec3 normal;

void main(void)
{
	if(pass == 1)
	{
		if(mode == 0){
			gl_Position = PV*M*vec4(pos_attrib, 1.0);
		}
		else if(mode == 1){
			gl_Position = PV*M*vec4(pos_attrib*0.15*(length(transform_attrib)-8.0)/12.0*S_scale + transform_attrib*S_radius + vec3(S_pos_x,S_pos_y,S_pos_z), 1.0);
		}
		else{
			gl_Position = PV*M*vec4(pos_attrib, 1.0);
		}

		tex_coord = tex_coord_attrib;
		position = gl_Position.xyz;
		normal = normal_attrib;

		vec4 pos = vec4(gl_Position.xyz,1.0);
		vec4 nor = vec4(normal_attrib.xyz,1.0);
		//position = (M*pos).xyz;
		//normal = normalize((M*nor).xyz);
	}
	else if(pass == 3)
	{
      gl_Position = PV*M*vec4(pos_attrib, 1.0);
      return;
	}
	else if(pass == 2)
	{
		gl_Position = vec4(pos_attrib, 1.0);
		tex_coord = 0.5*pos_attrib.xy + vec2(0.5);
	}
	else
	{
		gl_Position = vec4(pos_attrib, 1.0);
		tex_coord = 0.5*pos_attrib.xy + vec2(0.5);
	}
}