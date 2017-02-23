#version 410 core
precision mediump float;

//in vec2 TexCoord;
out vec4 color;
//uniform sampler2D textureAA;

in vec2 v_rgbNW;
in vec2 v_rgbNE;
in vec2 v_rgbSW;
in vec2 v_rgbSE;
in vec2 v_rgbM;

//#define FXAA_REDUCE_MIN (1.0/ 128.0)
#define FXAA_REDUCE_MIN (1.0/ 1024.0)
//#define FXAA_REDUCE_MUL (1.0 / 8.0)
#define FXAA_REDUCE_MUL (1.0 / 128.0)
#define FXAA_SPAN_MAX   8.0


in vec2 vUv;
uniform vec2 iResolution;
//vec2 iResolution = vec2(1024,1024);
uniform sampler2D textureAA;
uniform int enabled;

vec4 fxaa(sampler2D tex, vec2 fragCoord, vec2 resolution,
            vec2 v_rgbNW, vec2 v_rgbNE, 
            vec2 v_rgbSW, vec2 v_rgbSE, 
            vec2 v_rgbM) {
    vec4 color;
    vec2 inverseVP = vec2(1.0 / resolution.x, 1.0 / resolution.y);
    vec3 rgbNW = texture2D(tex, v_rgbNW).xyz;
    vec3 rgbNE = texture2D(tex, v_rgbNE).xyz;
    vec3 rgbSW = texture2D(tex, v_rgbSW).xyz;
    vec3 rgbSE = texture2D(tex, v_rgbSE).xyz;
    vec4 texColor = texture2D(tex, v_rgbM);
    vec3 rgbM  = texColor.xyz;
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *
                          (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
              max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
              dir * rcpDirMin)) * inverseVP;
    
    vec3 rgbA = 0.5 * (
        texture2D(tex, fragCoord * inverseVP + dir * (1.0 / 3.0 - 0.5)).xyz +
        texture2D(tex, fragCoord * inverseVP + dir * (2.0 / 3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture2D(tex, fragCoord * inverseVP + dir * -0.5).xyz +
        texture2D(tex, fragCoord * inverseVP + dir * 0.5).xyz);
    
	//color = vec4(fragCoord* inverseVP, 0, 1);

	float lumaB = dot(rgbB, luma);
    if ((lumaB < lumaMin) || (lumaB > lumaMax))
        color = vec4(rgbA, texColor.a);
    else
        color = vec4(rgbB, texColor.a);
    return color;
}


void main()
{
  //can also use gl_FragCoord.xy
  vec2 fragCoord = vUv * iResolution;
  //vec2 fragCoord = vUv * vec2(1024,1024); 
  //vec2 fragCoord = vec2(1,1);

  vec4 colorF;
  if (enabled == 1) {
      colorF = fxaa(textureAA, fragCoord, iResolution, v_rgbNW, v_rgbNE, v_rgbSW, v_rgbSE, v_rgbM);
      //colorF = vec4(1,1,1,1);
  } else {
      colorF = texture2D(textureAA, vUv);
  }

  color = colorF;
  //color = vec4(1,1,1,1);
}