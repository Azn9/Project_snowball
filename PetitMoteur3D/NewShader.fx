struct Light {
	bool initialized;
	int lightType; // 0 = ambient, 1 = directional, 2 = point, 3 = spot
	float4 position;
	float4 direction;
	
	float4 ambiant;
	float4 diffuse;
	float4 specular;
	float specularPower;

	// Only for spot
	float innerAngle;
	float outerAngle;
};

cbuffer param
{
    float4x4 matWorldViewProj; // la matrice totale 
    float4x4 matWorld; // matrice de transformation dans le monde
    float4 vCamera; // la position de la caméra
    float4 vAMat; // la valeur ambiante du matériau
    float4 vDMat; // la valeur diffuse du matériau
    float4 vSMat; // la valeur spéculaire du matériau
    float puissance; // la puissance de spécularité
    int bTex; // Booléen pour la présence de texture
    float2 remplissage;
};

#define MAX_LIGHTS 10
StructuredBuffer<Light> lights; //  : register(t0)

struct VS_Sortie
{
	float4 Pos : SV_Position;
	float3 Norm :    TEXCOORD0;
	float3 vDirCam : TEXCOORD1;
	float2 coordTex : TEXCOORD2;
};

VS_Sortie MainVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex: TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = mul(Pos, matWorldViewProj);
	sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

	float3 PosWorld = mul(Pos, matWorld).xyz;

	sortie.vDirCam = vCamera.xyz - PosWorld;

	// Coordonnées d’application de texture
	sortie.coordTex = coordTex;

	return sortie;
}

Texture2D textureEntree; // la texture
SamplerState SampleState; // l’état de sampling

float4 MainPS(VS_Sortie vs) : SV_Target
{
	float3 totalAmbiant = float3(0, 0, 0);
	float3 totalDiffuse = float3(0, 0, 0);
	float3 totalSpecular = float3(0, 0, 0);

	// Normaliser les param�tres
	float3 N = normalize(vs.Norm);
	float3 V = normalize(vs.vDirCam);


	for (uint i = 0; i < MAX_LIGHTS; ++i) {
		Light li = lights[i];

		if (!li.initialized)
			continue;

		totalAmbiant += li.ambiant.rgb;

		if (li.lightType == 0) // ambiant
		{
			continue;
		}
		else if (li.lightType == 1) // Directionnal
		{
			float3 L = normalize(-li.direction.xyz);
            float3 diff = saturate(dot(N, L));
            float3 R = normalize(2 * diff * N - L);
			float3 S = pow(saturate(dot(R, V)), li.specularPower);

			totalDiffuse += li.diffuse.rgb * diff;
			totalSpecular += li.specular.rgb * S;
		}
		else if (li.lightType == 2) // Point
		{
			float3 L = normalize(li.position.xyz - vs.Pos.xyz);
            float3 diff = saturate(dot(N, L));
            float3 R = normalize(2 * diff * N - L);
			float3 S = pow(saturate(dot(R, V)), li.specularPower);
            
			totalDiffuse += li.diffuse.rgb * diff;
			totalSpecular += li.specular.rgb * S;
		}
		else if (li.lightType == 3) // Spot
		{
			// TODO
		}
	}

	// Échantillonner la couleur du pixel à partir de la texture
	float3 couleurTexture = textureEntree.Sample(SampleState, vs.coordTex).rgb;

	if (bTex > 0)
    {
        // Échantillonner la couleur du pixel à partir de la texture
        couleurTexture = textureEntree.Sample(SampleState, vs.coordTex).rgb;
    }
    else
    {
        couleurTexture = float3(1, 1, 1);
    }

	float3 finalColor = couleurTexture.rgb * (totalAmbiant * vAMat.rgb + totalDiffuse * vDMat.rgb + totalSpecular * vSMat.rgb);

	return float4(finalColor, 1.0f);
}

technique11 NewShader
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0, MainVS()));
		SetPixelShader(CompileShader(ps_5_0, MainPS()));
		SetGeometryShader(NULL);
	}
}