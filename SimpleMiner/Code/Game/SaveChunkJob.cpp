#include "SaveChunkJob.hpp"
#include "Engine/Core/FileUtils.hpp"

void SaveChunkJob::Execute()
{
    m_chunk->m_chunkState = ChunkState::DEACTIVATING_SAVING;
	ChunkSave save = m_chunk->CreateSave();
	FileWriteBuffer(&save.saveData, m_chunk->GetSaveName());
	m_chunk->m_chunkState = ChunkState::DEACTIVATING_SAVED;
	m_done = true;
}
