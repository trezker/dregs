#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURF_DIST .01

varying vec4 diffuse,ambient;
varying vec3 normal,lightDir,halfVector;

float dSphere(vec3 p, vec4 s) {
    return length(p-s.xyz)-s.w;
}

float GetDist(vec3 p) {
    float d = dSphere(p, vec4(0, 1, 6, 1));
    float pd = p.y;
    return min(d, pd);
}

float raymarch(vec3 ro, vec3 rd) {
    float dO = 0.;
    
    for(int i=0; i<MAX_STEPS; i++) {
        vec3 p = ro + rd*dO;
        float dS = GetDist(p);
        dO += dS;
        if(dO>MAX_DIST || dS<SURF_DIST) break;
    }
    
    return dO;
}

vec3 GetNormal(vec3 p) {
    float d = GetDist(p);
    vec2 e = vec2(.01, 0);
    
    vec3 n = d - vec3(
        GetDist(p-e.xyy),
        GetDist(p-e.yxy),
        GetDist(p-e.yyx)
    );
    
    return normalize(n);
}

float GetLight(vec3 p) {
    vec3 lightPos = vec3(0, 5, 5);
    //lightPos.xz += vec2(sin(iTime), cos(iTime))*2.; 
    vec3 l = normalize(lightPos-p);
    vec3 n = GetNormal(p);
    
    float dif = clamp(dot(n, l), 0., 1.);
    float d = raymarch(p+n*SURF_DIST*2., l);
    if(d<length(lightPos-p)) dif *= .1;
    
    return dif;
}

void main()
{
   //vec2 uv = gl_TexCoord[0].st;
   //gl_FragColor = vec4(uv.s, uv.t, 0, 1);
   
   vec2 uv = gl_TexCoord[0].xy-.5;

   vec3 col = vec3(0);
   vec3 ro = vec3(0, 2, -6);
   vec3 rd = normalize(vec3(uv.x, uv.y-.2, 1));

   float d = raymarch(ro, rd);

   vec3 p = ro + rd * d;

   float dif = GetLight(p);
   col = vec3(dif);

   gl_FragColor = vec4(col, .5);
}