#include "pch.h"
#include <Kiss/render/commands.h>
#include "assets/data.h"

using namespace kiss;

namespace gfxCmdBuffer 
{
	constexpr int buffersize = 512;
	u8 bufferData[buffersize];
	gfx2d::commandBuffer<u32> commandbuffer(bufferData, buffersize);

	void setup(float x, float y) {
		commandbuffer.reset();
		using namespace gfx2d;
		using namespace id;
		constexpr sprId spr = spr::RectRect;
		constexpr sprId s9 = s9::Test;
		constexpr u8 font = fnt::Text;
		u32 col[4] = { 0xFF505050, 0x80FFFFFF, 0xFF505050, 0x80FFFFFF };
		commandbuffer
			.vertexdata(col)
			.sprite(spr, 16, 32)
			.sprite(spr, 16, y)
			.sprite(spr, x - 10, y)
			.sprite(spr, x - 10, 32)
			.scale9(s9, 250, 250, 294, 300)
			.scale9(s9, 306, 250, 500, 300)
			.textblock(100, 100).text("Ciao ")
			.vertexdata(iColor::Red).font(font).text("Mondo!!!");
	}

	void render() {
		commandbuffer.execute();
	}
}