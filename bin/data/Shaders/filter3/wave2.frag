#version 120

uniform sampler2DRect u_tex_unit0;
uniform float time;
uniform vec2 windowSize;
uniform vec2 mousePos;
uniform float vol;
uniform float midi_time;
uniform float midi_r;
uniform float midi_g;
uniform float midi_b;
uniform float midi_a;
uniform float midi_size;

void main( void )
{
    
    float effect = vol  * midi_size;
    
    vec2 newTexCoord;
	newTexCoord.s = gl_TexCoord[0].s + sin(gl_FragCoord.y * effect * 0.0002)*3;
    
    newTexCoord.t = gl_TexCoord[0].t + sin(gl_FragCoord.x * effect * 0.0002)*3;
 
    
    float r = texture2DRect( u_tex_unit0, newTexCoord ).r * (sin(time*midi_time) + 3) * 0.3;
    
    float g = texture2DRect( u_tex_unit0, newTexCoord ).g * (sin(time) + 2) * 0.3;

    float b = texture2DRect( u_tex_unit0, newTexCoord ).b * (sin(time) + 3) * 0.2;
    
	gl_FragColor = vec4(r*midi_r, g*midi_g, b*midi_b, midi_a);
}