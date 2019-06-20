#pragma once
#pragma pack(1)
#include "Common/BasicTagTypes.h"

struct global_geometry_section_struct_block
{
	tag_block<> parts;

	tag_block<> subparts;

	tag_block<> visibilityBounds;

	tag_block<> rawVertices;

	tag_block<> stripIndices;

	/****************************************
	* definition_name: global_section_mopp_code_data
	* flags: 0
	* alignment_bit: 16
	* byteswap_proc: 0x00531b20
	****************************************/
	// DataSize(393216)
	byte_ref visibilityMoppCode;
	tag_block<> moppReorderTable;

	tag_block<> vertexBuffers;

	BYTE padding33[4];
};
CHECK_STRUCT_SIZE(global_geometry_section_struct_block, 108);

struct global_geometry_block_resource_block
{

	enum Type : BYTE
	{
		TagBlock = 0,
		TagData = 1,
		VertexBuffer = 2,
	};
	Type type;
	BYTE padding52[3];
	short primaryLocator;
	short secondaryLocator;
	int resourceDataSize;
	int resourceDataOffset;
};
CHECK_STRUCT_SIZE(global_geometry_block_resource_block, 16);

struct global_geometry_block_info_struct_block
{
	// Explanation("BLOCK INFO", "EMPTY STRING")
	int blockOffset;
	int blockSize;
	int sectionDataSize;
	int resourceDataSize;
	tag_block<global_geometry_block_resource_block> resources;

	BYTE padding34[4];
	short ownerTagSectionOffset;
	BYTE padding35[2];
	BYTE padding36[4];
};
CHECK_STRUCT_SIZE(global_geometry_block_info_struct_block, 40);

struct global_geometry_compression_info_block
{
	real_vector2d positionBoundsX;
	real_vector2d positionBoundsY;
	real_vector2d positionBoundsZ;
	real_vector2d texcoordBoundsX;
	real_vector2d texcoordBoundsY;
	real_vector2d secondaryTexcoordBoundsX;
	real_vector2d secondaryTexcoordBoundsY;
};
CHECK_STRUCT_SIZE(global_geometry_compression_info_block, 56);

struct global_geometry_section_info_struct_block
{
	// Explanation("SECTION INFO", "EMPTY STRING")
	short totalVertexCount;
	short totalTriangleCount;
	short totalPartCount;
	short shadowCastingTriangleCount;
	short shadowCastingPartCount;
	short opaquePointCount;
	short opaqueVertexCount;
	short opaquePartCount;
	BYTE opaqueMaxNodesVertex;
	BYTE transparentMaxNodesVertex;
	short shadowCastingRigidTriangleCount;

	enum GeometryClassification : short
	{
		Worldspace = 0,
		Rigid = 1,
		RigidBoned = 2,
		Skinned = 3,
		UnsupportedReimport = 4,
	};
	GeometryClassification geometryClassification;

	enum GeometryCompressionFlags : short
	{
		CompressedPosition = 0x1,
		CompressedTexcoord = 0x2,
		CompressedSecondaryTexcoord = 0x4,
	};
	GeometryCompressionFlags geometryCompressionFlags;
	tag_block<global_geometry_compression_info_block> eMPTYSTRING;

	BYTE hardwareNodeCount;
	BYTE nodeMapSize;
	short softwarePlaneCount;
	short totalSubpartcont;

	enum SectionLightingFlags : short
	{
		HasLmTexcoords = 0x1,
		HasLmIncRad = 0x2,
		HasLmColors = 0x4,
		HasLmPrt = 0x8,
	};
	SectionLightingFlags sectionLightingFlags;
};
CHECK_STRUCT_SIZE(global_geometry_section_info_struct_block, 44);