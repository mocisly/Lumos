#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#include "Buffers.glslh"

layout(push_constant) uniform PushConsts
{
	mat4 transform;
} pushConsts;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;
layout(location = 6) in ivec4 inBoneIndices;
layout(location = 7) in vec4 inBoneWeights;

struct VertexData
{
	vec3 Colour;
	vec2 TexCoord;
	vec4 Position;
	vec3 Normal;
	mat3 WorldNormal;
};

layout(location = 0) out VertexData VertexOutput;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	vec3 position = inPosition;
	vec4 colour = inColor;
	vec2 uv = inTexCoord;
	vec3 normal = inNormal;
	vec3 tangent = inTangent;
	vec3 bitangent = inBitangent;
	ivec4 boneIn = inBoneIndices;
    vec4 boneWe = inBoneWeights;

	mat4 boneTransform = u_BoneTransforms.BoneTransforms[int(boneIn[0])] * boneWe[0];
    boneTransform += u_BoneTransforms.BoneTransforms[int(boneIn[1])] * boneWe[1];
    boneTransform += u_BoneTransforms.BoneTransforms[int(boneIn[2])] * boneWe[2];
    boneTransform += u_BoneTransforms.BoneTransforms[int(boneIn[3])] * boneWe[3];

	VertexOutput.Position = pushConsts.transform * boneTransform * vec4(position, 1.0);
    gl_Position = u_CameraData.projView * VertexOutput.Position;

	VertexOutput.Colour = colour.xyz;
	VertexOutput.TexCoord = uv;
	mat3 transposeInv = transpose(inverse(mat3(pushConsts.transform) * mat3(boneTransform)));
    VertexOutput.Normal = transposeInv * normal;

    VertexOutput.WorldNormal = transposeInv * mat3(tangent, bitangent, normal);
}