#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;
out vec4 posPos;
//uniform float FXAA_SUBPIX_SHIFT = 1.0/4.0;
float FXAA_SUBPIX_SHIFT = 1.0/4.0;
uniform float rt_w; // GeeXLab built-in
uniform float rt_h; // GeeXLab built-in

void main(void)
{
  //gl_Position = ftransform();
  gl_Position = vec4(position,1.0f);
  //gl_TexCoord[0] = gl_MultiTexCoord0;
  TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);

  vec2 rcpFrame = vec2(1.0/rt_w, 1.0/rt_h);
  
  //posPos.xy = gl_MultiTexCoord0.xy;
  //posPos.zw = gl_MultiTexCoord0.xy - 
  //                (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));
  posPos.xy = TexCoord.xy;
  posPos.zw = TexCoord.xy - (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));
}
