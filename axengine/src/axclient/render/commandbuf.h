#ifndef AX_COMMANDBUF_H
#define AX_COMMANDBUF_H

AX_BEGIN_NAMESPACE

struct CommandBuf
{
	enum CommandId {
		DELETE_TEX,
		CREATE_TEX,
		UPLOAD_SUBTEX,
	};

	struct Command {
		short commandId;
		short commandSize;
	};

	struct DeleteTexCmd : public Command {
		SamplerData *sampler;
	};

	struct CreateTextureCmd : public Command {
		SamplerData *sampler;
	};

	struct UploadSubTexCmd : public Command {
		SamplerData *sampler;
		Rect rect;
		const void *pixel;
		TexFormat format;
		Rgba color; // if pixel is null, then use this color
	};

	enum {
		MaxBufSize = 64 * 1024
	};

	int bufSize;
	byte_t buf[MaxBufSize];
};

AX_END_NAMESPACE

#endif // AX_COMMANDBUF_H
