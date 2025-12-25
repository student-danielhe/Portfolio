#include "ActorHandle.hpp"
const ActorHandle ActorHandle::INVALID_ID(0x0000ffff,0x0000ffff);
ActorHandle::ActorHandle(unsigned int uid, unsigned int index) {
	m_data = (uid << 16) | (index & 0x0000ffff);
}

bool ActorHandle::IsValid()const {
	return *this != ActorHandle::INVALID_ID;
}
unsigned int ActorHandle::GetIndex()const {
	return m_data & 0x0000ffff;
}
bool ActorHandle::operator==(const ActorHandle& other) const {
	return m_data == other.m_data;
}
bool ActorHandle::operator!=(const ActorHandle& other) const {
	return m_data != other.m_data;
}