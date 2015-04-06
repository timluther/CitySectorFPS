precision mediump float;
uniform vec4	  MaterialColour;
uniform sampler2D TextureSampler;
varying vec3 vWorldPosition;
varying vec3 vNormal;
varying vec2 vTexCoord;


uniform vec3 uLightNormal;	
uniform vec3 uSkyColour;
uniform vec3 uGroundColour;

vec3 get_ambient_light(vec3 normal)
{
	return mix(uGroundColour, uSkyColour, (normal.y * 0.5) + 0.5);
}


void main()
{
	float d = length(vWorldPosition);
	vec4 texcolour = vec4(1.0,1.0,1.0,1.0);//= texture2D(TextureSampler, vTexCoord, -100.0) * MaterialColour;

	float dp = max(0.0, dot(vNormal, uLightNormal));
	//d = d < 2.0 ? 0.0 : 1.0; // (d* d*d*d) * 100.0;
	//vec4(d, d, 0.0, 1.0) 
	gl_FragColor.a = 1.0;
	gl_FragColor.rgb = get_ambient_light(vNormal);
	gl_FragColor +=  texcolour.zyxw * dp;		
	
}
