#version 330 core
out vec4 FragColor;
in vec3 color;

in vec2 texCoord;
uniform vec3 mycolor;
uniform sampler2D tex0;
uniform vec3 light_col;

void main()
{
   FragColor = texture(tex0, texCoord) * vec4(light_col, 1.0f) ;
   //FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
