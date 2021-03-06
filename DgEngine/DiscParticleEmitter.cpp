/*!
* @file DiscParticleEmitter.cpp
*
* @author Frank Hart
* @date 16/01/2014
*
* class definitions: DiscParticleEmitter
*/

#include "DiscParticleEmitter.h"
#include "BasisR3.h"
#include "pugixml.hpp"


//--------------------------------------------------------------------------------
//	@	Statics
//--------------------------------------------------------------------------------
const Point4 DiscParticleEmitter::ORIGIN = DE_DEFAULT_ORIGIN;
const Vector4 DiscParticleEmitter::AXIS = DE_DEFAULT_AXIS;
const float DiscParticleEmitter::RADIUS = DE_DEFAULT_RADIUS;

//--------------------------------------------------------------------------------
//	@	DiscParticleEmitter::DiscParticleEmitter()
//--------------------------------------------------------------------------------
//		Constructor. Uses default parameters.
//--------------------------------------------------------------------------------
DiscParticleEmitter::DiscParticleEmitter() : ParticleEmitter()
{
	//Set the disc
    origin = ORIGIN;
    axis = AXIS;
    discRadius = RADIUS;

	//Set the outer angle
	SetOuterAngle(DE_DEFAULT_OUTERANGLE);

}	//End: DiscParticleEmitter::DiscParticleEmitter()


//--------------------------------------------------------------------------------
//	@	DiscParticleEmitter::DiscParticleEmitter()
//--------------------------------------------------------------------------------
//		
//--------------------------------------------------------------------------------
DiscParticleEmitter::DiscParticleEmitter(const DiscParticleEmitter& other) :
ParticleEmitter(other), generator(other.generator), origin(other.origin),
axis(other.axis), discRadius(other.discRadius), outerAngle(other.outerAngle)
{
}	//End: DiscParticleEmitter::DiscParticleEmitter()


//--------------------------------------------------------------------------------
//	@	DiscParticleEmitter::operator=()
//--------------------------------------------------------------------------------
//		
//--------------------------------------------------------------------------------
DiscParticleEmitter& DiscParticleEmitter::operator=(const DiscParticleEmitter& other)
{
	if (this == &other)
		return *this;

	ParticleEmitter::operator=(other);

	generator = other.generator;
	origin = other.origin;
	axis = other.axis;
	discRadius = other.discRadius;
	outerAngle = other.outerAngle;

	return *this;
}	//End: DiscParticleEmitter::operator=()


//--------------------------------------------------------------------------------
//	@	operator>>()
//--------------------------------------------------------------------------------
//		Build particle emitter from an xml node
//--------------------------------------------------------------------------------
pugi::xml_node& operator>>(pugi::xml_node& node, DiscParticleEmitter& dest)
{
	//Build base data
	dest.BuildBase(node);

	//iterate through all nodes
	for (pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
	{
		//Get the name of the node
		std::string tag = it->name();

		if (tag == "outerAngle")
		{
			float a;
			DgString(it->child_value()) >> a;
			dest.SetOuterAngle(a);
		}
	}

	return node;

}	//End: Read_VQS()


//--------------------------------------------------------------------------------
//	@	DiscParticleEmitter::SetVQS()
//--------------------------------------------------------------------------------
//		
//--------------------------------------------------------------------------------
void DiscParticleEmitter::SetVQS(const VQS& in_vqs)
{
	//Set vqs
	vqs = in_vqs;

	//Transform the disc
	origin = ORIGIN;
	axis = AXIS;
	discRadius = RADIUS;

	origin *= vqs;
	vqs.RotateSelf(axis);
	discRadius *= vqs.S();

}	//End: ParticleEmitter::SetNumberParticles()


//--------------------------------------------------------------------------------
//	@	DiscParticleEmitter::SetVQS()
//--------------------------------------------------------------------------------
//		
//--------------------------------------------------------------------------------
void DiscParticleEmitter::SetOuterAngle(float ang)
{
	outerAngle = ang;

}	//End: DiscParticleEmitter::SetVQS()


//--------------------------------------------------------------------------------
//	@	DiscParticleEmitter::GetNumberToEmit()
//--------------------------------------------------------------------------------
//		Culls particles against a frustum
//--------------------------------------------------------------------------------
void DiscParticleEmitter::SetNewParticle(Particle* par, float dt)
{
	if (IsZero(discRadius))
	{
		par->position = origin;
		par->velocity = axis * entry_speed.Get() * vqs.S() + velocity_global;
	}
	else
	{
		//Get random orthogonal vector
		BasisR3 b(axis, GetRandomVector());

		//Get distance from origin along v.
		float dist = generator.GetUniform<float>(0.0f, discRadius);
		float ang = dist * outerAngle / discRadius;

		Quaternion q(b.x2(), ang);

        par->position = origin + b.x1() * dist - dt * velocity_global;
        par->velocity = q.Rotate(axis) * entry_speed.Get() * vqs.S() + velocity_global;
        
	}

	par->acceleration = acceleration_global;
	par->life = 0.0f;
	
}	//End: ParticleEmitter::FrustumCull()