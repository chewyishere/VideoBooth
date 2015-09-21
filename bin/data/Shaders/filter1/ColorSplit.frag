#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect 	u_tex_unit0;
uniform float 			time;
uniform float blurAmount;
uniform float vol;
uniform float midi_size;
uniform float midi_r;
uniform float midi_g;
uniform float midi_b;
uniform float midi_a;
uniform float midi_time;

void main( void )
{
	//float tmpCos = (cos(u_time*2.0)+1.0)*0.5;
    //gl_FragColor = vec4( tmpCos, 1.0-tmpCos, 1.0, 1.0);

	//gl_FragColor = texture2DRect( u_tex_unit0, gl_TexCoord[0].st );

    float u_time = time*midi_time*0.6;
	float newTime = u_time * 2.0;
    float redTime = u_time*1.5;
    float greenTime = u_time*2.0;
    float blueTime = u_time*2.0;
    float effect = vol*0.3;

    
	vec2 newTexCoord;
	newTexCoord.s = gl_TexCoord[0].s + (cos(newTime + (gl_TexCoord[0].s*0.02)) * midi_size);
	newTexCoord.t = gl_TexCoord[0].t + (sin(newTime + (gl_TexCoord[0].t*0.02)) * midi_size);

	vec2 texCoordRed 	= newTexCoord;
	vec2 texCoordGreen 	= newTexCoord;
	vec2 texCoordBlue 	= newTexCoord;

	texCoordRed 	+= vec2( cos((redTime * 2.23)), sin((redTime * 2)) ) * effect * midi_r;
    
	texCoordGreen 	+= vec2( cos((greenTime * 2.23)), sin((greenTime * 2.40)) )*effect * midi_g;
    
	texCoordBlue 	+= vec2( cos((blueTime * 2.98)), sin((blueTime * 2.82)) ) *effect * midi_b;
    
	float colorR = texture2DRect( u_tex_unit0, texCoordRed ).r;
	float colorG = texture2DRect( u_tex_unit0, texCoordGreen).g;
	float colorB = texture2DRect( u_tex_unit0, texCoordBlue).b;

    vec4 color;
    color = vec4(colorR, colorG, colorB, midi_a);
    
    gl_FragColor = color;
    



}