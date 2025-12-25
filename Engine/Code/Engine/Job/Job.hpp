#pragma once

class Job {
public:
    bool m_done = false;
    virtual void Execute()=0;
};