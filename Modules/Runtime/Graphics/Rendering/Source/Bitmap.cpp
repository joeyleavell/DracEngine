#include "Bitmap.h"
#include "Vector.h"

namespace Ry
{

	PixelBufferFourByteRGBA::PixelBufferFourByteRGBA(int32 Width, int32 Height):
	PixelBuffer(Width, Height, PixelFormat::RGBA, PixelStorage::FOUR_BYTE_RGBA)
	{
		Data = new uint8[Width * Height * 4];
		memset(Data, 0, Width * Height * 4);
	}

	uint32 PixelBufferFourByteRGBA::GetPixel(int32 X, int32 Y) const
	{
		uint8 Red = ((uint8*)Data)[(X + Y * Width) * 4 + 0];
		uint8 Green = ((uint8*)Data)[(X + Y * Width) * 4 + 1];
		uint8 Blue = ((uint8*)Data)[(X + Y * Width) * 4 + 2];
		uint8 Alpha = ((uint8*)Data)[(X + Y * Width) * 4 + 3];

		return (Red << 0) | (Green << 8) | (Blue << 16) | (Alpha << 24);
	}
	
	void PixelBufferFourByteRGBA::SetPixel(int32 X, int32 Y, uint32 Value)
	{
		((uint8*) Data)[4 * (X + Y * Width) + 0] = (Value >> 0) & 0xFF;
		((uint8*) Data)[4 * (X + Y * Width) + 1] = (Value >> 8) & 0xFF;
		((uint8*) Data)[4 * (X + Y * Width) + 2] = (Value >> 16) & 0xFF;
		((uint8*) Data)[4 * (X + Y * Width) + 3] = (Value >> 24) & 0xFF;
	}

	PixelBufferThreeByteRGB::PixelBufferThreeByteRGB(int32 Width, int32 Height) :
		PixelBuffer(Width, Height, PixelFormat::RGB, PixelStorage::THREE_BYTE_RGB)
	{
		Data = new uint8[Width * Height * 3];
		memset(Data, 0, Width * Height * 3);
	}

	uint32 PixelBufferThreeByteRGB::GetPixel(int32 X, int32 Y) const
	{
		uint8 Red = ((uint8*)Data)[(X + Y * Width) * 3 + 0];
		uint8 Green = ((uint8*)Data)[(X + Y * Width) * 3 + 1];
		uint8 Blue = ((uint8*)Data)[(X + Y * Width) * 3 + 2];

		return (Red << 0) | (Green << 8) | (Blue << 16) | (0xFF << 24);
	}

	void PixelBufferThreeByteRGB::SetPixel(int32 X, int32 Y, uint32 Value)
	{
		((uint8*)Data)[3 * (X + Y * Width) + 0] = (Value >> 0) & 0xFF;
		((uint8*)Data)[3 * (X + Y * Width) + 1] = (Value >> 8) & 0xFF;
		((uint8*)Data)[3 * (X + Y * Width) + 2] = (Value >> 16) & 0xFF;
	}

	PixelBufferRed8::PixelBufferRed8(int32 Width, int32 Height) :
		PixelBuffer(Width, Height, PixelFormat::GRAYSCALE, PixelStorage::RED8)
	{
		Data = new uint8[Width * Height];
		memset(Data, 0, Width * Height);
	}

	uint32 PixelBufferRed8::GetPixel(int32 X, int32 Y) const
	{
		return ((uint8*)Data)[X + Y * Width];
	}

	void PixelBufferRed8::SetPixel(int32 X, int32 Y, uint32 Value)
	{
		((uint8*)Data)[X + Y * Width] = Value & 0xFF;
	}
	
	Bitmap::Bitmap(uint32 Width, uint32 Height, PixelStorage Storage)
	{
		switch(Storage)
		{
		case PixelStorage::FOUR_BYTE_RGBA:
			this->Buffer = new PixelBufferFourByteRGBA(Width, Height);
			break;
		case PixelStorage::THREE_BYTE_RGB:
			this->Buffer = new PixelBufferThreeByteRGB(Width, Height);
			break;
		case PixelStorage::RED8:
			this->Buffer = new PixelBufferRed8(Width, Height);
			break;
		default:
			std::cerr << "Bitmap type not supported" << std::endl;
			break;
		}
	}
	
	Bitmap::~Bitmap()
	{
		delete Buffer;
	}

	uint32 Bitmap::GetPixel(int32 X, int32 Y) const
	{
		return Buffer->GetPixel(X, Y);
	}

	void Bitmap::SetPixel(int32 X, int32 Y, uint32 Value)
	{
		Buffer->SetPixel(X, Y, Value);
	}

	void Bitmap::SetPixelBuffer(PixelBuffer* Buffer)
	{
		delete this->Buffer;

		this->Buffer = Buffer;
	}

	PixelBuffer* Bitmap::GetPixelBuffer() const
	{
		return Buffer;
	}

	int32 Bitmap::GetWidth() const
	{
		return Buffer->GetWidth();
	}
	
	int32 Bitmap::GetHeight() const
	{
		return Buffer->GetHeight();
	}

	void Bitmap::DrawTexture(Bitmap* Other, int32 X, int32 Y)
	{
		if (!Other)
			return;

		int32 Width = GetWidth();
		int32 Height = GetHeight();
		
		for(int32 DrawX = 0; DrawX < Other->GetWidth(); DrawX++)
		{
			for(int32 DrawY = 0; DrawY < Other->GetHeight(); DrawY++)
			{
				int32 AX = X + DrawX;
				int32 AY = Y + DrawY;

				if(AX >= 0 && AX < Width && AY >= 0 && AY < Height)
				{
					SetPixel(AX, AY, Other->GetPixel(DrawX, DrawY));
				}
			}
		}
	}

	void Bitmap::DrawBox(int32 X, int32 Y, int32 W, int32 H, const Ry::Vector4& Color)
	{
		for(int32 DrawX = 0; DrawX < W; DrawX++)
		{
			for(int32 DrawY = 0; DrawY < H; DrawY++)
			{
				int32 AX = X + DrawX;
				int32 AY = Y + DrawY;

				if ((DrawX == 0 || DrawX == W - 1 || DrawY == 0 || DrawY == H - 1) && AX >= 0 && AX < GetWidth() && AY >= 0 && AY < GetHeight())
				{
					SetPixel(AX, AY, Ry::VectorToColor(Color));
				}
			}
		}
	}

	void Bitmap::FlipY()
	{
		for(int32 X = 0; X < GetWidth(); X++)
		{
			for(int32 Y = 0; Y < (uint32) (GetHeight() / 2); Y++)
			{
				uint32 Temp = GetPixel(X, Y);
				SetPixel(X, Y, GetPixel(X, GetHeight() - Y - 1));
				SetPixel(X, GetHeight() - Y - 1, Temp);
			}
		}
	}

	uint32 VectorToColor(const Ry::Vector4& Color)
	{
		uint32 R = (uint32) (Color.r * 255.0f);
		uint32 G = (uint32)(Color.r * 255.0f);
		uint32 B = (uint32)(Color.r * 255.0f);

		return (R << 0) | (G << 8) | (B << 16) | (0xff << 24);
	}
	
}