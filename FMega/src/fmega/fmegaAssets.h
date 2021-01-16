#pragma once

#include "graphics/texture2d.h"
#include "graphics/mesh.h"
#include <thread>
#include <mutex>

namespace fmega {

	const int NumRewindImages = 49;

	class FMegaAssets {
	public:
		FMegaAssets() {}
		~FMegaAssets() {}

		void UpdateLazyAssets();
		void LoadAll();
		void DestroyAll();

		Texture2D* TunnelAlbedo = nullptr;
		Texture2D* TunnelNormalmap = nullptr;
		Texture2D* TunnelRoughness = nullptr;
		Texture2D* TunnelMetalness = nullptr;

		Texture2D* PlatformAlbedo = nullptr;
		Texture2D* PlatformNormalmap = nullptr;
		Texture2D* PlatformRoughness = nullptr;
		Texture2D* PlatformMetalness = nullptr;

		Texture2D* PlayerAlbedo = nullptr;
		Texture2D* PlayerNormalmap = nullptr;
		Texture2D* PlayerRoughness = nullptr;
		Texture2D* PlayerMetalness = nullptr;

		Texture2D* BarsAlbedo = nullptr;
		Texture2D* BarsNormalmap = nullptr;
		Texture2D* BarsRoughness = nullptr;
		Texture2D* BarsMetalness = nullptr;

		Texture2D* DiscoAlbedo = nullptr;
		Texture2D* DiscoNormalmap = nullptr;
		Texture2D* DiscoRoughness = nullptr;
		Texture2D* DiscoMetalness = nullptr;

		Texture2D* BinocularsTexture = nullptr;
		Texture2D* QuestionmarkTexture = nullptr;

		Image RewindImages[NumRewindImages];
		Texture2D* RewindTextures[NumRewindImages];
		int NumRewindTexturesLoaded = 0;

		Mesh* TunnelMesh = nullptr;
		Mesh* LampMesh = nullptr;
		Mesh* BoxMesh = nullptr;
		Mesh* SegmentMesh = nullptr;
		Mesh* SphereMesh = nullptr;
		Mesh* PickupMesh = nullptr;
		Mesh* HeartMesh = nullptr;
		Mesh* BarsMesh = nullptr;
	private:
		std::atomic_int m_NumRewindImagesLoaded = 0;
		std::atomic_bool m_RewindLoadRunning = true;
		std::thread m_RewindLoadThread;
	};

}