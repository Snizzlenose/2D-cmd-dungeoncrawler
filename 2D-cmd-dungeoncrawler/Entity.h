#pragma once

#include "Vector2i.h"
#include "Enums.h"

class Entity
{
	public:
		Entity( const Vector2i& position, const Portrait& portrait );

		const Portrait portrait;

		void SetPosition( const Vector2i& position );
		const Vector2i GetPosition( ) const;

	private:
		Vector2i _position;
};