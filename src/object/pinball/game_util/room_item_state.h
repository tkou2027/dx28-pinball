#pragma once

class RoomItemState
{
public:
	enum class State
	{
		IDLE,
		TRANS_IN,
		ACTIVE,
		TRANS_OUT,
		DONE
	};
	virtual ~RoomItemState() = default;
	virtual void Update();
private:
};