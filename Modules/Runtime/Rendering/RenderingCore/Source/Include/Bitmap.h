#pragma once

#include "Core/Core.h"
#include "RenderingCoreGen.h"

namespace Ry
{
	class Text;
	class Vector4;
	class Vector3;

	/**
	 * Specifies a pixel format for textures.
	 */
	enum class RENDERINGCORE_MODULE PixelFormat
	{
		NONE, RGB, RGBA, GRAYSCALE, DEPTH
	};

	enum class PixelStorage
	{
		NONE, FOUR_BYTE_RGBA, THREE_BYTE_RGB, RED8, FLOAT
	};
	
	class PixelBuffer
	{
	public:
		PixelBuffer(int32 Width, int32 Height, PixelFormat Format, PixelStorage Storage): Width(Width),Height(Height), Data(nullptr), Format(Format), Storage(Storage) {};
		virtual ~PixelBuffer() { delete[] Data; };

		int32 GetWidth() const { return Width; }
		int32 GetHeight() const { return Height; }

		/**
		 *
		 * @return uint32 The pixel in R8G8B8A8 format.
		 */
		virtual uint32 GetPixel(int32 X, int32 Y) const = 0;

		/**
		 *
		 * @param Value The pixel in R8G8B8A8 format.
		 */
		virtual void SetPixel(int32 X, int32 Y, uint32 Value) = 0;

		PixelFormat GetPixelFormat()
		{
			return Format;
		}

		PixelStorage GetPixelStorage()
		{
			return Storage;
		}

		template <typename T>
		T* GetData() const
		{
			return static_cast<T*>(Data);
		}

		void SetData(void* Data)
		{
			this->Data = Data;
		}
		
	protected:
		PixelFormat Format;
		PixelStorage Storage;
		int32 Width;
		int32 Height;
		void* Data;
	};

	class PixelBufferFourByteRGBA : public PixelBuffer
	{
	public:
		PixelBufferFourByteRGBA(int32 Width, int32 Height);

		uint32 GetPixel(int32 X, int32 Y) const override;
		void SetPixel(int32 X, int32 Y, uint32 Value) override;
	};

	class PixelBufferThreeByteRGB : public PixelBuffer
	{
	public:
		PixelBufferThreeByteRGB(int32 Width, int32 Height);

		uint32 GetPixel(int32 X, int32 Y) const override;
		void SetPixel(int32 X, int32 Y, uint32 Value) override;
	};

	class PixelBufferRed8 : public PixelBuffer
	{
	public:
		PixelBufferRed8(int32 Width, int32 Height);

		uint32 GetPixel(int32 X, int32 Y) const override;
		void SetPixel(int32 X, int32 Y, uint32 Value) override;
	};
	
	class RENDERINGCORE_MODULE Bitmap
	{
	public:

		Bitmap(uint32 Width, uint32 Height, PixelStorage Storage);
		~Bitmap();

		/**
		 *
		 * @return uint32 The pixel in R8G8B8A8 format.
		 */
		uint32 GetPixel(int32 X, int32 Y) const;

		/**
		 *
		 * @param Value The pixel in R8G8B8A8 format.
		 */
		void SetPixel(int32 X, int32 Y, uint32 Value);

		template <typename T>
		T* GetData() const
		{
			return Buffer->GetData<T>();
		}

		void SetData(void* Data)
		{
			Buffer->SetData(Data);
		}

		void FlipY();

		void SetPixelBuffer(PixelBuffer* Buffer);
		PixelBuffer* GetPixelBuffer() const;

		int32 GetWidth() const;
		int32 GetHeight() const;

		void DrawTexture(Bitmap* Other, int32 X, int32 Y);
		void DrawBox(int32 X, int32 Y, int32 W, int32 H, const Ry::Vector4& Color);

	private:

		PixelBuffer* Buffer;

	};

	uint32 VectorToColor(const Ry::Vector4& Color);
}
