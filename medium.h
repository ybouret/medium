//! \file
#ifndef MEDIUM_INCLUDED
#define MEDIUM_INCLUDED 1

#if !defined(MEDIUM_TEST)
#include <Arduino.h>
#define MEDIUM_ASSERT(EXPR)
#else
#include <cassert>
#define MEDIUM_ASSERT(EXPR) assert(EXPR)
#endif

// timing macros
//-----------------------------------------------------------------------------
//
// timing macros, GetCurrentTime is in seconds
//
//-----------------------------------------------------------------------------
#define TSYS() (micros())
#define TSYS2TIME(tmx) (1.0e-6f * ((float)(tmx)))
#define Medium_GetCurrentTime() TSYS2TIME(TSYS())

#define Medium_streq(lhs, arg) (0 == strcmp(lhs, arg))

class Medium
{
  public:
#define MEDIUM_CHECK_NODE(node)        \
    MEDIUM_ASSERT(node != NULL);       \
    MEDIUM_ASSERT(NULL == node->next); \
    MEDIUM_ASSERT(NULL == node->prev)

    //! generic doubly linked list
    template <typename NODE>
    class ListOf
    {
      public:
        NODE *head;
        NODE *tail;
        unsigned long size;

        inline explicit ListOf() throw() : head(NULL), tail(NULL), size(0)
        {
        }

        inline virtual ~ListOf() throw()
        {
        }

        inline void push_back(NODE *node) throw()
        {
            MEDIUM_CHECK_NODE(node);
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
            MEDIUM_CHECK_NODE(node);
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
                MEDIUM_ASSERT(NULL == node);
                return node;
            case 1:
            {
                head = tail = NULL;
                size = 0;
                MEDIUM_CHECK_NODE(node);
                return node;
            }
            default:
                break;
            }
            tail = node->prev;
            tail->next = NULL;
            node->prev = NULL;
            --size;
            MEDIUM_CHECK_NODE(node);
            return node;
        }

        inline NODE *pop_front() throw()
        {
            NODE *node = head;
            switch (size)
            {
            case 0:
                MEDIUM_ASSERT(NULL == node);
                return node;

            case 1:
            {
                head = tail = NULL;
                size = 0;
                MEDIUM_CHECK_NODE(node);
                return node;
            }
            default:
                break;
            }
            head = node->next;
            head->prev = NULL;
            node->next = NULL;
            --size;
            MEDIUM_CHECK_NODE(node);
            return node;
        }

      private:
        ListOf(const ListOf &);
        ListOf &operator=(const ListOf &);
    };

    //! generic pool
    template <typename NODE>
    class PoolOf
    {
      public:
        NODE *top;
        unsigned long size;

        inline explicit PoolOf() throw() : top(NULL), size(0) {}
        inline virtual ~PoolOf() throw() {}

        inline void store(NODE *node) throw()
        {
            MEDIUM_CHECK_NODE(node);
            node->next = top;
            top = node;
            ++size;
        }

        inline NODE *query() throw()
        {
            MEDIUM_ASSERT(size > 0);
            NODE *node = top;
            top = top->next;
            node->next = NULL;
            --size;
            MEDIUM_CHECK_NODE(node);
            return node;
        }

      private:
        PoolOf(const PoolOf &);
        PoolOf &operator=(const PoolOf &);
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

    static const unsigned maxInputLength = 255;                //!< to process serial input
    static const unsigned maxInputMemory = maxInputLength + 1; //!< memory, keep and extra '\0'
    static const unsigned maxInputWords = 4;                   //!< command + (maxInputWords-1) arguments

    //! check an input is ready
    inline bool inputCompleted() const { return _inputCompleted; }

    //! get read only current input
    inline const char *input() const { return _input; }

    //! get read only current input length
    inline unsigned inputLength() const { return _inputLength; }

    void serialEventCallback(); //!< to be called within serialEvent() function
    void resetInput();          //!< hard reset input status

    //! tokenize input into 0..maxInputArgs
    /**
     * \param sep if NULL, blanks are used
     */
    unsigned splitInput(const char *sep = NULL);
    const char *operator[](const unsigned i) const;

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

    Medium(const Medium &);
    Medium &operator=(const Medium &);
};

#endif
