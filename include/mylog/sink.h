#pragma once

#include <string>

class Sink{
    public:
        virtual ~Sink() = default;
        virtual void write(const std::string& message) = 0;
};