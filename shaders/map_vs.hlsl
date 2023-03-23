cbuffer ConstantBuffer : register(b0)
{
  float4x4 mvp;
};

struct VS_IN
{
  float2 pos : position;
  float2 tex : texcoord;
};

struct VS_OUT
{
  float4 pos : sv_position;
  float2 tex : texcoord;
};

VS_OUT main(VS_IN input)
{
  VS_OUT vout;
  
  vout.pos = mul(float4(input.pos, 0.0f, 1.0f), mvp);
  vout.tex = input.tex;

  return vout;
}