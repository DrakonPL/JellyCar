#include "JellyHelper.h"

JellyHellper* JellyHellper::_helper = 0;

JellyHellper::JellyHellper()
{
	_colorShader = 0;
	_textureShader = 0;

	_arrayObject = 0;
}

void JellyHellper::LoadShaders()
{
	//load color shader
	_colorShader = ShaderManager::Instance()->LoadFromFile("simple_color", "Assets/Shaders/simple_color_transform", "Assets/Shaders/simple_color_transform", Simple);

	//load texture shader
	_textureShader = ShaderManager::Instance()->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);
}

JellyHellper* JellyHellper::Instance()
{
	if (_helper == 0)
	{
		_helper = new JellyHellper();
	}

	return _helper;
}

void JellyHellper::UpdateLines(VertexArrayObject* vertexArray, std::vector<PointMass> &pointMasses, bool create)
{
	unsigned int vertsCount = pointMasses.size();
	unsigned int indicesCount = pointMasses.size() * 2;

	SimpleVertex* _simpleData = 0;
	unsigned short* _indices = 0;

	if (create)
	{
		vertexArray->CreateVertices(vertsCount);
		vertexArray->CreateIndices(indicesCount);

		_simpleData = static_cast<SimpleVertex*>(vertexArray->GetVertices());
		_indices = static_cast<unsigned short*>(vertexArray->GetIndices());
	}
	else
	{
		//_simpleData = new SimpleVertex[vertsCount];
        _simpleData = static_cast<SimpleVertex*>(vertexArray->GetVertices());
	}

	//update verts
	for (unsigned int i = 0; i < vertsCount; i++)
	{
		_simpleData[i].x = pointMasses[i].Position.X;
		_simpleData[i].y = pointMasses[i].Position.Y;
		_simpleData[i].z = 0.0f;
	}

	//create indices
	if (create)
	{
		int counter = 0;

		for (unsigned int i = 0; i < pointMasses.size(); i++)
		{
			if (i == 0)
			{
				_indices[counter] = i;
				counter++;
			}
			else
			{
				_indices[counter] = i;
				counter++;
				_indices[counter] = i;
				counter++;
			}
		}

		_indices[counter] = 0;
	}


	//update or create
	if (create)
	{
		vertexArray->Generate(false);
	}
	else
	{
		vertexArray->UpdateVertices(_simpleData, vertsCount, false);
	}
}

void JellyHellper::DrawLines(VertexArrayObject* vertexArray, glm::mat4 &proj, glm::vec4 &color)
{
	if (_colorShader != 0 && vertexArray != 0)
	{
		_colorShader->Bind();

		_colorShader->SetUniform(VertexShader, "translation", proj);
		_colorShader->SetUniform(FragmentShader, "colors", color);

		vertexArray->Draw();
	}
}

void JellyHellper::UpdateSpringShape(VertexArrayObject* vertexArray, std::vector<PointMass> &pointMasses, int *mIndices, int mIndicesCount, bool create)
{
	unsigned int vertsCount = pointMasses.size();
	int indicesCount = mIndicesCount;

	SimpleVertex* _simpleData = 0;
	unsigned short* _indices = 0;

	if (create)
	{
		vertexArray->CreateVertices(vertsCount);
		vertexArray->CreateIndices(indicesCount);

		_simpleData = static_cast<SimpleVertex*>(vertexArray->GetVertices());
		_indices = static_cast<unsigned short*>(vertexArray->GetIndices());
	}
	else
	{
		//_simpleData = new SimpleVertex[vertsCount];
        _simpleData = static_cast<SimpleVertex*>(vertexArray->GetVertices());
	}

	//update verts
	for (unsigned int i = 0; i < vertsCount; i++)
	{
		_simpleData[i].x = pointMasses[i].Position.X;
		_simpleData[i].y = pointMasses[i].Position.Y;
		_simpleData[i].z = 0.0f;
	}

	//create indices
	if (create)
	{
		for (int i = 0; i < indicesCount; i++)
		{
			_indices[i] = mIndices[i];
		}
	}

	//update or create
	if (create)
	{
		vertexArray->Generate(false);
	}
	else
	{
		vertexArray->UpdateVertices(_simpleData, vertsCount, false);
	}
}

void JellyHellper::UpdateBlobShape(VertexArrayObject* vertexArray, std::vector<Vector2> &points, int count, bool create)
{
	unsigned int vertsCount = points.size();
	unsigned int indicesCount = points.size();

	SimpleVertex* _simpleData = 0;
	unsigned short* _indices = 0;

	if (create)
	{
		vertexArray->CreateVertices(vertsCount);
		vertexArray->CreateIndices(indicesCount);

		_simpleData = static_cast<SimpleVertex*>(vertexArray->GetVertices());
		_indices = static_cast<unsigned short*>(vertexArray->GetIndices());
	}
	else
	{
		//_simpleData = new SimpleVertex[vertsCount];
        _simpleData = static_cast<SimpleVertex*>(vertexArray->GetVertices());
	}

	//update verts
	for (unsigned int i = 0; i < vertsCount; i++)
	{
		_simpleData[i].x = points[i].X;
		_simpleData[i].y = points[i].Y;
		_simpleData[i].z = 0.0f;
	}

	//create indices
	if (create)
	{
		for (unsigned int i = 0; i < indicesCount; i++)
		{
			_indices[i] = i;
		}
	}

	//update or create
	if (create)
	{
		vertexArray->Generate(false);
	}
	else
	{
		vertexArray->UpdateVertices(_simpleData, vertsCount, false);
	}
}

