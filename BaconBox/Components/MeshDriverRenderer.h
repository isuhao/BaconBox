#ifndef BB_MESH_DRIVER_RENDERER_H
#define BB_MESH_DRIVER_RENDERER_H

#include "BaconBox/Core/Component.h"
#include "BaconBox/Components/DefaultColorTransform.h"
#include "BaconBox/Display/RenderMode.h"
namespace BaconBox {
	class Mesh;
	class Texture;
	class ColorFilter;
	class Visibility;
	class GraphicDriver;
	class MeshDriverRenderer : public Component {
	public:
		BB_ID_HEADER;

		static int MESSAGE_GET_RENDER_MODE;
		static int MESSAGE_SET_RENDER_MODE;

		MeshDriverRenderer(int newRenderMode = (RenderMode::SHAPE | RenderMode::COLOR), int degenerationStride = 3, int degenerationJump = -1);

		MeshDriverRenderer(const MeshDriverRenderer &src);

		virtual ~MeshDriverRenderer();

		MeshDriverRenderer &operator=(const MeshDriverRenderer &src);

		virtual MeshDriverRenderer *clone() const;

		virtual void receiveMessage(int senderID, int destID, int message, void *data);

		virtual void update();

		int getRenderMode() const;

		void setRenderMode(int newRenderMode);
	private:
		void initializeConnections();
		
		GraphicDriver  *  const graphicDriver;

		Mesh *mesh;
		Texture *texture;
		DefaultColorTransform * colorTransform;
		Visibility *visibility;
		Color color;
		Color colorMultiplier;
		Color colorOffset;
		int degenerationStride;
		int degenerationJump;

		int renderMode;
	};
}

#endif
