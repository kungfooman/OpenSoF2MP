//Anything above this #include will be ignored by the compiler
#include "qcommon/exe_headers.h"
// this include must remain at the top of every CPP file
#include "client.h"

#if !defined(FX_SCHEDULER_H_INC)
	#include "FxScheduler.h"
#endif
#include "ghoul2/G2.h"

cvar_t	*fx_debug;
#ifdef _SOF2DEV_
cvar_t	*fx_freeze;
#endif
cvar_t	*fx_countScale;
cvar_t	*fx_nearCull;
cvar_t	*fx_flashRadius;

#define DEFAULT_EXPLOSION_RADIUS	512

// Stuff for the FxHelper
//------------------------------------------------------
SFxHelper::SFxHelper(void) :
	mTime(0),
	mOldTime(0),
	mFrameTime(0),
	mTimeFrozen(false),
	refdef(0)
{
}
 
void SFxHelper::ReInit(refdef_t* pRefdef)
{
	mTime = 0;
	mOldTime = 0;
	mFrameTime = 0;
	mTimeFrozen = false;
	refdef = pRefdef;
}

//------------------------------------------------------
void SFxHelper::Print( const char *msg, ... )
{
	va_list		argptr;
	char		text[1024];

	va_start( argptr, msg );
	Q_vsnprintf(text, sizeof(text), msg, argptr);
	va_end( argptr );

	Com_DPrintf( text );
}

//------------------------------------------------------
void SFxHelper::AdjustTime( int frametime )
{
#ifdef _SOF2DEV_
	if ( fx_freeze->integer || ( frametime <= 0 ))
#else
	if ( frametime <= 0 )
#endif
	{
		// Allow no time progression when we are paused.
		mFrameTime = 0;
		mRealTime = 0.0f;
	}
	else
	{
		mOldTime = mTime;
		mTime = frametime;
		mFrameTime = mTime - mOldTime;
		
		mRealTime = mFrameTime * 0.001f;


/*		mFrameTime = frametime;
		mTime += mFrameTime;
		mRealTime = mFrameTime * 0.001f;*/

//		mHalfRealTimeSq = mRealTime * mRealTime * 0.5f;
	}
}

//------------------------------------------------------
void SFxHelper::CameraShake( vec3_t origin, float intensity, int radius, int time )
{
	TCGCameraShake	*data = (TCGCameraShake *)cl.mSharedMemory;

	VectorCopy(origin, data->mOrigin);
	data->mIntensity = intensity;
	data->mRadius = radius;
	data->mTime = time;

	VM_Call( cgvm, CG_FX_CAMERASHAKE ); 
}

//------------------------------------------------------
qboolean SFxHelper::GetOriginAxisFromBolt(CGhoul2Info_v *pGhoul2, int mEntNum, int modelNum, int boltNum, vec3_t /*out*/origin, vec3_t /*out*/axis[3])
{
	qboolean doesBoltExist;
	mdxaBone_t 		boltMatrix;

	TCGVectorData *data = (TCGVectorData*)cl.mSharedMemory;
	data->mEntityNum = mEntNum;

	VM_Call(cgvm, CG_GET_LERP_ANGLES);
	vec3_t lerpAngles;
	VectorCopy(data->mPoint, lerpAngles);

	VM_Call(cgvm, CG_GET_LERP_ORIGIN);
	vec3_t lerpOrigin;
	VectorCopy(data->mPoint, lerpOrigin);

	VM_Call(cgvm, CG_GET_MODEL_SCALE);
	vec3_t modelScale;
	VectorCopy(data->mPoint, modelScale);

	// go away and get me the bolt position for this frame please
	doesBoltExist = re.G2API_GetBoltMatrix(*pGhoul2, modelNum, boltNum, 
		&boltMatrix, lerpAngles, lerpOrigin, theFxHelper.mOldTime, 0, modelScale);

	if (doesBoltExist)
	{	// set up the axis and origin we need for the actual effect spawning
	   	origin[0] = boltMatrix.matrix[0][3];
		origin[1] = boltMatrix.matrix[1][3];
		origin[2] = boltMatrix.matrix[2][3];

		axis[1][0] = boltMatrix.matrix[0][0];
		axis[1][1] = boltMatrix.matrix[1][0];
		axis[1][2] = boltMatrix.matrix[2][0];

		axis[0][0] = boltMatrix.matrix[0][1];
		axis[0][1] = boltMatrix.matrix[1][1];
		axis[0][2] = boltMatrix.matrix[2][1];

		axis[2][0] = boltMatrix.matrix[0][2];
		axis[2][1] = boltMatrix.matrix[1][2];
		axis[2][2] = boltMatrix.matrix[2][2];
	}
	return doesBoltExist;
}