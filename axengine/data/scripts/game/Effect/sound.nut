class Game.Effect.Sound </ cppClass = "GameSound" />
{
	// properties
	</ editable = true />
	active = true;
	
	</ editable = true group = "Style" kind = PropKind.kSound />
	sound = "";
	</ editable = true group = "Style" />
	minDist = 0;
	</ editable = true group = "Style" />
	maxDist = 64;
	</ editable = true group = "Style" />
	looping = false;
	</ editable = true group = "Style" />
	interval = 0;
	// end properties
}
AX_REGISTER_CLASS("Game.Effect.Sound");