struct VS_IN {
	float4 position : POSITION0;
	float4 color : COLOR0;
};

struct PS_IN {
	float4 position : SV_POSITION;
 	float4 color : COLOR;
};