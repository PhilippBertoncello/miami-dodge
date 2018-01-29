#version 130

attribute vec2 in_position;
attribute vec4 in_color;

uniform mat4 uni_viewmodel;

varying vec4 var_color;

void main() {
	gl_Position = (vec4(in_position, 1, 1) * uni_viewmodel);
	gl_Position.z = 1;
	gl_Position.w = 1;
	var_color = in_color;
}