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
  //! generic doubly linked list
  template <typename NODE>
  class List
  {
  public:
    NODE *head;
    NODE *tail;
    unsigned long size;

    inline explicit List() throw() : head(NULL), tail(NULL), size(0)
    {
    }

    inline virtual ~List() throw()
    {
    }

    inline void push_back(NODE *node) throw()
    {
      if (size <= 0)
      {
        head = tail = node;
      }
      else
      {
        tail->next = node;
        node->prev = tail;
        tail = node;
      }
      ++size;
    }

    inline void push_front(NODE *node) throw()
    {
      if (size <= 0)
      {
        head = tail = node;
      }
      else
      {
        head->prev = node;
        node->next = head;
        head = node;
      }
      ++size;
    }

    inline NODE *pop_back() throw()
    {
      NODE *node = tail;
      switch (size)
      {
      case 0:
        return node;
      case 1:
      {
        head = tail = NULL;
        size = 0;
        return node;
      }
      default:
        break;
      }
      tail = node->prev;
      tail->next = NULL;
      node->prev = NULL;
      --size;
      return node;
    }

    inline NODE *pop_front() throw()
    {
      NODE *node = head;
      switch (size)
      {
      case 0:
        return node;

      case 1:
      {
        head = tail = NULL;
        size = 0;
        return node;
      }
      default:
        break;
      }
      head = node->next;
      head->prev = NULL;
      node->next = NULL;
      --size;
      return node;
    }

  private:
    List(const List &);
    List &operator=(const List &);
  };

  //! generic pool
  template <typename NODE>
  class Pool
  {
  public:
    NODE *top;
    unsigned long size;

    inline explicit Pool() throw() : top(NULL), size(0) {}
    inline virtual ~Pool() throw() {}

    inline void store(NODE *node) throw()
    {
      node->next = top;
      top = node;
      ++size;
    }

    inline NODE *query() throw()
    {
      //assume size>0
      NODE *node = top;
      top = top->next;
      node->next = NULL;
      --size;
      return node;
    }

  private:
    Pool(const Pool &);
    Pool &operator=(const Pool &);
  };

  //! generic node
  template <typename T>
  class NodeOf
  {
  public:
    NodeOf *next;
    NodeOf *prev;
    T data;

    inline NodeOf() throw() : next(NULL), prev(NULL), data() {}
    inline NodeOf(const T &args) throw() : next(NULL), prev(NULL), data(args) {}
    inline ~NodeOf() throw() {}

  private:
    NodeOf(const NodeOf &);
    NodeOf &operator=(const NodeOf &);
  };

  static const unsigned maxInputLength = 255;
  static const unsigned maxInputMemory = maxInputLength + 1;
  static const unsigned maxInputWords = 4;
  static const unsigned maxOutputLength = 128;

  //! check an input is ready
  inline bool inputCompleted() const { return _inputCompleted; }

  //! get read only current input
  inline const char *input() const { return _input; }

  //! get read only current input length
  inline unsigned inputLength() const { return _inputLength; }

  //! to be called within serialEvent() function
  void serialEventCallback();
  void resetInput(); //!< hard reset input status
  //! tokenize input into 0..maxInputArgs
  /**
   * \param sep if NULL, blanks are used 
   */
  unsigned findInputWords(const char *sep = NULL);
  const char *getInputWord(const unsigned i) const;

  void loopCallback();

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

  typedef NodeOf<char> CharNode;
  typedef List<CharNode> CharList;
  typedef Pool<CharNode> CharPool;
  CharList outputQ;
  CharPool outputP;
  CharNode *nodes;

  Medium(const Medium &);
  Medium &operator=(const Medium &);
};

#endif