void JellyHellper::DrawShape(VertexArrayObject* vertexArray, glm::mat4 &proj, glm::vec4 &color)
{
	if (_colorShader != 0 && vertexArray != 0)
	{
		_colorShader->Bind();

		_colorShader->SetUniform(VertexShader, "translation", proj);
		_colorShader->SetUniform(FragmentShader, "colors", color);

		vertexArray->Draw();
	}
}

std::vector<Vector2> JellyHellper::GetTexturePositions(const AABB& aabb, const Body::PointMassList& vector)
{
	std::vector<Vector2> positions;

	float width = aabb.Max.X - aabb.Min.X;// fabsf() + fabsf();
	float height = aabb.Max.Y - aabb.Min.Y;

	for (size_t i = 0; i < vector.size(); ++i)
	{
		float newPosX = fabsf(vector[i].Position.X - aabb.Min.X);
		float newPosY = fabsf(vector[i].Position.Y - aabb.Min.Y);

		float pointX = newPosX / width;
		float pointY = 1.0f - (newPosY / height);

		positions.push_back(Vector2(pointX, pointY));
	}

	return positions;
}

void JellyHellper::UpdateTextured(VertexArrayObject* vertexArray, std::vector<PointMass> &pointMasses, std::vector<Vector2> &mTextureList, int *mIndices, int mIndicesCount, bool create)
{
	unsigned int vertsCount = pointMasses.size();
	int indicesCount = mIndicesCount;

	TextureVertex* _simpleData = 0;
	unsigned short* _indices = 0;

	if (create)
	{
		vertexArray->CreateVertices(vertsCount);
		vertexArray->CreateIndices(indicesCount);

		_simpleData = static_cast<TextureVertex*>(vertexArray->GetVertices());
		_indices = static_cast<unsigned short*>(vertexArray->GetIndices());
	}
	else
	{
		//_simpleData = new TextureVertex[vertsCount];
        _simpleData = static_cast<TextureVertex*>(vertexArray->GetVertices());
	}

	//update verts
	for (unsigned int i = 0; i < vertsCount; i++)
	{
		_simpleData[i].x = pointMasses[i].Position.X;
		_simpleData[i].y = pointMasses[i].Position.Y;
		_simpleData[i].z = 0.0f;

		_simpleData[i].u = mTextureList[i].X;
		_simpleData[i].v = mTextureList[i].Y;
	}

	//create indices
	if (create)
	{
		for (int i = 0; i < indicesCount; i++)
		{
			_indices[i] = mIndices[i];
		}
	}


	//update or create
	if (create)
	{
		vertexArray->Generate(false);
	}
	else
	{
		vertexArray->UpdateVertices(_simpleData, vertsCount, false);
	}
}

void JellyHellper::UpdateTexturedBlob(VertexArrayObject* vertexArray, std::vector<Vector2> &points, int count, std::vector<Vector2> &mTetxure, bool create)
{
	unsigned int vertsCount = points.size();
	unsigned int indicesCount = points.size();

	TextureVertex* _simpleData = 0;
	unsigned short* _indices = 0;

	if (create)
	{
		vertexArray->CreateVertices(vertsCount);
		vertexArray->CreateIndices(indicesCount);

		_simpleData = static_cast<TextureVertex*>(vertexArray->GetVertices());
		_indices = static_cast<unsigned short*>(vertexArray->GetIndices());
	}
	else
	{
		//_simpleData = new TextureVertex[vertsCount];
		_simpleData = static_cast<TextureVertex*>(vertexArray->GetVertices());
	}

	//update verts
	for (unsigned int i = 0; i < vertsCount; i++)
	{
		_simpleData[i].x = points[i].X;
		_simpleData[i].y = points[i].Y;
		_simpleData[i].z = 0.0f;

		_simpleData[i].u = mTetxure[i].X;
		_simpleData[i].v = mTetxure[i].Y;
	}

	//create indices
	if (create)
	{
		for (unsigned int i = 0; i < indicesCount; i++)
		{
			_indices[i] = i;
		}
	}

	//update or create
	if (create)
	{
		vertexArray->Generate(false);
	}
	else
	{
		vertexArray->UpdateVertices(_simpleData, vertsCount, false);
	}
}

void JellyHellper::DrawTextured(VertexArrayObject* vertexArray, glm::mat4 &proj, Texture* texture, glm::vec4 &color)
{
	if (_textureShader != 0 && vertexArray != 0)
	{
		//bind texture
		RenderManager::Instance()->UseTexture(texture);

		//bind shader
		_textureShader->Bind();

		//set uniforms
		_textureShader->SetUniform(VertexShader, "translation", proj);
		_textureShader->SetUniform(FragmentShader, "colors", color);

		vertexArray->Draw();
	}
}

