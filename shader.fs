#version 100

precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

// Make a nice spectrum of colors based on counter and maxSize
vec4 Colorizer(float counter, float maxSize)
{
    float red = 0.0, green = 0.0, blue = 0.0;
    float normsize = counter/maxSize;

    red = smoothstep(0.3, 0.7, normsize);
    green = sin(3.14159*normsize);
    blue = 1.0 - smoothstep(0.0, 0.4, normsize);

    return vec4(0.8*red, 0.8*green, 0.8*blue, 1.0);
}

void main()
{
    vec4 color = vec4(1.0);
    float scale = 1000.0; // Makes 100x100 square grid. Change this variable to make a smaller or larger grid
    float value = scale*floor(fragTexCoord.y*scale) + floor(fragTexCoord.x*scale);  // Group pixels into boxes representing integer values
    int valuei = int(value);

    //if ((valuei == 0) || (valuei == 1) || (valuei == 2)) gl_FragColor = vec4(1.0);
    //else
    {
        //for (int i = 2; (i < int(max(2.0, sqrt(value) + 1.0))); i++)
        // NOTE: On GLSL 100 for loops are restricted and loop condition must be a constant
        // Tested on RPI, it seems loops are limited around 60 iteractions
        for (int i = 2; i < 48; i++)
        {
            if ((value - float(i)*floor(value/float(i))) <= 0.0)
            {
                gl_FragColor = Colorizer(float(i), scale);
                //break;    // Uncomment to color by the largest factor instead
            }
        }
    }
}