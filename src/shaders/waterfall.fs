#version 330

// Input from vertex shader
in vec2 fragTexCoord;
in vec4 fragColor;

// Output
out vec4 finalColor;

// Previous frame texture (history)
uniform sampler2D texture0;

// Ambient data as 1D texture
uniform sampler2D ambientTexture;
uniform int ambientSize;

// Signal data (up to 16 signals)
uniform vec2 signalData[16];
uniform int signalCount;

// Display resolution
uniform vec2 resolution;

// Whether to insert new line and scroll (1.0 = yes)
uniform float drawNewLine;

// Color mapping - intensity to green waterfall color
vec3 intensityToColor(float t)
{
    t = clamp(t, 0.0, 1.0);
    
    if (t < 0.5)
    {
        return vec3(0.0, t * 2.0 * 0.5, 0.0);
    }
    else
    {
        float t2 = (t - 0.5) * 2.0;
        return vec3(t2 * 0.25, 0.5 + t2 * 0.5, t2 * 0.12);
    }
}

void main()
{
    vec2 uv = fragTexCoord;
    float lineHeight = 1.0 / resolution.y;
    
    if (drawNewLine > 0.5)
    {
        // We're inserting a new line - check if this is the top row
        if (uv.y < lineHeight)
        {
            // TOP ROW: Draw new data from ambient texture and signals
            float intensity = 0.0;
            
            // Sample ambient from texture (red channel, normalized 0-1)
            if (ambientSize > 0)
            {
                vec4 ambientSample = texture(ambientTexture, vec2(uv.x, 0.5));
                intensity = ambientSample.r;  // Already 0-1 from texture
            }
            
            // Add signal contributions
            for (int i = 0; i < signalCount && i < 16; i++)
            {
                float signalBearing = signalData[i].x;
                float signalIntensity = signalData[i].y / 255.0;  // Normalize to 0-1
                float signalX = signalBearing / 360.0;
                
                float dist = abs(uv.x - signalX);
                dist = min(dist, 1.0 - dist);  // Wrap-around
                
                float spread = 10.0 / resolution.x;
                if (dist < spread)
                {
                    float falloff = 1.0 - (dist / spread);
                    falloff = falloff * falloff;
                    intensity += signalIntensity * falloff;
                }
            }
            
            intensity = clamp(intensity, 0.0, 1.0);
            finalColor = vec4(intensityToColor(intensity), 1.0);
        }
        else
        {
            // OTHER ROWS: Sample from previous frame, shifted up by one line
            // (we read from y - lineHeight to shift content down visually)
            vec2 sampleUV = vec2(uv.x, uv.y - lineHeight);
            
            if (sampleUV.y >= 0.0)
            {
                finalColor = texture(texture0, sampleUV);
            }
            else
            {
                finalColor = vec4(0.0, 0.0, 0.0, 1.0);
            }
        }
    }
    else
    {
        // No new line - just pass through the texture
        finalColor = texture(texture0, uv);
    }
}
