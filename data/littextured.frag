precision mediump float;
uniform vec4	  MaterialColour;
uniform sampler2D TextureSampler;
varying vec3 vWorldPosition;
varying vec3 vNormal;
varying vec2 vTexCoord;


vec3 lightNormal = vec3(0.0, -1.0, 0.0);	


void main()
{
	float d = length(vWorldPosition);
	vec4 texcolour = vec4(1.0, 1.0, 1.0, 1.0);//texture2D(TextureSampler, vTexCoord, -100.0) * MaterialColour;

	float dp = dot(vNormal, lightNormal);
	//d = d < 2.0 ? 0.0 : 1.0; // (d* d*d*d) * 100.0;
	//vec4(d, d, 0.0, 1.0) 
	gl_FragColor =  texcolour.zyxw * dp;		
	
}
