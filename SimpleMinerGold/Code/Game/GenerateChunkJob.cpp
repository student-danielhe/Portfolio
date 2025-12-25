#include "GenerateChunkJob.hpp"
#include "Engine/Core/FileUtils.hpp"
void GenerateChunkJob::Execute()
{
    m_chunk->m_chunkState = ChunkState::ACTIVATING_LOADING;
	if (!DoesFileExists(m_chunk->GetSaveName())){
		m_chunk->CreateBlocks();
	}
	else {
		m_chunk->CreateBlocksFromSave();
	}

	m_done = true;
	m_chunk->m_chunkState = ChunkState::ACTIVATING_GENERATION_COMPLETE;
}

