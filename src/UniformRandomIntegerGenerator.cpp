//
// Created by Kaushal Sali on 16/10/20.
//

#include "UniformRandomIntegerGenerator.h"

UniformRandomIntegerGenerator::UniformRandomIntegerGenerator(int min /*=std::numeric_limits<int>::min()*/, int max /*=std::numeric_limits<int>::max()*/) :
    m_randomGenerator(m_randomSeedGen()),
    m_uniformDist(min, max)
{
}

UniformRandomIntegerGenerator::UniformRandomIntegerGenerator(unsigned seed, int min /*=std::numeric_limits<int>::min()*/, int max /*=std::numeric_limits<int>::max()*/) :
    m_randomGenerator(seed),
    m_uniformDist(min, max)
{
}

int UniformRandomIntegerGenerator::generate() {
    return m_uniformDist(m_randomGenerator);
}

void UniformRandomIntegerGenerator::setRange(int min, int max) {
    std::uniform_int_distribution<> dist(min, max);
    m_uniformDist = dist; //std::move not used since trivially copyable
}

