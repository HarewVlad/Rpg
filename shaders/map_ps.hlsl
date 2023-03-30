SamplerState s;
Texture2D t;

struct VS_OUT
{
  float4 pos : sv_position;
  float2 tex : texcoord;
};

float4 main(VS_OUT input) : sv_target
{
  // return t.Sample(s, input.tex);
  return float4(1, 0, 0, 1);
}