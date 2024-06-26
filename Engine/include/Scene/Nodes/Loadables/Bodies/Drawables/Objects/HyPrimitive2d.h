/**************************************************************************
 *	HyPrimitive2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPrimitive2d_h__
#define HyPrimitive2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"

class HyAtlasGroupData;

class HyPrimitive2d : public IHyDrawable2d
{
protected:
	glm::vec2 *		m_pVertBuffer;
	uint32			m_uiNumVerts;

	bool			m_bWireframe;
	float			m_fLineThickness;
	uint32			m_uiNumSegments;

	HyRenderMode	m_eRenderMode;
	bool			m_bUpdateShaderUniforms;

	HyShape2d		m_Shape;

public:
	HyPrimitive2d(HyEntity2d *pParent = nullptr);
	HyPrimitive2d(const HyPrimitive2d &copyRef);
	virtual ~HyPrimitive2d(void);

	const HyPrimitive2d &operator=(const HyPrimitive2d &rhs);

	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) override;
	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	HyShapeType GetShapeType() const;

	void SetAsNothing();

	void SetAsShape(const HyShape2d &shapeRef);

	// Set as an isolated edge.
	void SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2);
	void SetAsLineSegment(const b2Vec2 &pt1, const b2Vec2 &pt2);

	// Set as a line loop. This automatically connects last vertex to the first.
	// Passed in parameters are copied, and understood to be local coordinates
	void SetAsLineLoop(const glm::vec2 *pVertices, uint32 uiNumVerts);
	void SetAsLineLoop(const std::vector<glm::vec2> &verticesList);

	// Set as a line chain with isolated end vertices. Passed in parameters are 
	// copied, and understood to be local coordinates
	void SetAsLineChain(const glm::vec2 *pVertices, uint32 uiNumVerts);
	void SetAsLineChain(const std::vector<glm::vec2> &verticesList);

	// Set as a circle with the specified center and radius
	void SetAsCircle(float fRadius);
	void SetAsCircle(const glm::vec2 &ptCenter, float fRadius);
	void SetAsCircle(const b2Vec2 &center, float fRadius);

	// Set as a convex hull from the given array of local points.
	// uiCount must be in the range [3, b2_maxPolygonVertices].
	// The points may be re-ordered, even if they form a convex polygon
	// Collinear points are handled but not removed. Collinear points
	// may lead to poor stacking behavior in physics simulation.
	void SetAsPolygon(const std::vector<glm::vec2> &verticesList);
	void SetAsPolygon(const glm::vec2 *pPointArray, uint32 uiCount);
	void SetAsPolygon(const b2Vec2 *pPointArray, uint32 uiCount);

	void SetAsBox(float fWidth, float fHeight);	// Axis-aligned box, bottom left corner at 0,0
	void SetAsBox(const HyRect &rect);			// Represent an oriented box

	uint32 GetNumVerts() const;
	const glm::vec2 *GetVerts() const;
	void GetCentroid(glm::vec2 &ptCentroidOut) const;

	bool IsWireframe();
	void SetWireframe(bool bIsWireframe);

	float GetLineThickness();
	void SetLineThickness(float fThickness);

	uint32 GetNumCircleSegments();
	void SetNumCircleSegments(uint32 uiNumSegments);

	virtual bool IsLoadDataValid() override;

protected:
	virtual void SetDirty(uint32 uiDirtyFlags) override;

	virtual bool OnIsValidToRender() override;

	virtual void OnUpdateUniforms(float fExtrapolatePercent) override;

	virtual void PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) override;

private:
	virtual void Load() override;

	void ClearVertexData();
	void AssembleData();

	void AssembleLineChain(b2Vec2 *pVertexList, uint32 uiNumVertices);
	void AssembleCircle(glm::vec2 ptCenter, float fRadius, uint32 uiSegments);
	void AssemblePolygon(const b2Vec2 *pVertexList, uint32 uiNumVertices);

	static void OnShapeChanged(void *pData);

private: // Hide inherited functionality that doesn't exist for primitives
	using IHyLoadable::GetState;
	using IHyLoadable::SetState;
	using IHyLoadable::GetNumStates;
	using IHyLoadable::GetName;
	using IHyLoadable::GetPrefix;
	using IHyLoadable2d::Init;
	using IHyLoadable2d::Uninit;
};

#endif /* HyPrimitive2d_h__ */
