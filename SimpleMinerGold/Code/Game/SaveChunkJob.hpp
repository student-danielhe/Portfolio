#pragma once
#include "Engine/Job/Job.hpp"
#include "Game/Chunk.hpp"


class SaveChunkJob : public Job {
public:
	Chunk* m_chunk = nullptr;
	void Execute();

};