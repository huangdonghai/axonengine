// set some global var for easy programming

_C <- getconsttable();

// don't modified this, it should indentical to cpp difinition
enum PropKind {
	kVoid, kBool, kInt, kFloat, kString, kObject, kVector3, kColor3, kPoint, kRect, kMatrix/*, kTable*/, kScriptValue,
	kEnum, kFlag, kTexture, kModel, kMaterial, kAnimation, kSpeedTree, kSound, kGroup
}

// declare namespace
Game <- {}
Physics <- {}
Ui <- {}
Gfx <- {}

::print("I'm start\n");

::dofile("object.nut");
::dofile("game/game_all.nut");
