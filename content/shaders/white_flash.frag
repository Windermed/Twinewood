uniform sampler2D texture;

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
	
    if (pixel.a > 0.0)
	{
		gl_FragColor = vec4(1.0, 1.0, 1.0, pixel.a);
	}
    else
	{
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	}
}