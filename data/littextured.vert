precision mediump float;
uniform mat4 u_mvpmatrix;
uniform mat4 u_mmatrix;
attribute vec2 aTexCoord;
attribute vec4 aPosition;
attribute vec3 aNormal;
varying vec3 vWorldPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;



void main()
{
	gl_Position = u_mvpmatrix * aPosition;
	
	mat3 normalMatrix = mat3(u_mmatrix);
	vWorldPosition =  (u_mmatrix * aPosition).xyz;

	vNormal = normalMatrix * aNormal;
	vTexCoord = aTexCoord;
}
