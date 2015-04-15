Shader "Custom/Terrain Surfaces" { 

Properties 
{
	_Color ("Main Color", Color) = (1,1,1,1)
	_Blend ("Blend Area", Range(0, 1)) = 0.2
	_DesertTopTex ("Desert Top Tex (RGB) Gloss (A)", 2D) = "white" {}
	_DesertSideTex ("Desert Side Tex (RGB) Gloss (A)", 2D) = "white" {}
	_DesertBotTex ("Desert Bot Tex (RGB) Gloss (A)", 2D) = "white" {}
	_DirtTopTex("Dirt Top Tex (RGB) Gloss (A)", 2D) = "white" {}
	_DirtSideTex("Dirt Side Tex (RGB) Gloss (A)", 2D) = "white" {}
	_DirtBotTex("Dirt Bot Tex (RGB) Gloss (A)", 2D) = "white" {}
	_RockTopTex("Rock Top Tex (RBG) Gloss (A)", 2D) = "white" {}
	_RockSideTex("Rock Side Tex (RBG) Gloss (A)", 2D) = "white" {}
	_RockBotTex("Rock Bot Tex (RBG) Gloss (A)", 2D) = "white" {}
	_SpecTex("Sparkles are love, sparkles are life", 2D) = "white" {}
	_SpecColor ("Specular Color", Color) = (0.5, 0.5, 0.5, 1)
	_Shininess ("Shininess", Range (0.03, 1)) = 0.078125
}

SubShader
{
	Tags { "RenderType"="Opaque" }
	LOD 600

	CGPROGRAM
	#pragma surface surf BlinnPhong vertex:vert
	#pragma target 3.0
	#include "UnityCG.cginc"

	fixed4 _Color;
	float3 _Division;
	float3 _Normal;
	half _Blend;
	sampler2D _DesertTopTex;
	sampler2D _DesertSideTex;
	sampler2D _DesertBotTex;
	sampler2D _DirtTopTex;
	sampler2D _DirtSideTex;
	sampler2D _DirtBotTex;
	sampler2D _RockTopTex;
	sampler2D _RockSideTex;
	sampler2D _RockBotTex;
	sampler2D _SpecTex;
	half _Shininess;

	struct Input
	{
		float4 topUVs;
		float4 sideUVs;
		float4 vertColor;
		// x = side, y = top, z = front, w = bottom
		float4 blendColor;
		float3 vert_norm;
		
		// for some reason i cant add any more variables without compiler issues
	};
	
	float4 _DesertTopTex_ST;
	float4 _DesertSideTex_ST;
	float4 _DesertBotTex_ST;
	float4 _DirtTopTex_ST;
	float4 _DirtSideTex_ST;
	float4 _DirtBotTex_ST;
	float4 _RockTopTex_ST;
	float4 _RockSideTex_ST;
	float4 _RockBotTex_ST;
	float4 _SpecTex_ST;
	
	void vert(inout appdata_full v, out Input o)
	{
		#if defined(SHADER_API_D3D11) || defined(SHADER_API_D3D11_9X)
			UNITY_INITIALIZE_OUTPUT(Input,o);
		#endif
		
		// do dot products to determine front/side/bottom
		float4 worldPos = mul(_Object2World, v.vertex);
		float3 normDist = normalize(worldPos);
		float3 normNormal = normalize(v.normal);
		float dotProduct = dot(normDist, normNormal);
		if (dotProduct >= 0)
		{
			// side ratio
			o.blendColor.x = 1 - dotProduct;
			// top ratio
			o.blendColor.y = dotProduct;
			o.blendColor.z = 1 - dotProduct;
			// no bottom because dotproduct is positive
			o.blendColor.w = 0;
		}
		else
		{
			o.blendColor.x = dotProduct + 1;
			o.blendColor.y = 0;
			o.blendColor.z = dotProduct + 1;
			o.blendColor.w = abs(dotProduct);
		}
		half m = max(o.blendColor.x, max(o.blendColor.y, o.blendColor.z));	// get maximum component
		o.blendColor.x *= (o.blendColor.x == m ? 1 : _Blend);		// if not the max then scale down by the _Blend factor
		o.blendColor.y *= (o.blendColor.y == m ? 1 : _Blend);
		o.blendColor.z *= (o.blendColor.z == m ? 1 : _Blend);
		o.blendColor = saturate(o.blendColor);		// probably don't need this, nothing should get out of range.
		o.blendColor = normalize(o.blendColor);

		// Color based on vertex color, w component = blend factor
		// Red = soil, Green = sand, Blue = rock
		
		o.vertColor.w = 0.0f;
		o.vertColor.x = v.color.r;
		o.vertColor.y = v.color.g;
		o.vertColor.z = v.color.b;

		o.vert_norm.xyz = abs(v.normal);	// abs of the normal.
		m = max(o.vert_norm.x, max(o.vert_norm.y, o.vert_norm.z));	// get maximum component
		o.vert_norm.x *= (o.vert_norm.x == m ? 1 : _Blend);		// if not the max then scale down by the _Blend factor
		o.vert_norm.y *= (o.vert_norm.y == m ? 1 : _Blend);
		o.vert_norm.z *= (o.vert_norm.z == m ? 1 : _Blend);
		if (o.vert_norm.x == m && (o.vert_norm.x == o.vert_norm.y || o.vert_norm.x == o.vert_norm.z))
		{
			o.vert_norm.x = _Blend;
		}
		if (o.vert_norm.y == m && o.vert_norm.y == o.vert_norm.z)
		{
			o.vert_norm.y = _Blend;
		}
		o.vert_norm = saturate(o.vert_norm);		// probably don't need this, nothing should get out of range.
		o.vert_norm = normalize(o.vert_norm);

		float2 top = v.vertex.xz;
		float2 front = v.vertex.zy;
		float2 side = v.vertex.xy;
		float2 bot = v.vertex.xz;

		float dotTop = degrees(acos(dot(normDist, float3(0, 1, 0)))) / 90;
		float dotFront = degrees(acos(dot(normDist, float3(0, 0, 1)))) / 90;
		float dotSide = degrees(acos(dot(normDist, float3(1, 0, 0)))) / 90;

		//o.vert_norm = normalize(float3(dotSide, dotTop, dotFront));

		float3 dots = normalize(float3(dotSide, dotTop, dotFront));
		float2 topUV = TRANSFORM_TEX(side, _DirtTopTex) * dotSide + TRANSFORM_TEX(top, _DirtTopTex) * dotTop + TRANSFORM_TEX(front, _DirtTopTex) * dotFront;
		float2 frontUV = TRANSFORM_TEX(side, _DirtSideTex) * dotSide + TRANSFORM_TEX(top, _DirtSideTex) * dotTop + TRANSFORM_TEX(front, _DirtSideTex) * dotFront;
		float2 sideUV = TRANSFORM_TEX(side, _DirtSideTex) * dotSide + TRANSFORM_TEX(top, _DirtSideTex) * dotTop + TRANSFORM_TEX(front, _DirtSideTex) * dotFront;
		float2 botUV = TRANSFORM_TEX(side, _DirtBotTex) * dotSide + TRANSFORM_TEX(top, _DirtBotTex) * dotTop + TRANSFORM_TEX(front, _DirtBotTex) * dotFront;

		//use grass tex as UV template
		o.topUVs.xy = TRANSFORM_TEX(top, _DirtTopTex);
		o.topUVs.zw = TRANSFORM_TEX(bot, _DirtSideTex);
		o.sideUVs.xy = TRANSFORM_TEX(front, _DirtSideTex);
		o.sideUVs.zw = TRANSFORM_TEX(side, _DirtBotTex);
	}
	
	void surf (Input IN, inout SurfaceOutput o)
	{	
		float4 blend = normalize(IN.vertColor);

		float topBlend = blend.g * IN.blendColor.y;
		float botBlend = blend.g * IN.blendColor.w;
		float frontBlend = blend.g * IN.blendColor.z;
		float sideBlend = blend.g * IN.blendColor.x;

		fixed4 specTex = tex2D(_DesertTopTex, IN.topUVs.xy) * topBlend * IN.vert_norm.y;
		specTex += tex2D(_SpecTex, IN.sideUVs.xy) * topBlend * IN.vert_norm.x;
		specTex += tex2D(_SpecTex, IN.sideUVs.zw) * topBlend * IN.vert_norm.z;
						  
		specTex += tex2D(_SpecTex, IN.topUVs.zw) * botBlend * IN.vert_norm.y;
		specTex += tex2D(_SpecTex, IN.sideUVs.xy) * botBlend * IN.vert_norm.x;
		specTex += tex2D(_SpecTex, IN.sideUVs.zw) * botBlend * IN.vert_norm.z;
						  
		specTex += tex2D(_SpecTex, IN.topUVs.xy) * frontBlend * IN.vert_norm.y;
		specTex += tex2D(_SpecTex, IN.sideUVs.xy) * frontBlend * IN.vert_norm.x;
		specTex += tex2D(_SpecTex, IN.sideUVs.zw) * frontBlend * IN.vert_norm.z;
						  
		specTex += tex2D(_SpecTex, IN.topUVs.xy) * sideBlend * IN.vert_norm.y;
		specTex += tex2D(_SpecTex, IN.sideUVs.xy) * sideBlend * IN.vert_norm.x;
		specTex += tex2D(_SpecTex, IN.sideUVs.zw) * sideBlend * IN.vert_norm.z;

		fixed4 desertTex = tex2D(_DesertTopTex, IN.topUVs.xy) * topBlend * IN.vert_norm.y;
		desertTex += tex2D(_DesertTopTex, IN.sideUVs.xy) * topBlend * IN.vert_norm.x;
		desertTex += tex2D(_DesertTopTex, IN.sideUVs.zw) * topBlend * IN.vert_norm.z;

		desertTex += tex2D(_DesertBotTex, IN.topUVs.zw) * botBlend * IN.vert_norm.y;
		desertTex += tex2D(_DesertBotTex, IN.sideUVs.xy) * botBlend * IN.vert_norm.x;
		desertTex += tex2D(_DesertBotTex, IN.sideUVs.zw) * botBlend * IN.vert_norm.z;

		desertTex += tex2D(_DesertSideTex, IN.topUVs.xy) * frontBlend * IN.vert_norm.y;
		desertTex += tex2D(_DesertSideTex, IN.sideUVs.xy) * frontBlend * IN.vert_norm.x;
		desertTex += tex2D(_DesertSideTex, IN.sideUVs.zw) * frontBlend * IN.vert_norm.z;

		desertTex += tex2D(_DesertSideTex, IN.topUVs.xy) * sideBlend * IN.vert_norm.y;
		desertTex += tex2D(_DesertSideTex, IN.sideUVs.xy) * sideBlend * IN.vert_norm.x;
		desertTex += tex2D(_DesertSideTex, IN.sideUVs.zw) * sideBlend * IN.vert_norm.z;

		topBlend = blend.r * IN.blendColor.y;
		botBlend = blend.r * IN.blendColor.w;
		frontBlend = blend.r * IN.blendColor.z;
		sideBlend = blend.r * IN.blendColor.x;

		fixed4 dirtTex = tex2D(_DirtTopTex, IN.topUVs.xy) * topBlend * IN.vert_norm.y;
		dirtTex += tex2D(_DirtTopTex, IN.sideUVs.xy) * topBlend * IN.vert_norm.x;
		dirtTex += tex2D(_DirtTopTex, IN.sideUVs.zw) * topBlend * IN.vert_norm.z;
						  
		dirtTex += tex2D(_DirtBotTex, IN.topUVs.zw) * botBlend * IN.vert_norm.y;
		dirtTex += tex2D(_DirtBotTex, IN.sideUVs.xy) * botBlend * IN.vert_norm.x;
		dirtTex += tex2D(_DirtBotTex, IN.sideUVs.zw) * botBlend * IN.vert_norm.z;

		dirtTex += tex2D(_DirtSideTex, IN.topUVs.xy) * frontBlend * IN.vert_norm.y;
		dirtTex += tex2D(_DirtSideTex, IN.sideUVs.xy) * frontBlend * IN.vert_norm.x;
		dirtTex += tex2D(_DirtSideTex, IN.sideUVs.zw) * frontBlend * IN.vert_norm.z;
						  
		dirtTex += tex2D(_DirtSideTex, IN.topUVs.xy) * sideBlend * IN.vert_norm.y;
		dirtTex += tex2D(_DirtSideTex, IN.sideUVs.xy) * sideBlend * IN.vert_norm.x;
		dirtTex += tex2D(_DirtSideTex, IN.sideUVs.zw) * sideBlend * IN.vert_norm.z;

		topBlend = blend.b * IN.blendColor.y;
		botBlend = blend.b * IN.blendColor.w;
		frontBlend = blend.b * IN.blendColor.z;
		sideBlend = blend.b * IN.blendColor.x;

		fixed4 rockTex = tex2D(_RockTopTex, IN.topUVs.xy) * topBlend * IN.vert_norm.y;
		rockTex += tex2D(_RockTopTex, IN.sideUVs.xy) * topBlend * IN.vert_norm.x;
		rockTex += tex2D(_RockTopTex, IN.sideUVs.zw) * topBlend * IN.vert_norm.z;
						  
		rockTex += tex2D(_RockBotTex, IN.topUVs.zw) * botBlend * IN.vert_norm.y;
		rockTex += tex2D(_RockBotTex, IN.sideUVs.xy) * botBlend * IN.vert_norm.x;
		rockTex += tex2D(_RockBotTex, IN.sideUVs.zw) * botBlend * IN.vert_norm.z;
						  
		rockTex += tex2D(_RockSideTex, IN.topUVs.xy) * frontBlend * IN.vert_norm.y;
		rockTex += tex2D(_RockSideTex, IN.sideUVs.xy) * frontBlend * IN.vert_norm.x;
		rockTex += tex2D(_RockSideTex, IN.sideUVs.zw) * frontBlend * IN.vert_norm.z;
						  
		rockTex += tex2D(_RockSideTex, IN.topUVs.xy) * sideBlend * IN.vert_norm.y;
		rockTex += tex2D(_RockSideTex, IN.sideUVs.xy) * sideBlend * IN.vert_norm.x;
		rockTex += tex2D(_RockSideTex, IN.sideUVs.zw) * sideBlend * IN.vert_norm.z;


		// how to mix colors... addition or multiplication.... D:
		fixed4 col = normalize(desertTex + dirtTex + rockTex);
		//col.rgb = IN.vertColor.xyz;
		o.Albedo = col.rgb * _Color.rgb;
		o.Emission = half3(0.05f, 0.05f, 0.05f);
		// gloss is fucked
		o.Gloss = blend.g * specTex.rgb * _Shininess;

		//o.Specular = _Shininess * specTex.rgb;

		}
		ENDCG
	}
	FallBack "Diffuse"
}