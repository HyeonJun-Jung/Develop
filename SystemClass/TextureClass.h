#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.h
////////////////////////////////////////////////////////////////////////////////



//////////////
// INCLUDES //
//////////////
#include <d3d11.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class TextureClass
{
	
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	//Tga파일 사용 시
	//bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);		

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	
	ID3D11ShaderResourceView* m_texture = nullptr;
	

	/*
	Tga 파일 사용 시
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};
private:
	bool LoadTarga(char*, int&, int&);

private:
	unsigned char* m_targaData = nullptr;
	ID3D11ShaderResourceView* m_textureView = nullptr;
	*/

};