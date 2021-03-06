R"(

struct Out {
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORDS;
	float4 color : COLOR;
	float tid : TID;
};

cbuffer view_data : register(b0) {
	float4x4 projection;
};

Out vsMain(float2 position : POSITION, float2 texCoords : TEXCOORDS, float4 color : COLOR, float tid : TID) {
	Out o;

	o.position = mul(projection, float4(position, 0, 1));
	o.texCoord = texCoords;
	o.color = color;
	o.tid = tid;

	return o;
}

)"