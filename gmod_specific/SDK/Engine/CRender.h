#pragma once
class CRender 
{
public:

	virtual void	FrameBegin(void) = 0;
	virtual void	FrameEnd(void) = 0;

	virtual void	ViewSetupVis(bool novis, int numorigins, const Vector origin[]) = 0;

	virtual void	ViewDrawFade(byte *color, IMaterial* pFadeMaterial) = 0;

	virtual void	DrawSceneBegin(void) = 0;
	virtual void	DrawSceneEnd(void) = 0;

	virtual IWorldRenderList * CreateWorldList() = 0;
	virtual void	BuildWorldLists(IWorldRenderList *pList, WorldListInfo_t* pInfo, int iForceViewLeaf, const VisOverrideData_t* pVisData, bool bShadowDepth, float *pReflectionWaterHeight) = 0;
	virtual void	DrawWorldLists(IWorldRenderList *pList, unsigned long flags, float waterZAdjust) = 0;

	// UNDONE: these are temporary functions that will end up on the other
	// side of this interface
	// accessors
	//	virtual const Vector& UnreflectedViewOrigin() = 0;
	virtual const Vector& ViewOrigin(void) = 0;
	virtual const QAngle& ViewAngles(void) = 0;
	virtual CViewSetup const &ViewGetCurrent(void) = 0;
	virtual const VMatrix& ViewMatrix(void) = 0;
	virtual const matrix3x4_t& WorldToScreenMatrix(void) = 0;
	virtual float	GetFramerate(void) = 0;
	virtual float	GetZNear(void) = 0;
	virtual float	GetZFar(void) = 0;

	// Query current fov and view model fov
	virtual float	GetFov(void) = 0;
	virtual float	GetFovY(void) = 0;
	virtual float	GetFovViewmodel(void) = 0;

	// Compute the clip-space coordinates of a point in 3D
	// Clip-space is normalized screen coordinates (-1 to 1 in x and y)
	// Returns true if the point is behind the camera
	virtual bool	ClipTransform(Vector const& point, Vector* pClip) = 0;

	// Compute the screen-space coordinates of a point in 3D
	// This returns actual pixels
	// Returns true if the point is behind the camera
	virtual bool ScreenTransform(Vector const& point, Vector* pScreen) = 0;

	virtual void Push3DView(const CViewSetup &view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes) = 0;
	virtual void Push3DView(const CViewSetup &view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes, ITexture* pDepthTexture) = 0;
	virtual void Push2DView(const CViewSetup &view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes) = 0;
	virtual void PopView(Frustum frustumPlanes) = 0;
	virtual void SetMainView(const Vector &vecOrigin, const QAngle &angles) = 0;
	virtual void ViewSetupVisEx(bool novis, int numorigins, const Vector origin[], unsigned int &returnFlags) = 0;
	virtual void OverrideViewFrustum(Frustum custom) = 0;
	virtual void UpdateBrushModelLightmap(model_t *model, IClientRenderable *Renderable) = 0;
	virtual void BeginUpdateLightmaps(void) = 0;
	virtual void EndUpdateLightmaps(void) = 0;
	virtual bool InLightmapUpdate(void) const = 0;

	const CViewSetup &CurrentView() const { return m_ViewStack.Top().m_View; }
	CViewSetup &CurrentView() { return m_ViewStack.Top().m_View; }

	// Stack of view info
	struct ViewStack_t
	{
		CViewSetup m_View;

		// matrices
		VMatrix	m_matrixView;
		VMatrix	m_matrixProjection;
		VMatrix	m_matrixWorldToScreen;

		bool m_bIs2DView;
		bool m_bNoDraw;
	};


	// Y field of view, calculated from X FOV and screen aspect ratio.
	float			m_yFOV;

	// timing
	double		m_frameStartTime;
	float		m_framerate;

	float		m_zNear;
	float		m_zFar;

	// matrices
	VMatrix		m_matrixView;
	VMatrix		m_matrixProjection;
	VMatrix		m_matrixWorldToScreen;

	CUtlStack< ViewStack_t > m_ViewStack;
	int m_iLightmapUpdateDepth;
};