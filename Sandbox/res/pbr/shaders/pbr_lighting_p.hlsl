
SamplerState samp {
	U = Wrap;
	V = Wrap;
	W = Wrap;
	Filter = MIN_MAG_LINEAR;
};

static const float PI = 3.14159265359;

cbuffer Light : register(b0) {
	float3 l_Color;
	float l_pad0;
	float3 l_Position;
	float l_pad1;
	float3 l_Attenuation;
	float pad2;
};

cbuffer Material : register(b10) {
	float3 m_Albedo;
	float m_AlbedoFactor;
	float m_Metallic;
	float m_MetallicFactor;
	float m_Roughness;
	float m_RoughnessFactor;
	float m_AmbientOcclusion;
	float m_AmbientOcclusionFactor;

	float2 m_Pad0;
};

TextureCube m_EnvironmentMap : register(t1);
Texture2D m_AlbedoMap : register(t2);
Texture2D m_MetallicMap : register(t3);
Texture2D m_RoughnessMap : register(t4);
Texture2D m_AmbientOcclusionMap : register(t5);

float DistributionGGX(float3 N, float3 H, float roughness) {
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
	
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness) {

	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness) {
	float oneMinusRough = 1 - roughness;
	return F0 + (max(float3(oneMinusRough, oneMinusRough, oneMinusRough), F0) - F0) * pow(1 - cosTheta, 5);
}

float4 psMain(float4 pos : SV_POSITION, float3 position : POSITION, float3 normal : NORMAL, float2 texCoord : TEXCOORD, float3 camPos : CAMPOS) : SV_TARGET0 {

	float3 albedo = lerp(m_Albedo, m_AlbedoMap.Sample(samp, texCoord).xyz, m_AlbedoFactor);
	float metallic = lerp(m_Metallic, m_MetallicMap.Sample(samp, texCoord).x, m_MetallicFactor);
	float roughness = lerp(m_Roughness, m_RoughnessMap.Sample(samp, texCoord).x, m_RoughnessFactor);
	float ao = lerp(m_AmbientOcclusion, m_AmbientOcclusionMap.Sample(samp, texCoord).w, m_AmbientOcclusionFactor);

	float3 N = normalize(normal);
	float3 V = normalize(camPos - position);

	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);
	float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	float3 kS = F;
	float3 kD = float3(1, 1, 1) - kS;
	kD *= 1 - metallic;

	float3 Lo = float3(0, 0, 0);
	for(int i = 0; i < 1; i++) {

		float3 L = normalize(l_Position - position);
		float3 H = normalize(V + L);

		float distance = length(l_Position - position);
		float attenuation = 1.0 / (distance * distance);
		float3 radiance = l_Color * attenuation;

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);

		float3 nominator = NDF * G * F;
		float denominator = 1 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0) + 0.000001;
		float3 brdf = nominator / denominator;

		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / PI + brdf) * radiance * NdotL;
	}

	float3 ambient = float3(0.03, 0.03, 0.03) * albedo * ao;
	float3 color = ambient + Lo;

	color = color / (color + float3(1, 1, 1));
	color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

	return float4(color, 1);
}