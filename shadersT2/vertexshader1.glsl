#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 object_color;
uniform int hp;

// Output value to fragment shader
out vec3 color;


void main()
{

    // TODO(student): Send color light output to fragment shader
    color = object_color;
    float deformation;
    if (v_position.x > 0.5) {
        deformation = 0.1 * float(3 - hp);
    } else {
        deformation = - 0.1 * float(3 - hp);
    }

    if (hp == 2) {
        color.x -= 0.1;
        color.y -= 0.1;
        color.z -= 0.1;
        if (color.x < 0) {
            color.x = 0;
        }
        if (color.y < 0) {
            color.y = 0;
        }
        if (color.z < 0) {
            color.z = 0;
        }
    }
    if (hp == 1) {
        color.x -= 0.2;
        color.y -= 0.2;
        color.z -= 0.2;
        if (color.x < 0) {
            color.x = 0;
        }
        if (color.y < 0) {
            color.y = 0;
        }
        if (color.z < 0) {
            color.z = 0;
        }
    }
    if (hp <= 0) {
        color.x -= 0.3;
        color.y -= 0.3;
        color.z -= 0.3;
        if (color.x < 0) {
            color.x = 0;
        }
        if (color.y < 0) {
            color.y = 0;
        }
        if (color.z < 0) {
            color.z = 0;
        }
    }

    gl_Position = Projection * View * Model * vec4(v_position - vec3(0.0, deformation, 0.0), 1.0);
}
