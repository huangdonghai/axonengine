enum LightType {
	None, SunLight, PointLight, SpotLight
}

class Game.Effect.Light </ cppClass = "GameLight" />
{
	// properties
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

	</ editable = true, gourp = "Style" enums = [128, 256, 512, 1024, 2048] />
	shadowMapSize = 256;
	</ editable = true, gourp = "Style" />
	spotAngle = 60.0;

	</ editable = true, gourp = "Color" />
	color = Color3(1,1,1);
	</ editable = true, gourp = "Color" />
	intensity = 1.0;
	</ editable = true, gourp = "Color" />
	specularX = 1.0;
	</ editable = true, gourp = "Color" />
	hdrStops = 0;

	</ editable = true, gourp = "Option" />
	oneAreaOnly = true;
	</ editable = true, gourp = "Option", kind = PropKind.kModel />
	model = "";
	</ editable = true, gourp = "Option", kind = PropKind.kTexture />
	texture = "";
}
AX_REGISTER_CLASS("Game.Effect.Light");