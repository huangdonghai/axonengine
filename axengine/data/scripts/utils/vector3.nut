
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

