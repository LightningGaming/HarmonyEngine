/**************************************************************************
*	HyShader.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
**************************************************************************/
#include "Renderer/Effects/HyShader.h"
#include "Renderer/IHyRenderer.h"

/*static*/ HyShaderHandle HyShader::sm_hHandleCount = 0;

HyShader::HyShader(HyShaderProgramDefaults eDefaultsFrom) : IHyLoadableData(HYLOADABLE_Shader),
															m_hHANDLE(++sm_hHandleCount),
															m_eDEFAULTS_FROM(eDefaultsFrom),
															m_bIsFinalized(false),
															m_uiStride(0)
{
	for(int i = 0; i < HYNUMSHADERTYPES; ++i)
		m_sSourceCode[i].clear();
}

HyShader::~HyShader()
{
}

void HyShader::Destroy()
{
	IHyRenderer::RemoveShader(this);
	delete this;
}

HyShaderHandle HyShader::GetHandle()
{
	return m_hHANDLE;
}

bool HyShader::IsFinalized()
{
	return m_bIsFinalized;
}

int32 HyShader::GetStride()
{
	return static_cast<int32>(m_uiStride);
}

const std::string &HyShader::GetSourceCode(HyShaderType eType)
{
	return m_sSourceCode[eType];
}

void HyShader::SetSourceCode(std::string sSource, HyShaderType eType)
{
	if(sSource.empty())
		return;

	m_sSourceCode[eType] = sSource;
}

/*virtual*/ void HyShader::AddVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize /*= false*/, uint32 uiInstanceDivisor /*= 0*/)
{
	if(szName == nullptr)
		return;

	HyShaderVertexAttribute vertAttrib;
	vertAttrib.sName = szName;
	vertAttrib.eVarType = eVarType;
	vertAttrib.bNormalized = bNormalize;
	vertAttrib.uiInstanceDivisor = uiInstanceDivisor;

	m_VertexAttributeList.push_back(vertAttrib);
}

std::vector<HyShaderVertexAttribute> &HyShader::GetVertextAttributes()
{
	return m_VertexAttributeList;
}

void HyShader::Finalize()
{
	IHyRenderer::AddShader(this);
	m_bIsFinalized = true;
}

/*virtual*/ void HyShader::OnLoadThread() /*override*/
{
}

/*virtual*/ void HyShader::OnRenderThread(IHyRenderer &rendererRef) /*override*/
{
	rendererRef.UploadShader(m_eDEFAULTS_FROM, this);

	// Calculate the stride based on the specified vertex attributes
	m_uiStride = 0;
	for(uint32 i = 0; i < static_cast<uint32>(m_VertexAttributeList.size()); ++i)
	{
		switch(m_VertexAttributeList[i].eVarType)
		{
		case HYSHADERVAR_bool:		m_uiStride += sizeof(bool);			break;
		case HYSHADERVAR_int:		m_uiStride += sizeof(int32);		break;
		case HYSHADERVAR_uint:		m_uiStride += sizeof(uint32);		break;
		case HYSHADERVAR_float:		m_uiStride += sizeof(float);		break;
		case HYSHADERVAR_double:	m_uiStride += sizeof(double);		break;
		case HYSHADERVAR_bvec2:		m_uiStride += sizeof(glm::bvec2);	break;
		case HYSHADERVAR_bvec3:		m_uiStride += sizeof(glm::bvec3);	break;
		case HYSHADERVAR_bvec4:		m_uiStride += sizeof(glm::bvec4);	break;
		case HYSHADERVAR_ivec2:		m_uiStride += sizeof(glm::ivec2);	break;
		case HYSHADERVAR_ivec3:		m_uiStride += sizeof(glm::ivec3);	break;
		case HYSHADERVAR_ivec4:		m_uiStride += sizeof(glm::ivec4);	break;
		case HYSHADERVAR_vec2:		m_uiStride += sizeof(glm::vec2);	break;
		case HYSHADERVAR_vec3:		m_uiStride += sizeof(glm::vec3);	break;
		case HYSHADERVAR_vec4:		m_uiStride += sizeof(glm::vec4);	break;
		case HYSHADERVAR_dvec2:		m_uiStride += sizeof(glm::dvec2);	break;
		case HYSHADERVAR_dvec3:		m_uiStride += sizeof(glm::dvec3);	break;
		case HYSHADERVAR_dvec4:		m_uiStride += sizeof(glm::dvec4);	break;
		case HYSHADERVAR_mat3:		m_uiStride += sizeof(glm::mat3);	break;
		case HYSHADERVAR_mat4:		m_uiStride += sizeof(glm::mat4);	break;
		}
	}
}