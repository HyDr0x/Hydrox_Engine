#version 440 core

in float gsout_clipDepth;

void main()
{
	if(gsout_clipDepth < 0.0)
	{
		discard;
	}
	gl_FragColor = vec4(1.0);
}