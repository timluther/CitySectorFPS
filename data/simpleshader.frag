precision mediump float;
uniform vec4	  MaterialColour;
uniform sampler2D TextureSampler;
varying vec3 vWorldPosition;
varying vec2 vTexCoord;
		
void main()
{
	float d = length(vWorldPosition);
	vec4 texcolour = texture2D(TextureSampler, vTexCoord, -100.0) * MaterialColour;
	//d = d < 2.0 ? 0.0 : 1.0; // (d* d*d*d) * 100.0;
	//vec4(d, d, 0.0, 1.0) 
	gl_FragColor =  texcolour.zyxw;		
	
}
