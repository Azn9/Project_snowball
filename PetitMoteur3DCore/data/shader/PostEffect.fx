struct VS_Sortie
{
    float4 Pos : SV_POSITION;
    float2 CoordTex : TEXCOORD0;
};

cbuffer param {
    float4x4 matInvProj;

    int time;
    float velocity;

    //general
    float far;
    float near;

    //Radial
    float radialDistance;
    float startVelocityRadial;
    float endVelocityRadial;
    float startScreenDistRadial;//0 to sqrt(2)

    //Vignette
    float vignettePower;
    float4 vignetteColor;

    //Depth of field 
    int blurKernelHalfSize;
    float startBlurDist;
    float endBlurDist;

    //speed lines
    uint speedLinesFrameLength;
    float startVelocityLineSpeed;
    float endVelocityLineSpeed;
}

Texture2DArray speedlines;

Texture2D depthTexture; // la texture profondeur
Texture2D textureEntree; // la texture
SamplerState SampleState; // l’état de sampling

//---------------------------------------------------------
// Vertex Shader « Nul »
//---------------------------------------------------------
VS_Sortie NulVS(float4 Pos : POSITION, float2 CoordTex : TEXCOORD0 )
{
    VS_Sortie sortie = (VS_Sortie)0;
    sortie.Pos = Pos;
    sortie.CoordTex = CoordTex;
    return sortie;
}


//-----------------------------------------------------
// Pixel Shader « Speedline »
//-----------------------------------------------------
float4 SpeedlinePS( VS_Sortie vs) : SV_Target
{
    float4 couleur = textureEntree.Sample(SampleState, vs.CoordTex);

    float width, height, elements;
    speedlines.GetDimensions(width, height, elements);

    uint frame = (int(time) / speedLinesFrameLength);
    uint actualFrame = frame % int(elements);

    float speedTexSample = speedlines.Sample(SampleState, float3(vs.CoordTex, actualFrame)).r;

    float4 blanc = float4(1.0,1.0,1.0,1.0);

    float t = smoothstep(startVelocityLineSpeed, endVelocityLineSpeed, velocity);
    return lerp(couleur, blanc, speedTexSample * t);
}


//-----------------------------------------------------
// Pixel Shader « DepthOfField »
//-----------------------------------------------------
float4 BoxBlur(float2 coordTex)
{   
    float2 dimensions;
    textureEntree.GetDimensions(dimensions.x, dimensions.y);
    float2 normalisedPixelSize = 1.0 / dimensions;
    
    float4 couleur = float4(0.0, 0.0, 0.0, 0.0);
    for(int y = -blurKernelHalfSize; y <= blurKernelHalfSize; ++y)
    {
        for(int x = -blurKernelHalfSize; x <= blurKernelHalfSize; ++x)
        {
            float2 sampleCoord = float2(coordTex.x + normalisedPixelSize.x * x, coordTex.y + normalisedPixelSize.y * y);
            float4 sampl = textureEntree.Sample(SampleState, sampleCoord);
            couleur += sampl;
        }
    }
    
    int sampleLengthHeight = blurKernelHalfSize * 2 + 1;
    return couleur / (sampleLengthHeight * sampleLengthHeight);
}

//for testing
float4 BoxBlurPS(VS_Sortie vs) : SV_Target
{   
    return BoxBlur(vs.CoordTex);
}

float4 DepthOfFieldPS(VS_Sortie vs) : SV_Target
{
    float t;
    float4 blurredColor;
    float4 color = textureEntree.Sample(SampleState, vs.CoordTex);
    
    float depth = depthTexture.Sample(SampleState, vs.CoordTex).r;
    float linearDepth = (2.0f * near) / (far + near - depth * (far - near));
    float dist = linearDepth * (far - near) + near;

    t = smoothstep(startBlurDist, endBlurDist, dist);
    blurredColor = BoxBlur(vs.CoordTex);
    
    color = lerp(color, blurredColor, t);
    
    return color;
}

//-----------------------------------------------------
// Pixel Shader « RadialBlur »
//-----------------------------------------------------
float4 RadialBlurPS(VS_Sortie vs) : SV_Target
{
    float4 couleur;
    float4 ct;
    float2 tc = vs.CoordTex;
    float d, dx, dy;

    couleur = 0;
    float x = tc.x*2 - 1.0;
    float y = tc.y*2 - 1.0;
    dx = sqrt(x*x); // Distance du centre
    dy = sqrt(y*y); // Distance du centre

    dx = x * (radialDistance*dx); // Le dégradé (blur) est en fonction de la
    dy = y * (radialDistance*dy); // distance du centre et de la variable distance.

    x = x - (dx*10); // Vous pouvez jouer avec le nombre d’itérations
    y = y - (dy*10);
    tc.x = (x+1.0)/2.0;
    tc.y = (y+1.0)/2.0;

    for (int i = 0; i<10; i++) // Vous pouvez jouer avec le nombred’itérations
    {
        ct = textureEntree.Sample(SampleState, tc);
        couleur = couleur * 0.6 + ct * 0.4; // Vous pouvez « jouer » avec les%
        x = x + dx;
        y = y + dy;
        tc.x = (x+1.0)/2.0;
        tc.y = (y+1.0)/2.0;
    }

    float t = smoothstep(startVelocityRadial, endVelocityRadial, velocity);
    float dist = length(vs.CoordTex - float2(0.5,0.5)) * (2.0f / sqrt(2));
    t = t * smoothstep(startScreenDistRadial, startScreenDistRadial+0.1, dist);

    return lerp(ct, couleur, t);
}

//-----------------------------------------------------
// Pixel Shader « Vignette »
//-----------------------------------------------------
float4 VignettePS(VS_Sortie vs) : SV_Target
{
    float2 texCoord = vs.CoordTex;
    float4 ct = textureEntree.Sample(SampleState, texCoord);

    float dist = length(texCoord - float2(0.5,0.5)) * (2.0f / sqrt(2));

    float t = pow(dist, vignettePower) * vignetteColor.a; // Parce que linéaire c'est pas très beau
    t = min(t, 1.0);

    float4 couleur = ct * (1.0-t) + float4(vignetteColor.rgb, 1.0) * t;

    return couleur;
}


technique11 DepthOfField
{
    pass p0
    {
        VertexShader = compile vs_5_0 NulVS();
        PixelShader = compile ps_5_0 DepthOfFieldPS();
        SetGeometryShader(NULL);
    }
};

technique11 RadialBlur
{
    pass p0
    {
        VertexShader = compile vs_5_0 NulVS();
        PixelShader = compile ps_5_0 RadialBlurPS();
        SetGeometryShader(NULL);
    }
};

technique11 SpeedLine
{
    pass p0
    {
        VertexShader = compile vs_5_0 NulVS();
        PixelShader = compile ps_5_0 SpeedlinePS();
        SetGeometryShader(NULL);
    }
};



/*
technique11 BoxBlurTech
{
    pass p0
    {
        VertexShader = compile vs_5_0 NulVS();
        PixelShader = compile ps_5_0 BoxBlurPS();
        SetGeometryShader(NULL);
    }
};*/



technique11 Vignette
{
    pass p0
    {
        VertexShader = compile vs_5_0 NulVS();
        PixelShader = compile ps_5_0 VignettePS();
        SetGeometryShader(NULL);
    }
};