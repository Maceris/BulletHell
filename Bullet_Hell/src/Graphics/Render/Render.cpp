#include "../../Utilities/Globals.h"
#include "Render.h"

Render::Render(const Window& window)
	: gBuffer(GBuffer(window.width, window.height))
{

}
