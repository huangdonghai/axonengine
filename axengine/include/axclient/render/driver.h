/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDERDRIVER_H
#define AX_RENDERDRIVER_H

#define ClassName_RenderDriver "gRenderDriver"

namespace Axon { namespace Render {

	struct SelectRecord {
		int name;
		float minz;
		float maxz;
	};
	typedef Sequence<SelectRecord> SelectRecordSeq;

	struct VideoMode {
		uint_t width;
		uint_t height;
		float pixelAspect;
	};


	struct ModelView {
		Vector3 vieworg;
		Matrix4 viewmatrix;
	};

	struct Projection {
		ModelView modelview;
		Matrix4 proj_matrix;
	};

	struct IDriver {
		struct Info {
			enum DriverType {
				OpenGL,
				D3D,
				UNKNOWN
			};

			enum DriverCaps {
				DXT = 1,		// supports DXT compressed texture
				HDR = 2,		// supports HDR rendering
			};

			DriverType driverType;	// opengl, d3d etc...

			// some caps
			int caps;					// DriverCaps
			ShaderQuality highestQualitySupport;

			// for opengl
			String vendor;
			String renderer;
			String version;
			String extension;

			int maxTextureSize;
			int max3DTextureSize;
			int maxCubeMapTextureSize;	// queried from GL

			int maxTextureUnits;		// arb_multitexture
			int maxTextureCoords;		// arb_fragment_program
			int maxTextureImageUnits;	// arb_fragment_program
		};

		// device
		virtual ~IDriver() {}
		virtual void initialize() = 0;
		virtual void finalize() = 0;
		virtual void postInit() = 0;			// after render system is initilized, call this

		// some status
		virtual bool isHDRRendering() = 0;

		// resource management
		virtual Target* createWindowTarget(handle_t wndId, const String& name) = 0;

		// caps
		virtual const Info* getDriverInfo() = 0;
		virtual uint_t getBackendCaps() = 0;

		// if not multi threads rendering, use this call render a frame
		virtual void runFrame() = 0;
	};

}} // namespace Axon::Render

#endif // AX_RENDERDRIVER_H
