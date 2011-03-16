#ifndef AX_CORE_MODEL_H
#define AX_CORE_MODEL_H

AX_BEGIN_NAMESPACE

class Archiver
{
public:
	Archiver(File *f);

	Archiver &operator<<(bool val) { return operator<<(char(val)); }
	Archiver &operator<<(char val) { m_file->write(&val, 1); return *this; }
	Archiver &operator<<(byte_t val) { m_file->write(&val, 1); return *this;}
	Archiver &operator<<(short val) { val = LittleShort(val); m_file->write(&val, 2); return *this; }
	Archiver &operator<<(ushort_t val) { val = LittleShort(val); m_file->write(&val, 2); return *this; }
	Archiver &operator<<(int val) { val = LittleInt(val); m_file->write(&val, 4); return *this; }
	Archiver &operator<<(uint_t val) { val = LittleInt(val); m_file->write(&val, 4); return *this; }
	Archiver &operator<<(float val) { val = LittleFloat(val); m_file->write(&val, 4); return *this; }
	Archiver &operator<<(double val) { val = LittleEndian(val); m_file->write(&val, 8); return *this; }
	Archiver &operator<<(std::string val) { operator<<(static_cast<int>(val.size())); m_file->write(&val[0], val.size()); return *this; }
	template <typename Q>
	Archiver &operator<<(const std::vector<Q> &val)
	{
		int size = val.size();
		operator<<(size);
		for (std::vector<Q>::const_iterator it = val.begin(); it != val.end(); ++it) {
			operator<<(*it);
		}
		return *this;
	}

	Archiver &operator>>(bool &val) { char c; m_file->read(&c, 1); val = c; return *this; }
	Archiver &operator>>(char &val) { m_file->read(&val, 1); return *this; }
	Archiver &operator>>(byte_t &val) { m_file->read(&val, 1); return *this; }
	Archiver &operator>>(short &val) { m_file->read(&val, 2); val = LittleShort(val); return *this; }
	Archiver &operator>>(ushort_t &val) {m_file->read(&val, 2); val = LittleShort(val); return *this; }
	Archiver &operator>>(int &val) { m_file->read(&val, 4); val = LittleInt(val); return *this; }
	Archiver &operator>>(uint_t &val) { m_file->read(&val, 4); val = LittleInt(val); return *this; }
	Archiver &operator>>(float &val) { m_file->read(&val, 4); val = LittleFloat(val); return *this; }
	Archiver &operator>>(double &val) { m_file->read(&val, 8); val = LittleEndian(val); return *this; }
	Archiver &operator>>(std::string &val) { int size; operator>>(size); val.resize(size); m_file->read(&val[0], size); return *this; }
	template <typename Q>
	Archiver &operator>>(std::vector<Q> &val)
	{
		int size = 0;
		operator>>size;
		val.resize(size);
		for (int i = 0; i < size; i++) {
			operator>>val[i];
		}
		return *this;
	}

private:
	File *m_file;
};

class VertexDeclaration
{
public:
	enum DataType
	{
		DT_NONE = 0,
		DT_BYTE4,
		DT_FLOAT,
		DT_FLOAT2,
		DT_FLOAT3,
		DT_FLOAT4
	};

	enum DataUsage
	{
		DU_NONE = 0,
		DU_POSITION = 1,
		DU_COLOR = 2,
		DU_NORMAL = 4,
		DU_TANGENT = 8,
		DU_BINORMAL = 16,
		DU_TEXCOORD = 32,
		DU_BLENDWEIGHTS = 64,  // usually 4 weights, but 3 can be stored with 1 implied. Can be stored as 4*uint8, so quantized where 1.0f => 0xff (255),
		DU_BLENDINDICES = 128, // usually 4 hkUint8s in a row. So can reference 256 blend transforms (bones)
		DU_USERDATA = 256
	};

	struct Element {
		int offset;
		DataType dataType;
		DataUsage usage;
		int usageIndex;
	};

	int m_stride;
	std::vector<Element> m_elements;
};

struct QuadStripData {
	float width;
	float height;
	ushort_t *indices;
};

struct MeshData {
	std::string name;
	std::string material;
	VertexDeclaration declaration;
	int numVertices;
	int numIndices;
	std::vector<float> floatData;
	std::vector<byte_t> byteData;
	std::vector<ushort_t> indices;
	std::vector<ushort_t> quadstrips;
	std::vector<ushort_t> orphanTriangles;
};

struct JointData {
	std::string name;
	int parent;
	Vector3 position;
	CompactQuat rotate;
};

struct SkeletonData {
	std::vector<JointData> joints;
};

struct SkinData {};

struct AnimationAnnotation {
	float time;
	std::string text;
};

struct AnimationData {
	SkeletonData *skeleton;
	int numFrames;
};

struct ModelData {
	enum {
		FileFourCC = AX_MAKEFOURCC('A', 'X', 'M', 'L'),
		Version = 1
	};
	std::vector<MeshData> meshData;

	void save(Archiver &ar)
	{
		ar << FileFourCC;
		ar << Version;
		ar << meshData;
	}

	void load(Archiver &ar);
};

AX_END_NAMESPACE

#endif // AX_CORE_MODEL_H
