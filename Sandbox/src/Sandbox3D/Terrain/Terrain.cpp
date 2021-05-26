#include "Terrain.h"
#include "../stb_image/stb_image.h"

Terrain::Terrain(const std::string& heightMap, unsigned int width, unsigned int depth, float step)
	: m_width(width), m_depth(depth), m_step(step)
{
	m_vertexArray = Generate(heightMap, width, depth, step);
}

void Terrain::SetPosition(const Jass::JVec3& position)
{
	m_position = position;

	m_transformation = Jass::Translate(Jass::JMat4(1.0f), m_position);
}

float Terrain::GetTerrainHeight(float worldX, float worldZ) const
{
	// Transform the coordinates to match the terrain transformation
	auto transformedPosition = Jass::Inverse(m_transformation) * Jass::JVec4({ worldX, 0.0f, worldZ, 1.0f });
	
	worldX = transformedPosition.x;
	worldZ = transformedPosition.z;

	// The terrain extends towards -z, but for our indices we need positive values
	worldZ = -worldZ;

	// Offsets of the coords into a terrain quad
	float offX = fmodf(worldX, m_step);
	float offZ = fmodf(worldZ, m_step);

	// Plane equation for the triangle we are now
	Jass::JVec3 p1, p2, p3;
	float A, B, C, D;
	if (offX + offZ <= m_step) {
		// First triangle in the quad

		p1.x = trunc(worldX / m_step);
		p1.z = trunc(worldZ / m_step);
		p1.y = GetHeight((int)p1.x, (int)p1.z);

		p2.x = trunc(worldX / m_step) + 1;
		p2.z = trunc(worldZ / m_step);
		p2.y = GetHeight((int)p2.x, (int)p2.z);

		p3.x = trunc(worldX / m_step);
		p3.z = trunc(worldZ / m_step) + 1;
		p3.y = GetHeight((int)p3.x, (int)p3.z);
	}
	else {
		// Second triangle in the quad

		p1.x = trunc(worldX / m_step) + 1;
		p1.z = trunc(worldZ / m_step);
		p1.y = GetHeight((int)p1.x, (int)p1.z);

		p2.x = trunc(worldX / m_step);
		p2.z = trunc(worldZ / m_step) + 1;
		p2.y = GetHeight((int)p2.x, (int)p2.z);

		p3.x = trunc(worldX / m_step) + 1;
		p3.z = trunc(worldZ / m_step) + 1;
		p3.y = GetHeight((int)p3.x, (int)p3.z);
	}

	// We used terrain coords for x and z to get the height, but for the plane equation we need world coords
	p1.x *= m_step;
	p1.z *= m_step;

	p2.x *= m_step;
	p2.z *= m_step;

	p3.x *= m_step;
	p3.z *= m_step;

	// Plane equation
	A = (p2.y - p1.y) * (p3.z - p1.z) - (p3.y - p1.y) * (p2.z - p1.z);
	B = (p2.z - p1.z) * (p3.x - p1.x) - (p3.z - p1.z) * (p2.x - p1.x);
	C = (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);
	D = -(A * p1.x + B * p1.y + C * p1.z);

	// Use the equation to get de Y given a X and Z
	return (-D - C * worldZ - A * worldX) / B;

}

void Terrain::SetBlendMap(const std::string& blendmap)
{
	TerrainTexture tt;
	tt.Texture = Jass::Texture2D::Create(blendmap);
	tt.Slot = 0;
	tt.UniformName = "u_blendMap";

	m_terrainTextures.push_back(tt);
}

void Terrain::AddTexture(const std::string& texture, const std::string& uniformName, unsigned int slot)
{
	TerrainTexture tt;
	tt.Texture = Jass::Texture2D::Create(texture);
	tt.UniformName = uniformName;
	tt.Slot = slot;
	m_terrainTextures.push_back(tt);
}

