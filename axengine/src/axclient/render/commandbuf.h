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

template <class A1, class A2, class A3>
void QueueRenderCommand(A1 a1, A2 a2, A3 a3)
{
}

#define QUEUE_RENDER_COMMAND(tag, code)
#define QUEUE_RENDER_COMMAND_1(tag, a1, code)
#define QUEUE_RENDER_COMMAND_2(tag, a1, code)
#define QUEUE_RENDER_COMMAND_3(tag, a1, code)


AX_END_NAMESPACE

#endif // AX_COMMANDBUF_H
