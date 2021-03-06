//================================================================================
// @ Texture.h
// 
// Class: Texture
//
// Base Texture class. Textures are essentially an array of one or more mipmaps 
// (more for animated textures). Typically they are used to texture a mesh. 
//
// -------------------------------------------------------------------------------
//
// Author: Frank Hart
// Date last modified: 2013
//
//================================================================================

#ifndef TEXTUREBASE_H
#define TEXTUREBASE_H

#include <string>
#include "DgTypes.h"
#include "BaseClass.h"

class Mipmap;

//--------------------------------------------------------------------------------
//	@	Texture
//--------------------------------------------------------------------------------
class Texture : public BaseClass<Texture>
{
public:

  enum
  {
    TYPE_MASK    = 0xFF000000,
    TYPE_S       = 0x11000000,
    TYPE_A       = 0x12000000,
    RESERVE_MASK = 0x000FFFFF 
  };

	//Constructor/Destructor
	Texture(){}
	virtual ~Texture() {}

	//Copy constructor
	Texture(const Texture& t) {}

	//Return mipmap for input time 't' (for animations)
	virtual uint32_t GetMipmap(uint32 time = 0) const =0;

};

#endif