void Terrain::Render(Jass::Ref<Jass::Shader>& shader, const Light& light, const Jass::JVec4& clipPlane)
{
	shader->Bind();
	
	shader->SetFloat4("u_lightColor", light.GetColor());
	shader->SetFloat3("u_lightPosition", light.GetPosition());

	shader->SetFloat("u_ambientReduction", m_ambientReduction);
	shader->SetFloat("u_diffuseReduction", m_diffuseReduction);

	shader->SetFloat("u_repeatFactor", m_uvRepeat);
	
	//shader->SetFloat4("u_clipPlane", clipPlane);

	for (const auto& texture : m_terrainTextures)
	{
		if (texture.Texture)
		{
			texture.Texture->Bind(texture.Slot);
			//shader->SetInt(texture.UniformName, texture.Slot);
		}
	}

	Jass::Renderer::Submit(shader, m_vertexArray, Jass::RenderMode::TriangleStrip, m_transformation);
}

Jass::Ref<Jass::VertexArray> Terrain::Generate(const std::string& heightmap, unsigned int width, unsigned int depth, float step)
{	
	std::vector<MeshVertex> vertices;
	std::vector<unsigned int> indices;
	
	int imgWidth, imgHeight, channels;
	stbi_uc* data = nullptr;
	{
		stbi_set_flip_vertically_on_load(1);
		data = stbi_load(heightmap.c_str(), &imgWidth, &imgHeight, &channels, 0);
	}
	if (!data) {
		JASS_LOG_WARN("The image {0} could not be loaded", heightmap);
	}

	ImageInfo imgInfo;
	imgInfo.Image = data;
	imgInfo.Channels = channels;
	imgInfo.Width = imgWidth;
	imgInfo.Height = imgHeight;

	int vertexCountW = width;
	int vertexCountD = depth;

	// 2D Vector
	m_terrainHeight = std::vector<std::vector<float>>(vertexCountW, std::vector<float>(vertexCountD));

	unsigned int totalCount = vertexCountD * vertexCountW;
	vertices.reserve(totalCount);
	unsigned int totalIndices = (vertexCountW - 1) * (vertexCountD * 2) + (vertexCountW - 2) + (vertexCountD - 2);
	indices.reserve(totalIndices);

	// Generate vertices
	for (int vx = 0; vx < vertexCountW; vx++) {
		for (int vz = 0; vz < vertexCountD; vz++) {

			float height = GetHeight(imgInfo, vx, vz);
			Jass::JVec3 position = { vx * step, height, -vz * step }; // vz is negative because the terrain expands towards -Z
			m_terrainHeight[vx][vz] = height;
			Jass::JVec3 normal = CalculateNormal(vx, vz, imgInfo);
			float u = (float)vx / (m_width - 1);
			float v = (float)vz / (m_depth - 1);
			Jass::JVec2 texCoord = { u, v };

			MeshVertex mv;
			mv.Position = position;
			mv.Normal = normal;
			mv.TexCoord = texCoord;
			vertices.push_back(mv);
		}
	}

	// Generate indices
	CalculateIndices(indices);

	JASS_ASSERT(indices.size() > 0, "The index buffer is empty");

	if (data)
		stbi_image_free(data);

	return PrepareVAO(vertices, indices);
}

void Terrain::CalculateIndices(std::vector<unsigned int>& indices)
{
	int minCol = 0;
	int maxCol = std::max(m_width - 2, 0);

	int minRow = 0;
	int maxRow = std::max(m_depth - 1, 0);

	// The terrain is not visible
	if (minCol == maxCol || minRow == maxRow)
		return;

	for (int c = minCol; c <= maxCol; c++) {
		for (int r = minRow; r <= maxRow; r++) {

			// If it is not the first strip, generate a link with the previous strip (degenerate triangle)
			if (c > minCol && r == minRow)
				indices.push_back(c * m_depth + r);

			indices.push_back(c * m_depth + r);
			indices.push_back((c + 1) * m_depth + r);

			// Link with the next triangle strip (degenerate triangle)
			if (r == maxRow && c < maxCol)
				indices.push_back((c + 1) * m_depth + r);
		}
	}
}

