#include <precomp.h>

#include "Texture.h"



Render::Texture::Texture(const std::string& name, Render::TextureType type, GLenum target, int channels)
	: m_name(name.substr( name.find_last_of('/') + 1)), m_type(type)
{    
    if ( m_name.empty() )
    {
        m_name = name;
        m_ID = LoadNativeTexture( std::string{ "Resources/Textures/" }.append( name ), target, channels );
    }
    else
	    m_ID = LoadNativeTexture(name, target, channels);
}

unsigned Render::Texture::GetID() const
{
	return m_ID;
}

std::string Render::Texture::GetName() const
{
	return m_name;
}

Render::TextureType Render::Texture::GetType() const
{
	return m_type;
}


unsigned Render::Texture::GenID( GLenum target )
{
    unsigned id;
    glGenTextures( 1, &id );
    glBindTexture( target, id );

    return id;
}

Render::RawImage Render::Texture::LoadRawImage( const std::string &name, int desiredChannels )
{        
    int width, height, nChannels;
    
    unsigned char *data = stbi_load( name.c_str(), &width, &height, &nChannels, desiredChannels );	

	return { width, height, nChannels, data };
}

GLint Render::Texture::GetFormat( int nChannels )
{    
	switch ( nChannels )
    {
    case 2:
        return GL_RG;
    case 3:
        return GL_RGB;
    case 4:
        return GL_RGBA;
    default:
        return GL_RED;
    }
}

void Render::Texture::BindTextureImage( unsigned id, RawImage& rawimage, GLenum target, bool genmm )
{
    auto [width, height, nChannels, data] = rawimage.Get();
    
    auto iForm = GetFormat( nChannels );

	BindTextureImage( id, width, height, iForm, data, target, genmm );
}

void Render::Texture::BindTextureImage( unsigned id, int width, int height, GLint internalformat,
                                        const stbi_uc *data, GLenum target, bool genmm, GLenum format, GLint level, GLenum type )
{
    format = format != 0 ? format : internalformat;
    glBindTexture( target, id );
    glTexImage2D( target, level, internalformat, width, height, 0, format, type, data );
    if (genmm)
		glGenerateMipmap( target );
}

std::string Render::Texture::TypeToString(TextureType type)
{
    switch (type)
    {
    case TextureType::DIFFUSE:
        return std::string{"diffuse"};
        // return std::string{"texture_diffuse"};
    case TextureType::NORMAL:
        return std::string{"normal"};
        // return std::string{"texture_normal"};
    case TextureType::SPECULAR:
        return std::string{"specular"};
        // return std::string{"texture_specular"};
    case TextureType::HEIGHT:
        return std::string{"texture_height"};
        // return std::string{"texture_height"};
    case TextureType::TEX_ERRORTYPE: default:
        break;
    }

    return std::string{"ErrorTextureType"};
}

template<typename T>
void Render::Texture::SetTexParam( GLenum pname, T param, GLenum target )
{
    glTexParameteri( target, pname, param );
}

template<typename T>
void Render::Texture::SetTexParam( GLenum pname, const T* param, GLenum target )
{
    glTexParameteriv( target, pname, param );
}

unsigned Render::Texture::LoadNativeTexture( const std::string &name, GLenum target, int desiredChannels)
{		
    auto id = GenID( target );
	RawImage rawIm{ LoadRawImage( name, desiredChannels ) };
	

	if ( rawIm.data == nullptr )
    {
        LOG_WARN( "Failed to load texture\nTexture: {}", name );

#ifdef _DEBUG
        LOG_WARN( "Error code: {}", glGetError() );
#endif // _DEBUG
        
        return 0;
    }

    BindTextureImage( id, rawIm, target );

	SetTexParam( GL_TEXTURE_WRAP_S,     GL_REPEAT, target );
    SetTexParam( GL_TEXTURE_WRAP_T,     GL_REPEAT, target );    
    SetTexParam( GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR, target );
    SetTexParam( GL_TEXTURE_MAG_FILTER, GL_LINEAR, target );

	LOG_INFO("Loaded texture {}", name);
    glBindTexture(target, 0);

	return id;
}



