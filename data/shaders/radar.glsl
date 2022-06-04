void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 pixelCoord = fragCoord.xy/iResolution.xy;
    float value = distance(pixelCoord, vec2(0.5));
    vec3 col = vec3(1.0, 0.4, 0);

    float r = abs(value-0.25);
    r = abs(r-0.08);
    r = abs(r-0.08);
    r = abs(r-0.05);
    float m = smoothstep(0.004, 0.002, r);
    float o = mod(iTime, 5.0);
    float h = smoothstep(0.2, 0.1, abs(value-o));
    col *= m*(h+0.5);

    // Output to screen
    fragColor = vec4(col, 1.0);
}