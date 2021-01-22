//R"(

#version 430

//Scalars
uniform bool u_bool;
uniform int u_int;
uniform uint u_uint;
uniform float u_float;

//Vectors
uniform vec2 u_vec2;
uniform vec3 u_vec3;
uniform vec4 u_vec4;

uniform bvec2 u_bvec2;
uniform bvec3 u_bvec3;
uniform bvec4 u_bvec4;

uniform ivec2 u_ivec2;
uniform ivec3 u_ivec3;
uniform ivec4 u_ivec4;

uniform uvec2 u_uvec2;
uniform uvec3 u_uvec3;
uniform uvec4 u_uvec4;

//Matrices - single precision
uniform mat2 u_mat2;
uniform mat3 u_mat3;
uniform mat4 u_mat4;

uniform mat2x2 u_mat2x2;
uniform mat2x3 u_mat2x3;
uniform mat2x4 u_mat2x4;

uniform mat3x2 u_mat3x2;
uniform mat3x3 u_mat3x3;
uniform mat3x4 u_mat3x4;

uniform mat4x2 u_mat4x2;
uniform mat4x3 u_mat4x3;
uniform mat4x4 u_mat4x4;

//Arrays
uniform float my_float_array_16[16];

//Struct uniforms
struct MyStruct1
{
  float s1_float;
  int   s1_int;
};

struct MyStruct2
{
  float s2_float;
  int   s2_int;
};

uniform MyStruct1 mys1;
uniform MyStruct2 mys2;

//Struct of structs
struct MyStruct_out1
{
  MyStruct1 my_out1;
  float out1_float;
};

struct MyStruct_out2
{
  MyStruct_out1 out1;
};

uniform MyStruct_out2 struct_of_structs;

//shader storage block
layout(std430) buffer MyShaderStorageBlock
{
    int           ssb_int;
    MyStruct_out2 ssb_layeredStruct;
    float         ssb_array[42];
    float         ssb_array_dynamic[];
};

//Uniform block
layout (std140) uniform MyUniformBlock
{
    int           ub_int;
    MyStruct_out2 ub_layeredStruct;
    float         ub_array[16];
}; 

uniform sampler2D texture1;
uniform sampler2D texture2[3];

out vec4 color;

void main(void)
{

}

//#type fragment
//
//#version 430
//
//uniform float fu_float;
//uniform int fu_int;
//uniform vec4 fu_vec4;
//uniform vec2 fu_vec2;
//uniform vec3 fu_vec3;
//uniform mat3 fu_mat3;
//uniform mat4 fu_mat4;
//
////shader storage block
//layout(std430) buffer anotherLayoutName
//{
//    int   fssb_some_int;
//    float fssb_fixed_array[42];
//    float fssb_variable_array[];
//};
//
////Uniform block
//layout (std140) uniform ExampleBlock
//{
//    float fub_value;
//    vec3  fub_vector;
//    mat4  fub_matrix;
//    float fub_values[3];
//    bool  fub_boolean;
//    int   fub_integer;
//}; 
//
//void main(void)
//{
//
//}
//)"