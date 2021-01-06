#include "fmega.h"
#include "fmegaAssets.h"
#include "fmegaObjectFactory.h"

namespace fmega {

	void FMegaAssets::LoadAll() {
		/*PlatformAlbedo = new Texture2D("assets/textures/floor_tiles_06_diff_1k_metal.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformNormalmap = new Texture2D("assets/textures/floor_tiles_06_nor_1k2.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformRoughness = new Texture2D("assets/textures/floor_tiles_06_metal2.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformMetalness = new Texture2D("assets/textures/floor_tiles_06_metal2.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);*/

		PlatformAlbedo    = new Texture2D("assets/textures/MetalPlates006_1K_Color2.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformNormalmap = new Texture2D("assets/textures/MetalPlates006_1K_Normal.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformRoughness = new Texture2D("assets/textures/MetalPlates006_1K_Roughness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformMetalness = new Texture2D("assets/textures/MetalPlates006_1K_Metalness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);

		/*TunnelAlbedo = new Texture2D("assets/textures/Concrete_wall_02_1K_Base_Color.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		TunnelNormalmap = new Texture2D("assets/textures/Concrete_wall_02_1K_Normal.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		TunnelRoughness = new Texture2D("assets/textures/Concrete_wall_02_1K_Roughness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);*/

		TunnelAlbedo    = new Texture2D("assets/textures/MetalPlates008_1K_Color.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		TunnelNormalmap = new Texture2D("assets/textures/MetalPlates008_1K_Normal.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		TunnelRoughness = new Texture2D("assets/textures/MetalPlates008_1K_Roughness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		TunnelMetalness = new Texture2D("assets/textures/MetalPlates008_1K_Metalness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);

		PlayerAlbedo    = new Texture2D("assets/textures/MetalPlates001_1K_Color.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlayerNormalmap = new Texture2D("assets/textures/MetalPlates001_1K_Normal.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlayerRoughness = new Texture2D("assets/textures/MetalPlates001_1K_Roughness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlayerMetalness = new Texture2D("assets/textures/MetalPlates001_1K_Metalness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);

		DiscoAlbedo    = new Texture2D("assets/textures/MetalPlates007_1K_Color2.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		DiscoNormalmap = new Texture2D("assets/textures/MetalPlates007_1K_Normal.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		DiscoRoughness = new Texture2D("assets/textures/MetalPlates007_1K_Roughness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		DiscoMetalness = new Texture2D("assets/textures/MetalPlates007_1K_Metalness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);

		LampMesh = FMegaObjectFactory::GenLamp(4);
		TunnelMesh = FMegaObjectFactory::GenTunnel(1);
		BoxMesh = FMegaObjectFactory::GenBox(1024);
		SegmentMesh = FMegaObjectFactory::GenSegment(64);
		SphereMesh = FMegaObjectFactory::GenSphere(1);
		PickupMesh = FMegaObjectFactory::GenPickup(128);

		m_RewindLoadThread = std::thread([this]() {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);
			for (int i = 0; i < NumRewindImages && m_RewindLoadRunning; i++) {
				RewindImages[i] = ImageIO::LoadImage("assets/textures/rewind/rewind" + std::string(i < 10 ? "0" : "") + StringUtils::ToString(i) + ".png");
				m_NumRewindImagesLoaded++;
			}
		});
	}

	void FMegaAssets::DestroyAll() {
		delete TunnelAlbedo;
		delete TunnelRoughness;
		delete TunnelNormalmap;
		delete TunnelMetalness;

		delete PlatformAlbedo;
		delete PlatformNormalmap;
		delete PlatformRoughness;
		delete PlatformMetalness;

		delete PlayerAlbedo;
		delete PlayerNormalmap;
		delete PlayerRoughness;
		delete PlayerMetalness;

		delete DiscoAlbedo;
		delete DiscoNormalmap;
		delete DiscoRoughness;
		delete DiscoMetalness;

		delete LampMesh;
		delete TunnelMesh;
		delete PickupMesh;
		delete SphereMesh;
		delete BoxMesh;
		delete SegmentMesh;

		if (m_RewindLoadRunning) {
			m_RewindLoadRunning = false;
			m_RewindLoadThread.join();
		}

		for (int i = 0; i < NumRewindTexturesLoaded; i++) {
			delete RewindTextures[i];
		}
		int rewindImages = m_NumRewindImagesLoaded;
		for (int i = NumRewindTexturesLoaded; i < rewindImages; i++) {
			ImageIO::FreeImage(RewindImages[i]);
		}
	}

	void FMegaAssets::UpdateLazyAssets() {
		int numImages = m_NumRewindImagesLoaded;
		int numTextures = NumRewindTexturesLoaded;
		if (numImages != numTextures) {
			while (numTextures < numImages) {
				RewindTextures[numTextures] = new Texture2D(RewindImages[numTextures]);
				ImageIO::FreeImage(RewindImages[numTextures]);
				numTextures++;
			}
			NumRewindTexturesLoaded = numImages;
			if (numImages == NumRewindImages) {
				m_RewindLoadRunning = false;
				m_RewindLoadThread.join();
			}
		}
 	}

}