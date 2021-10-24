//
// Created by Kaushal Sali on 23/05/21.
//

#include "RandomBooleanGenerator.h"
#ifdef ARCH_WIN
#include <ctime>

RandomBooleanGenerator::RandomBooleanGenerator(float probability/*=0.5*/) :
        m_randomGenerator(time(0)),
        m_bernoulliDist(probability)
{
}
#else
RandomBooleanGenerator::RandomBooleanGenerator(float probability/*=0.5*/) :
        m_randomGenerator(m_randomSeedGen()),
        m_bernoulliDist(probability)
{
}
#endif

RandomBooleanGenerator::RandomBooleanGenerator(unsigned seed, float probability/*=0.5*/) :
        m_randomGenerator(seed),
        m_bernoulliDist(probability)
{
}
float RandomBooleanGenerator::getProbability() const {
    return m_bernoulliDist.p();
}
void RandomBooleanGenerator::setProbability(float probability) {
    std::bernoulli_distribution dist(probability);
    m_bernoulliDist = dist;
}

bool RandomBooleanGenerator::generate() {
    return m_bernoulliDist(m_randomGenerator);
}