Jass::Ref<Jass::VertexArray> Terrain::PrepareVAO(std::vector<MeshVertex>& vertices, std::vector<unsigned int>& indices)
{
	CalculateTangentsBitangents(vertices);

	auto vertexArray = Jass::VertexArray::Create();

	Jass::VertexBufferConfig vbConfig;
	vbConfig.Data = &vertices[0];
	vbConfig.Size = (unsigned int)vertices.size() * sizeof(MeshVertex);
	vbConfig.DataUsage = Jass::DataUsage::StaticDraw;
	Jass::Ref<Jass::VertexBuffer> vbo = Jass::VertexBuffer::Create(vbConfig);

	vbo->SetLayout({
		Jass::BufferElement(Jass::ShaderDataType::Float3,"a_position"),
		Jass::BufferElement(Jass::ShaderDataType::Float3,"a_normal"),
		Jass::BufferElement(Jass::ShaderDataType::Float2,"a_texCoords"),
		Jass::BufferElement(Jass::ShaderDataType::Float3,"a_tangent"),
		Jass::BufferElement(Jass::ShaderDataType::Float3,"a_bitangent")
		});

	vertexArray->AddVertexBuffer(vbo);

	Jass::Ref<Jass::IndexBuffer> ibo = Jass::IndexBuffer::Create({ &indices[0], (unsigned int)indices.size(), Jass::DataUsage::StaticDraw });

	vertexArray->SetIndexBuffer(ibo);

	return vertexArray;
}

float Terrain::GetHeight(ImageInfo& imgInfo, int x, int z)
{
	const float MAX_PIXEL_COLOR = 255 * 255 * 255;

	if (!imgInfo.Image)
		return 0.0f;

	float scaleX = ((float)imgInfo.Width) / (m_width - 1);
	float scaleZ = ((float)imgInfo.Height) / (m_depth - 1);

	int imageX = (int)(x * scaleX);
	int imageY = (int)(z * scaleZ);

	unsigned int bytePerPixel = imgInfo.Channels;
	unsigned char* pixelOffset = imgInfo.Image + (imageX + imgInfo.Height * imageY) * bytePerPixel;

	float height = float(pixelOffset[0] * pixelOffset[1] * pixelOffset[2]);
	height += MAX_PIXEL_COLOR / 2;
	height /= MAX_PIXEL_COLOR / 2;

	return height * m_maxHeight;
}

float Terrain::GetHeight(int x, int z) const
{
	if (!m_terrainHeight.size())
		return 0.0f;
	
	if (x < 0 || x >= m_width || z < 0 || z >= m_depth)
		return 0.0f;

	return m_terrainHeight[x][z];
}

Jass::JVec3 Terrain::CalculateNormal(unsigned int x, unsigned int z, ImageInfo& imgInfo)
{
	x = x == 0 ? 1 : x;
	z = z == 0 ? 1 : z;

	float heightL = GetHeight(imgInfo, x - 1, z);
	float heightR = GetHeight(imgInfo, x + 1, z);
	// Terrain expands towards -Z
	float heightD = GetHeight(imgInfo, x, z + 1);
	float heightU = GetHeight(imgInfo, x, z - 1);

	Jass::JVec3 normal = { heightL - heightR, 2.0f, heightD - heightU };

	return Jass::Normalize(normal);
}

void Terrain::CalculateTangentsBitangents(std::vector<MeshVertex>& vertices)
{
	for (size_t vx = 0; vx < (size_t)m_width - 1; vx++) {
		for (size_t vz = 0; vz < (size_t)m_depth - 1; vz++) {
			size_t index0 = vx * m_depth + vz;
			size_t index1 = (vx + 1) * m_depth + vz;
			size_t index2 = vx * m_depth + (vz + 1);

			auto& v0 = vertices[index0].Position;
			auto& v1 = vertices[index1].Position;
			auto& v2 = vertices[index2].Position;

			auto& uv0 = vertices[index0].TexCoord;
			auto& uv1 = vertices[index1].TexCoord;
			auto& uv2 = vertices[index2].TexCoord;

			auto deltaPos1 = v1 - v0;
			auto deltaPos2 = v2 - v0;

			auto deltaUV1 = uv1 - uv0;
			auto deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

			auto tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			auto bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			vertices[index0].Tangent = tangent;
			vertices[index1].Tangent = tangent;
			vertices[index2].Tangent = tangent;

			vertices[index0].Bitangent = bitangent;
			vertices[index1].Bitangent = bitangent;
			vertices[index2].Bitangent = bitangent;
		}
	}
}