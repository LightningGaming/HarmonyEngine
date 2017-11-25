#include "CustomShader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char * const szCHECKERGRID_VERTEXSHADER = R"src(
#version 430

uniform mat4					u_mtxTransform;
uniform mat4					u_mtxWorldToCamera;
uniform mat4					u_mtxCameraToClip;

layout(location = 0) in vec2    attr_vPosition;
layout(location = 1) in vec2    attr_vUVcoord;

smooth out vec2                 interp_vUV;

void main()
{
    interp_vUV.x = attr_vUVcoord.x;
    interp_vUV.y = attr_vUVcoord.y;

    vec4 vTemp = u_mtxTransform * vec4(attr_vPosition, 0, 1);
    vTemp = u_mtxWorldToCamera * vTemp;
    gl_Position = u_mtxCameraToClip * vTemp;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char *const szCHECKERGRID_FRAGMENTSHADER = R"src(
#version 400

uniform float                   u_fGridSize;
uniform vec2                    u_vDimensions;
uniform vec4                    u_vGridColor1;
uniform vec4                    u_vGridColor2;

smooth in vec2                  interp_vUV;
out vec4                        out_vColor;

void main()
{
    vec2 vScreenCoords = (interp_vUV * u_vDimensions) / u_fGridSize;
    out_vColor = mix(u_vGridColor1, u_vGridColor2, step((float(int(floor(vScreenCoords.x) + floor(vScreenCoords.y)) & 1)), 0.9));
}
)src";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CustomShader::CustomShader(HarmonyInit &initStruct) :	IHyApplication(initStruct),
														m_CheckerGrid(400.0f, 400.0f, 25.0f, nullptr)
{
}

CustomShader::~CustomShader()
{
}

/*virtual*/ bool CustomShader::Initialize()
{
	m_pCamera = Window().CreateCamera2d();

	HyShader *pShader_CheckerGrid = IHyRenderer::MakeCustomShader();
	pShader_CheckerGrid->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
	pShader_CheckerGrid->AddVertexAttribute("attr_vPosition", HYSHADERVAR_vec2);
	pShader_CheckerGrid->AddVertexAttribute("attr_vUVcoord", HYSHADERVAR_vec2);
	pShader_CheckerGrid->SetSourceCode(szCHECKERGRID_FRAGMENTSHADER, HYSHADER_Fragment);
	pShader_CheckerGrid->Finalize(HYSHADERPROG_Primitive);

	m_CheckerGrid.SetCustomShader(pShader_CheckerGrid);
	m_CheckerGrid.Load();

	return true;
}

/*virtual*/ bool CustomShader::Update()
{
	return true;
}

/*virtual*/ void CustomShader::Shutdown()
{
}