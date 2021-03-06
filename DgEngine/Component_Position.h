#ifndef COMPONENT_POSITION_H
#define COMPONENT_POSITION_H

#include "Component.h"
#include "VQS.h"
#include "DgOrderedArray.h"
#include "DgTypes.h"

namespace pugi{class xml_node;}

//--------------------------------------------------------------------------------
//		Positional data, relative to the parent. The parent will be the 
//		world VQS if the entity is not a child of another.
//--------------------------------------------------------------------------------
class Component_POSITION : public Component
{
public:
    Component_POSITION() : parent(ENTITYID::ROOT) {}
    void Clear() { children.reset(); }

public:
	//DATA
	VQS T_WLD_OBJ;	//Set in Systems
	VQS T_PAR_OBJ;

	//VQS heirachy data
	DgOrderedArray<entityID>	children;	//This is set in the PostProcess system.
	entityID					parent;

};


//--------------------------------------------------------------------------------
//		Read from xml_node
//--------------------------------------------------------------------------------
pugi::xml_node& Read(pugi::xml_node&, Component_POSITION&, entityID _parent);

#endif