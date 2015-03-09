precision mediump float;
uniform mat4 u_mvpmatrix;
attribute vec2 aTexCoord;
attribute vec4 aPosition;
varying vec3 vWorldPosition;
varying vec2 vTexCoord;


void main()
{
	gl_Position = aPosition;
	vWorldPosition = aPosition.xyz;
	vTexCoord = aTexCoord;
}
