#ifndef PONG_RANDOMENGINE_H
#define PONG_RANDOMENGINE_H

#include "random"

/**
 * @class RandomEngine
 *
 * @brief Singleton class for generating random numbers using std::mt19937.
 *
 * RandomEngine provides a consistent random number generator using std::mt19937 engine.
 * This class is implemented as a singleton to ensure only one instance of the random number
 * generator is used throughout the program.
 *
 * @attention getInstance() returns a reference to the
 * same instance of RandomEngine every time it is called. This means that any modifications to the
 * instance will be reflected across all usages of RandomEngine in the program.
 */
class RandomEngine {
public:
  static RandomEngine &getInstance() {
    static RandomEngine instance;
    return instance;
  }

  std::mt19937 &getEngine() {
    return this->engine;
  }

private:
  std::random_device randomDevice;
  std::mt19937 engine;

  RandomEngine() : engine(randomDevice()){};

  // Both the copy constructor and the copy assignment operator
  // are deleted to prevent creating a copy of the RandomEngine instance.
public:
  RandomEngine(const RandomEngine &) = delete;
  RandomEngine &operator=(const RandomEngine &) = delete;
};

#endif//PONG_RANDOMENGINE_H