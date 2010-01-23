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

	</ editable = true, group = "Style" />
	glare = 0;
	</ editable = true, group = "Style" />
	glareScale = 1;
	</ editable = true, group = "Style" enums = [ ["PointLight", LightType.PointLight], ["SpotLight", LightType.SpotLight] ]/>
	lightStyle = LightType.PointLight;
	</ editable = true, group = "Style" />
	castShadowMap = false;

	</ editable = true, group = "Style" enums = [128, 256, 512, 1024, 2048] />
	shadowMapSize = 256;
	</ editable = true, group = "Style" />
	spotAngle = 60.0;

	</ editable = true, group = "Color" />
	color = Color3(1,1,1);
	</ editable = true, group = "Color" />
	intensity = 1.0;
	</ editable = true, group = "Color" />
	specularX = 1.0;
	</ editable = true, group = "Color" />
	hdrStops = 0;

	</ editable = true, group = "Option" />
	oneAreaOnly = true;
	</ editable = true, group = "Option", kind = PropKind.kModel />
	model = "";
	</ editable = true, group = "Option", kind = PropKind.kTexture />
	texture = "";
}
AX_REGISTER_CLASS("Game.Effect.Light");