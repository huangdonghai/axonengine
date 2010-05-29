/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_OUTDOORENV_H
#define AX_RENDER_OUTDOORENV_H

AX_BEGIN_NAMESPACE

struct NishitaParams {
	Vector3 sunPos;

	bool operator==(const NishitaParams &rhs) {
		return sunPos == rhs.sunPos;
	}

	bool operator!=(const NishitaParams &rhs) {
		return !operator==(rhs);
	}
};

class AX_API OutdoorEnv : public RenderEntity {
public:
	OutdoorEnv(RenderWorld *world);
	~OutdoorEnv();

	void setTimeOfDay(TimeOfDay &tod);
	void setDateTime(const DateTime &datetime);

	void setHaveOcean(bool have);
	void setHaveFarSky(bool have);
	void setHaveGlobalLight(bool have);
	void setFog(const Vector3 &color, float density);
	void setOceanFog(const Vector3 &color, float density);
	void setOceanMaterial(const String &matname);
	void setSunColor(const Color3 &color, float intensity, float specularX);
	void setCastShadow(bool val);
	void setSunDir(const Vector3 &dir);
	void setSkyColor(const Color3 &color, float intensity);
	void setEnvColor(const Color3 &color, float intensity);
	void setSkyBoxTexture(const String &matname);

	RenderLight *getGlobalLight() const;

	// generate render primitive
	virtual BoundingBox getLocalBoundingBox();
	virtual BoundingBox getBoundingBox();
	virtual Kind getType() const { return kOutdoorEnv; }

	// new interface
	virtual void frameUpdate(RenderScene *qscene);
	virtual void issueToQueue(RenderScene *qscene);

protected:
	void createSkyBox();
	void createSkyDome();
	void createOceanMesh();
	void createOceanGrid();

	void genNishitaUpdateScene(RenderScene *qscene);

private:
	// flags
	bool m_haveSky : 1;
	bool m_nishitaSky : 1;
	bool m_haveOcean : 1;
	bool m_haveGlobalLight : 1;

	// skybox
	String m_skyBoxMatName;
	MeshPrim *m_skybox12;
	MeshPrim *m_skybox34;
	MeshPrim *m_skybox5;
	MeshPrim *m_skydome;
	MeshPrim *m_oceanMesh;

	Material *m_skyNishitaMat;
	Material *m_skyNishitaGenMat;
	RenderTarget *m_skyNishitaRt;		// render target for nishita sky
	NishitaParams m_lastNishitaParams;

	bool m_dateTimeInited;
	DateTime m_dateTime;

	RenderLight *m_globalLight;
	RenderFog *m_globalFog;
	RenderFog *m_oceanFog;

	RenderWind m_wind;
};

AX_END_NAMESPACE


#endif // end guardian

