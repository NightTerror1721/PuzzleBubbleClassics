#include "game_basics.h"


Json GameObjectIdentifier::serialize() const
{
	return _id;
}

void GameObjectIdentifier::deserialize(const Json& json)
{
	_id = json;
}

GameObjectIdentifier GameObjectIdentifier::make()
{
	static UInt64 generator = 0;
	GameObjectIdentifier goid;
	goid._id = ++generator;
	return goid;
}

std::ostream& operator<< (std::ostream& left, const GameObjectIdentifier& right)
{
	return left << right._id;
}

std::istream& operator>> (std::istream& left, GameObjectIdentifier& right)
{
	return left >> right._id;
}



