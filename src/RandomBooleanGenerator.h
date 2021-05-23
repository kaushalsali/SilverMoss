//
// Created by Kaushal Sali on 23/05/21.
//

#ifndef SILVERMOSS_RANDOMBOOLEANGENERATOR_H
#define SILVERMOSS_RANDOMBOOLEANGENERATOR_H

#include <random>

/*
 * Provides an easy to use interface to get boolean result based on a probability.
 */
class RandomBooleanGenerator {
public:
    explicit RandomBooleanGenerator(float probability=0.5);
    explicit RandomBooleanGenerator(unsigned seed, float probability=0.5);
    float getProbability() const;
    void setProbability(float probability);
    bool generate();

private:
    std::random_device m_randomSeedGen;
    std::mt19937 m_randomGenerator;
    std::bernoulli_distribution m_bernoulliDist;
};


#endif //SILVERMOSS_RANDOMBOOLEANGENERATOR_H
