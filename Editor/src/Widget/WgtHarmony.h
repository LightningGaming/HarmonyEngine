/**************************************************************************
*	WgtHarmony.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef WGTHARMONY_H
#define WGTHARMONY_H

#include "Project.h"

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QTimer>

class WgtHarmony : public QOpenGLWidget//, protected QOpenGLFunctions
{
	Q_OBJECT

	static bool					sm_bHarmonyLoaded;
	Project *					m_pProject;

	class GuiHyEngine : public HyEngine
	{
		Project *				m_pProject;

	public:
		GuiHyEngine(HarmonyInit &initStruct, Project *pProject) :
			HyEngine(initStruct),
			m_pProject(pProject)
		{
			HyEngine::Window().CreateCamera2d();
			//m_pProject->HarmonyInitialize();
		}

		~GuiHyEngine() {
			m_pProject->HarmonyShutdown();
		}

	protected:
		virtual bool OnUpdate() override {
			return m_pProject->HarmonyUpdate();
		}
	};
	GuiHyEngine *				m_pHyEngine;

	QTimer *					m_pTimer;

public:
	WgtHarmony(QWidget *pParent = nullptr);
	virtual ~WgtHarmony();

	void InitProject(Project *pProject);

	Project *GetProject();

	bool IsProject(Project *pProjectToTest);
	void CloseProject();

	HyRendererInterop *GetHarmonyRenderer();

protected:
	// QOpenGLWidget overrides
	virtual void initializeGL() override;
	virtual void paintGL() override;
	virtual void resizeGL(int w, int h) override;
	//virtual bool event(QEvent *pEvent) override;

	// QWidget overrides
	virtual void enterEvent(QEvent *pEvent) override;
	virtual void leaveEvent(QEvent *pEvent) override;
	virtual void dragEnterEvent(QDragEnterEvent *pEvent) override;
	virtual void dropEvent(QDropEvent *pEvent) override;
	virtual void keyPressEvent(QKeyEvent *pEvent) override;
	virtual void keyReleaseEvent(QKeyEvent *pEvent) override;
	virtual void wheelEvent(QWheelEvent *pEvent) override;
	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void mousePressEvent(QMouseEvent *pEvent) override;
	virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;

private Q_SLOTS:
	void OnBootCheck();

Q_SIGNALS:
	void HarmonyWidgetReady(WgtHarmony *pWidget);
};

#endif // WGTHARMONY_H
