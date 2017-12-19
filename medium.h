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
  static const unsigned maxInputLength = 255;
  static const unsigned maxInputMemory = maxInputLength + 1;
  static const unsigned maxInputWords = 4;
  static const unsigned maxOutputLength = 255;
  static const unsigned maxOutputMemory = maxOutputLength + 1;

  //! check an input is ready
  inline bool inputCompleted() const { return _inputCompleted; }

  //! get read only current input
  inline const char *input() const { return _input; }

  //! get read only current input length
  inline unsigned inputLength() const { return _inputLength; }

  //! to be called within serialEvent() function
  void processSerialInput();
  void resetInput(); //!< hard reset input status
  //! tokenize input into 0..maxInputArgs
  /**
   * \param sep if NULL, blanks are used 
   */
  unsigned findInputWords(const char *sep = NULL);
  const char *getInputWord(const unsigned i) const;

  void resetOutput(); //!< clean output
  inline bool outputIsActive() const { return _outputIsActive; }
  inline bool outputIsReady() const { return !_outputIsActive; }
  void processSerialOutput();

  //! print a formatted message, must be outputReady
  void print(const char *fmt, ...);

  //! prepare an object for I/O
  Medium();
  ~Medium() throw();

  static float Triangle(float t, const float T);
  static float SineWave(float t, const float T);

private:
  char *_input;                //!< allocated input buffer
  unsigned _inputLength;       //!< current length
  bool _inputCompleted;        //!< end of line, or max size reached
  char *_words[maxInputWords]; //!< split words from input

  char *_output;           //!< allocated output buffer
  unsigned _outputLength;  //!< output length
  unsigned _outputCurrent; //!< output current position
  bool _outputIsActive;    //!< there is something in the output

  Medium(const Medium &);
  Medium &operator=(const Medium &);
};

#endif
