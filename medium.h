//! \file
#ifndef MEDIUM_INCLUDED
#define MEDIUM_INCLUDED 1
#include <Arduino.h>

// timing macros
//-----------------------------------------------------------------------------
//
// timing macros, GetCurrentTime is in seconds
//
//-----------------------------------------------------------------------------
#define TSYS() (micros())
#define TSYS2TIME(tmx) (1.0e-6f * ((float)(tmx)))
#define Medium_GetCurrentTime() TSYS2TIME(TSYS())

class Medium
{
public:
  static const unsigned maxInputLength = 200;
  static const unsigned maxInputMemory = maxInputLength + 1;
  static const unsigned maxInputWords = 3;

  //! check an input is ready
  inline bool inputCompleted() { return _inputCompleted; }

  //! get read only current input
  inline const char *input() { return _input; }

  //! get read only current input length
  inline unsigned inputLength() { return _inputLength; }

  void processSerialInput(); //!< to be called within serialEvent() function
  void resetInput();         //!< hard reset input status
  //! tokenize input into 0..maxInputArgs
  /**
   * \param sep if NULL, blanks are used 
   */
  unsigned findInputWords(const char *sep = NULL);
  const char *getInputWord(const unsigned i) const;

  //! prepare an object for I/O
  Medium();
  ~Medium() throw();

  static float Triangle(float t, const float T);
  static float SineWave(float t, const float T);

private:
  char *_input;          //!< allocated input buffer
  unsigned _inputLength; //!< current length
  bool _inputCompleted;  //!< end of line, or max size reached
  char *_words[maxInputWords];

  Medium(const Medium &);
  Medium &operator=(const Medium &);
};

#endif
