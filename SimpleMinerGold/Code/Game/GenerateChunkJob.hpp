#pragma once
#include "Engine/Job/Job.hpp"
#include "Game/Chunk.hpp"


class GenerateChunkJob : public Job {
public:
    Chunk* m_chunk = nullptr;
    void Execute();

};