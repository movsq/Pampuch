#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float time;
uniform int mode;
uniform vec2 playerPos;
uniform float powerMode;

float lum(vec3 c) { return dot(c, vec3(0.299, 0.587, 0.114)); }

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float detectEdge(vec2 uv, vec2 px) {
    float center = lum(texture(texture0, uv).rgb);
    float top = lum(texture(texture0, uv + vec2(0.0, px.y)).rgb);
    float bot = lum(texture(texture0, uv - vec2(0.0, px.y)).rgb);
    float left = lum(texture(texture0, uv - vec2(px.x, 0.0)).rgb);
    float right = lum(texture(texture0, uv + vec2(px.x, 0.0)).rgb);

    float edge = abs(center - top) + abs(center - bot) +
                 abs(center - left) + abs(center - right);
    return edge;
}

vec3 smartBlur(vec2 uv, vec2 px, float strength) {
    vec3 center = texture(texture0, uv).rgb;
    vec3 sum = center * 4.0;

    sum += texture(texture0, uv + vec2(px.x, 0.0)).rgb;
    sum += texture(texture0, uv - vec2(px.x, 0.0)).rgb;
    sum += texture(texture0, uv + vec2(0.0, px.y)).rgb;
    sum += texture(texture0, uv - vec2(0.0, px.y)).rgb;

    sum += texture(texture0, uv + vec2(px.x, px.y) * 0.7).rgb * 0.5;
    sum += texture(texture0, uv - vec2(px.x, px.y) * 0.7).rgb * 0.5;
    sum += texture(texture0, uv + vec2(-px.x, px.y) * 0.7).rgb * 0.5;
    sum += texture(texture0, uv - vec2(-px.x, px.y) * 0.7).rgb * 0.5;

    vec3 blurred = sum / 10.0;
    return mix(center, blurred, strength);
}

void main() {
    vec2 uv = fragTexCoord;
    vec2 px = 1.0 / resolution;

    vec3 src = texture(texture0, uv).rgb;
    float alpha = texture(texture0, uv).a;
    float brightness = lum(src);

    float edge = detectEdge(uv, px);

    float aaStrength = 0.0;
    if (brightness > 0.2) {
        aaStrength = smoothstep(0.05, 0.3, edge) * 0.75;
    }

    vec3 color = smartBlur(uv, px, aaStrength);

    float scanBand1 = sin(uv.y * 8.0 + time * 1.2) * 0.5 + 0.5;
    float scanBand2 = sin(uv.x * 6.0 - time * 0.8) * 0.5 + 0.5;
    float scanPattern = (scanBand1 + scanBand2) * 0.5;

    float hue = fract(scanPattern * 0.4 + time * 0.05);
    vec3 scanColor = hsv2rgb(vec3(hue, 0.4, 0.6));

    float backgroundMask = 1.0 - smoothstep(0.15, 0.5, brightness);

    float offset = 0.0015 * backgroundMask;
    vec3 chromatic = vec3(
        texture(texture0, uv + vec2(offset, 0.0)).r,
        texture(texture0, uv).g,
        texture(texture0, uv - vec2(offset, 0.0)).b
    );

    color = mix(color, chromatic, backgroundMask * 0.3);
    color = mix(color, color * scanColor * 1.2, backgroundMask * 0.2);

    vec2 center = uv - 0.5;
    float vignette = 1.0 - dot(center, center) * 0.3;
    color *= vignette;

    finalColor = vec4(color, alpha);
}
