#pragma once

#include "Core/Core.h"
#include "RenderingCoreGen.h"

namespace Ry
{
	class Bitmap;
	enum class PixelFormat;
	enum class PixelStorage;

	class RENDERINGCORE_MODULE Texture
	{

	public:

		Texture() = default;
		virtual ~Texture() = default;
		
		/************************************************************************/
		/* Interface functions                                                  */
		/************************************************************************/

		/**
		 * Binds this texture for rendering usage. Should be called before rendering a vertex array.
		 *
		 * @param Unit The texture unit to bind this texture to.
		 */
		virtual void Bind(uint32 Unit) = 0;
		virtual void Data(void* Data, PixelFormat Format, PixelStorage Storage, uint32 Width, uint32 Height) = 0;
		virtual int32 GetSamples() const = 0;

		/**
		 * Sets this texture up as a blank buffer with the specified width and height.
		 * @param width The width of the texture.
		 * @param height The height of the texture.
		 */
		void AllocateColor(uint32 width, uint32 height);
		void AllocateDepth(uint32 width, uint32 height);

		void Data(const Bitmap* Bitmap);

		void Bind();

	};
}
