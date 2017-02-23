#version 330 core

//tracerFragmentSourceHeader
precision highp float;

uniform vec3 eye;
in vec3 initialRay;

uniform float textureWeight;
uniform float timeSinceStart;
uniform sampler2D texture;
uniform float glossiness;
vec3 roomCubeMin = vec3(-1.0, -1.0, -1.0);
vec3 roomCubeMax = vec3(1.0, 1.0, 1.0);

//concat(objects, function(o){ return o.getGlobalCode(); })
uniform vec3 light;

//intersectCubeSource
vec2 intersectCube(vec3 origin, vec3 ray, vec3 cubeMin, vec3 cubeMax) {
   vec3 tMin = (cubeMin - origin) / ray;
   vec3 tMax = (cubeMax - origin) / ray;
   vec3 t1 = min(tMin, tMax);
   vec3 t2 = max(tMin, tMax);
   float tNear = max(max(t1.x, t1.y), t1.z);
   float tFar = min(min(t2.x, t2.y), t2.z);
   return vec2(tNear, tFar);
   }

//normalForCubeSource
vec3 normalForCube(vec3 hit, vec3 cubeMin, vec3 cubeMax)
{
   if(hit.x < cubeMin.x + 0.0001) return vec3(-1.0, 0.0, 0.0);
   else if(hit.x > cubeMax.x - 0.0001) return vec3(1.0, 0.0, 0.0);
   else if(hit.y < cubeMin.y + 0.0001) return vec3(0.0, -1.0, 0.0);
   else if(hit.y > cubeMax.y - 0.0001) return vec3(0.0, 1.0, 0.0);
   else if(hit.z < cubeMin.z + 0.0001) return vec3(0.0, 0.0, -1.0);
   else return vec3(0.0, 0.0, 1.0);
}

//intersectSphereSource
float intersectSphere(vec3 origin, vec3 ray, vec3 sphereCenter, float sphereRadius) {
   vec3 toSphere = origin - sphereCenter;
   float a = dot(ray, ray);
   float b = 2.0 * dot(toSphere, ray);
   float c = dot(toSphere, toSphere) - sphereRadius*sphereRadius;
   float discriminant = b*b - 4.0*a*c;
   if(discriminant > 0.0) {
     float t = (-b - sqrt(discriminant)) / (2.0 * a);
     if(t > 0.0) return t;
   }
   return 10000;
}

//normalForSphereSource
vec3 normalForSphere(vec3 hit, vec3 sphereCenter, float sphereRadius) {
   return (hit - sphereCenter) / sphereRadius;
 }

//randomSource
float random(vec3 scale, float seed) {
   return fract(sin(dot(gl_FragCoord.xyz + seed, scale)) * 43758.5453 + seed);
}

//from http://www.rorydriscoll.com/2009/01/07/better-sampling/
//cosineWeightedDirectionSource
vec3 cosineWeightedDirection(float seed, vec3 normal) {
   float u = random(vec3(12.9898, 78.233, 151.7182), seed);
   float v = random(vec3(63.7264, 10.873, 623.6736), seed);
   float r = sqrt(u);
   float angle = 6.283185307179586 * v;
   // compute basis from normal
   vec3 sdir, tdir;
   if (abs(normal.x)<.5) {
     sdir = cross(normal, vec3(1,0,0));
   } else {
     sdir = cross(normal, vec3(0,1,0));
   }
   tdir = cross(normal, sdir);
   return r*cos(angle)*sdir + r*sin(angle)*tdir + sqrt(1.-u)*normal;
 }

//uniformlyRandomDirectionSource
vec3 uniformlyRandomDirection(float seed) {
   float u = random(vec3(12.9898, 78.233, 151.7182), seed);
   float v = random(vec3(63.7264, 10.873, 623.6736), seed);
   float z = 1.0 - 2.0 * u;
   float r = sqrt(1.0 - z * z);
   float angle = 6.283185307179586 * v;
   return vec3(r * cos(angle), r * sin(angle), z);
}

//uniformlyRandomVectorSource
vec3 uniformlyRandomVector(float seed) {
   return uniformlyRandomDirection(seed) * sqrt(random(vec3(36.7539, 50.3658, 306.2759), seed));
}

//makeShadow
float shadow(vec3 origin, vec3 ray) {
   //concat(objects, function(o){ return o.getShadowTestCode(); }) +
   //vec2  tCubeID = intersectCube(origin, ray, cubeMinID, cubeMaxID);
   //if(tCubeID.x > 0.0 && tCubeID.x < 1.0 && tCubeID.x < tCubeID.y) return 0.0;
   
   
   return 1.0;
 }

//makeCalculateColor
vec3 calculateColor(vec3 origin, vec3 ray, vec3 light) {
    vec3 colorMask = vec3(1.0);
    vec3 accumulatedColor = vec3(0.0);
  
    // main raytracing loop
    for(int bounce = 0; bounce < 5; bounce++) {
      // compute the intersection with everything
      vec2 tRoom = intersectCube(origin, ray, roomCubeMin, roomCubeMax);
      //concat(objects, function(o){ return o.getIntersectCode(); }) +

      // find the closest intersection
      float t = 10000.0;
      if(tRoom.x < tRoom.y) t = tRoom.y;
      //concat(objects, function(o){ return o.getMinimumIntersectCode(); }) +

      // info about hit
      vec3 hit = origin + ray * t;
      vec3 surfaceColor = vec3(0.75);
      float specularHighlight = 0.0;
      vec3 normal;

      // calculate the normal (and change wall color)
      if(t == tRoom.y) {
        normal = -normalForCube(hit, roomCubeMin, roomCubeMax);
        //[yellowBlueCornellBox, redGreenCornellBox][environment] +
        //newDiffuseRay +
		if(hit.x < -0.9999) surfaceColor = vec3(1.0, 0.3, 0.1);
        else if(hit.x > 0.9999) surfaceColor = vec3(0.3, 1.0, 0.1);
		ray = cosineWeightedDirection(timeSinceStart + float(bounce), normal);
      } else if(t == 10000.0) {
        break;
      } else {
        if(false) ;
        //concat(objects, function(o){ return o.getNormalCalculationCode(); }) +
        //[newDiffuseRay, newReflectiveRay, newGlossyRay][material] +
		ray = cosineWeightedDirection(timeSinceStart + float(bounce), normal);
      }

      // compute diffuse lighting contribution
      vec3 toLight = light - hit;
      float diffuse = max(0.0, dot(normalize(toLight), normal));

      // trace a shadow ray to the light
      float shadowIntensity = shadow(hit + normal * 0.0001, toLight);

      // do light bounce
      colorMask *= surfaceColor;
	  accumulatedColor += colorMask * (0.5 * diffuse * shadowIntensity);
      accumulatedColor += colorMask * specularHighlight * shadowIntensity;
	  
	  //accumulatedColor = vec3(1,0.7,0.5);
      // calculate next origin
      origin = hit;
    }
	
    return accumulatedColor;
  }

void main() {
   vec3 newLight = light + uniformlyRandomVector(timeSinceStart - 53.0) * 0.1;
   vec3 texture = texture2D(texture, gl_FragCoord.xy / 512.0).rgb;
   //vec4 tempColor = vec4(mix(calculateColor(eye, initialRay, newLight), texture, textureWeight), 1.0);
   gl_FragColor = vec4(mix(calculateColor(eye, initialRay, newLight), texture, textureWeight), 1.0);
   //gl_FragColor = vec4(mix(calculateColor(eye, initialRay, newLight), texture, 0.2), 1.0);
   //gl_FragColor = vec4(texture, 1.0);
}














