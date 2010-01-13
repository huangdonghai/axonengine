
// don't modify this if you modify c++ same time
enum VariantType {
	kEmpty, kBool, kInt, kFloat, kString, kObject, kTable, kVector3, kColor, kPoint, kRect, kAffineMat, kMaxType
}

enum MemberKind {
	kEmpty, kBool, kInt, kFloat, kString, kObject, kTable, kVector3, kColor, kPoint, kRect, kAffineMat, kEnum, kFlag, kTexture, kModel, kMaterial, kAnimation, kSpeedTree, kSound, kGroup
}

class Vector3 {
	x = 0;
	y = 0;
	z = 0;
}

class Rgb {
	r = 0; g = 0; b = 0;
}

class Point {
	x = 0; y = 0;
}

class Rect {
	x = 0; y = 0; width = 0; height = 0;
}

class AffineMat {
	m = [];
}

enum LightType {
	None, SunLight, PointLight, SpotLight
}

class Light {
	</ editable = true />
	active = true;
	</ editable = true />
	radius = 10.0;

	</ editable = true, gourp = "Style" />
	glare = 0;
	</ editable = true, gourp = "Style" />
	glareScale = 1;
	</ editable = true, gourp = "Style" enums = [ ["PointLight", LightType.PointLight], ["SpotLight", LightType.SpotLight] ]/>
	lightStyle = LightType.PointLight;
	</ editable = true, gourp = "Style" />
	castShadowMap = false;

	</ editable = true, gourp = "Style" enums = [ 128, 256, 512, 1024, 2048 ] />
	shadowMapSize = 256;
	</ editable = true, gourp = "Style" />
	spotAngle = 60.0;

	</ editable = true, gourp = "Color" />
	color = Rgb(1,1,1);
	</ editable = true, gourp = "Color" />
	intensity = 1.0;
	</ editable = true, gourp = "Color" />
	specularX = 1.0;
	</ editable = true, gourp = "Color" />
	hdrStops = 0;

	</ editable = true, gourp = "Option" />
	oneAreaOnly = true;
	</ editable = true, gourp = "Option", kind = MemberKind.kModel />
	model = "";
	</ editable = true, gourp = "Option", kind = MemberKind.kTexture />
	texture = "";
}