uniform float iTime;

void main()
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 pixelCoord = gl_TexCoord[0].xy;
    float value = distance(pixelCoord, vec2(0.5));
    vec3 col = vec3(1.0, 0.4, 0);

    float r = abs(value-0.25);
    r = abs(r-0.08);
    r = abs(r-0.08);
    r = abs(r-0.05);
    float m = smoothstep(0.008, 0.004, r);
    float o = mod(iTime, 5.0);
    float h = smoothstep(0.2, 0.1, abs(value-o));
    col *= m*(h+0.5);

    // Output to screen
    gl_FragColor = vec4(col, m*0.5);
}