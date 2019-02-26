#ifndef GamePressureBody_H
#define GamePressureBody_H

#include <Andromeda/Graphics/RenderManager.h>
using namespace Andromeda::Graphics;

#include "JellyPhysics/JellyPhysics.h"

namespace JellyPhysics
{
	class GamePressureBody : public PressureBody
	{
	private:

		VertexArrayObject* _vertexObject;
		VertexArrayObject* _shapeObject;
		glm::vec4 _color;
		glm::vec4 _lineColor;
		bool _created;
		std::vector<Vector2>	shape;

		Texture* _texture;
		std::vector<Vector2> _textPositions;

	public:

		GamePressureBody(World* w, const ClosedShape& s, float mpp, float gasPressure, float shapeK, float shapeD,
			float edgeK, float edgeD, const Vector2& pos, float angleInRadians, const Vector2& scale, bool kinematic);

		~GamePressureBody();

		void accumulateInternalForces();
		void accumulateExternalForces();

		void SetTexture(Texture* texture);
		void SetLineColor(glm::vec4 color);

		void Draw(glm::mat4 &proj, int *mIndices, int mIndicesCount, float  R, float  G, float B);
	};
}

#endif
