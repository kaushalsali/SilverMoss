//
// Created by Kaushal Sali on 16/10/20.
//

#ifndef BEATPORTCODINGCHALLANGE_UNIFORMRANDOMINTEGERGENERATOR_H
#define BEATPORTCODINGCHALLANGE_UNIFORMRANDOMINTEGERGENERATOR_H

#include <random>

/*
 * Provides an easy to use interface to get a random integer within a range.
 */
class UniformRandomIntegerGenerator {
public:
    explicit UniformRandomIntegerGenerator(int min=std::numeric_limits<int>::min(), int max=std::numeric_limits<int>::max());
    explicit UniformRandomIntegerGenerator(unsigned seed, int min=std::numeric_limits<int>::min(), int max=std::numeric_limits<int>::max());
    int generate();
    void setRange(int min, int max);

private:
    #ifndef ARCH_WIN
    	std::random_device m_randomSeedGen;
    #endif
    std::mt19937 m_randomGenerator;
    std::uniform_int_distribution<> m_uniformDist;
};


#endif //BEATPORTCODINGCHALLANGE_UNIFORMRANDOMINTEGERGENERATOR_H
