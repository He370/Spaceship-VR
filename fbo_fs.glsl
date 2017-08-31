#version 400

uniform sampler2D texture;
uniform sampler2D texture02;
uniform int pass;
uniform int blur = 0;
uniform vec3 eye;
uniform int mode;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform int lightOption;
uniform float m;
uniform float eta;

out vec4 fragcolor;           
in vec2 tex_coord;
in vec3 position;
in vec3 normal;

vec4 lighting(vec4 color, vec3 pos, vec3 rayDir);

void main(void)
{   
	if(pass == 1){
		vec3 rayDir = normalize(position - eye);
		//vec3 rayDir = normalize(position);
		vec4 color = texture2D(texture, tex_coord);
		fragcolor = lighting(color,position,rayDir);
		fragcolor = pow(fragcolor, vec4(0.45, 0.45, 0.45, 1.0));

		if(mode==0){
			vec4 colorLights = texture2D(texture02, tex_coord);
			if(length(fragcolor)<0.07f&&length(colorLights)>1.1f)
				fragcolor=fragcolor+vec4(colorLights.rg*0.5f,colorLights.b*0.3f,1.0f);
			//if(length(colorLights)>1.1f)
				//fragcolor.r=1.0f;
		}

		if(mode==2){
			//fragcolor = fragcolor.brga;
			//fragcolor.g = fragcolor.g*0.5f;
			//fragcolor.b = fragcolor.b*0.5f;
			fragcolor = fragcolor.grba;
		}

	}
	else if(pass == 2)
	{
      if(blur < 1 || blur > 2)
      {
         fragcolor = texelFetch(texture, ivec2(gl_FragCoord), 0);
      }
      fragcolor = vec4(0.0);
      for(int i = -blur; i<= +blur; i++)
      {
         for(int j = -blur; j<= +blur; j++)
         {
            fragcolor += texelFetch(texture, ivec2(gl_FragCoord)+ivec2(i,j), 0);
         }
      }
      fragcolor /= float((2*blur+1)*(2*blur+1));

	}
	else if(pass == 3)
	{
		fragcolor = vec4(0.05f, 0.05f, 0.05f, 1.0f);
	}
	else
	{
		fragcolor = vec4(1.0, 0.0, 0.0, 1.0); //error
	}

}

//compute lighting on the intersected surface
vec4 lighting(vec4 color, vec3 pos, vec3 rayDir)
{
	const vec3 light = vec3(1.0/1.7, 1.0/1.7, 1.0/1.7)*10.0;

	vec4 ambient_color = color*Ka.x;
	vec4 diffuse_color = color*Kd.x;
	vec4 spec_color = color*Ks.x;

	vec3 n = normal;
	vec3 v = -rayDir;
	vec3 r = reflect(-light, n);
	vec3 h = (light + v)/abs(light + v);

	const float eps = 1e-8;
	float dotNH = max(eps,dot(n,h));
	float dotVH = max(eps,dot(v,h));
	float dotNL = max(eps,dot(n,light));
	float dotNV = max(eps,dot(n,v));

	float fLambda = pow((1-eta)/(1+eta),2.0);
	float F = fLambda + (1-fLambda)*pow(1-dotNV,5.0);

	float tanAlphaPow2 = (1-pow(dotNH,2.0))/pow(dotNH,2.0);
	float cosAlphaPow4 = pow(dotNH,4.0);
	float D = exp(-1.0*tanAlphaPow2/pow(m,2.0))/(4.0*pow(m,2.0)*cosAlphaPow4);

	float G = min(2.0*dotNH*dotNV/dotNH,2.0*dotNH*dotNL/dotVH);
	G = min(1.0, G);

	float factor = 1.0;
	if(lightOption == 0)
		factor = F*D*G;
	else if(lightOption == 1)
		factor = F;
	else if(lightOption == 2)
		factor = D;
	else if(lightOption == 3)
		factor = G;

	return vec4(Ka,1.0)*ambient_color + vec4(Kd,1.0)*diffuse_color*max(0.0, dotNL) + vec4(Ks,1.0)*spec_color*(factor/(3.1415926*dotNV));
}
















