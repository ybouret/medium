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

//------------------------------------------------------------------------------
//
// timing macros, GetCurrentTime is in seconds
//
//------------------------------------------------------------------------------
#define TSYS() (micros())
#define TSYS2TIME(tmx) (1.0e-6f * ((float)(tmx)))
#define Medium_GetCurrentTime() TSYS2TIME(TSYS())

#define Medium_streq(lhs, arg) (0 == strcmp(lhs, arg))

//------------------------------------------------------------------------------
//
// C++ macros
//
//------------------------------------------------------------------------------
#define MEDIUM_DISABLE_COPY(CLASS)   private: CLASS(const CLASS &)
#define MEDIUM_DISABLE_ASSIGN(CLASS) private: CLASS & operator=( const CLASS &)
#define MEDIUM_DISABLE_COPY_AND_ASSIGN(CLASS) \
MEDIUM_DISABLE_COPY(CLASS); MEDIUM_DISABLE_ASSIGN(CLASS)

//------------------------------------------------------------------------------
//
// Enclosing class
//
//------------------------------------------------------------------------------
class Medium
{
  public:
#define MEDIUM_CHECK_NODE(node)        \
    MEDIUM_ASSERT(node != NULL);       \
    MEDIUM_ASSERT(NULL == node->next); \
    MEDIUM_ASSERT(NULL == node->prev)

    //__________________________________________________________________________
    //
    //! generic doubly linked list
    //__________________________________________________________________________
    template <typename NODE>
    class ListOf
    {
      public:
        NODE         *head; //!< first item
        NODE         *tail; //!< last item
        unsigned long size; //!< number or items

        //! default constructor
        inline explicit ListOf() throw() : head(NULL), tail(NULL), size(0)
        {
        }

        //! destructor
        inline virtual ~ListOf() throw()
        {
        }

        //! push a new valid node at the back of the list
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

        //! push a new valid node at the front of the list
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

        //! remove the tail node
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

        //! remove the front node
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
        MEDIUM_DISABLE_COPY_AND_ASSIGN(ListOf);
    };

    //__________________________________________________________________________
    //
    //! generic pool
    //__________________________________________________________________________
    template <typename NODE>
    class PoolOf
    {
      public:
        NODE         *top;   //!< the top Node
        unsigned long size;  //!< number of stored nodes

        inline explicit PoolOf() throw() : top(NULL), size(0) {}
        inline virtual ~PoolOf() throw() {}

        //! push a valid node ont the top
        inline void store(NODE *node) throw()
        {
            MEDIUM_CHECK_NODE(node);
            node->next = top;
            top = node;
            ++size;
        }

        //! pop the last stored node
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
        MEDIUM_DISABLE_COPY_AND_ASSIGN(PoolOf);
    };

    //__________________________________________________________________________
    //
    //! generic data node
    //__________________________________________________________________________
    template <typename T>
    class NodeOf
    {
      public:
        NodeOf *next;
        NodeOf *prev;
        T       data;

        inline NodeOf() throw() : next(NULL), prev(NULL), data() {}
        inline NodeOf(const T &args) throw() : next(NULL), prev(NULL), data(args) {}
        inline ~NodeOf() throw() {}

      private:
        MEDIUM_DISABLE_COPY_AND_ASSIGN(NodeOf);
    };

    //__________________________________________________________________________
    //
    // functions to read input through serial interface
    //__________________________________________________________________________
    static const unsigned maxInputLength = 255;                //!< to process serial input
    static const unsigned maxInputMemory = maxInputLength + 1; //!< memory, keep and extra '\0'
    static const unsigned maxInputWords  = 4;                  //!< command + (maxInputWords-1) arguments

    //! check an input is ready
    inline bool inputCompleted() const { return _inputCompleted; }

    //! get read only current input
    inline const char *input() const { return _input; }

    //! get read only current input length
    inline unsigned inputLength() const { return _inputLength; }

    void serialEventCallback(); //!< to be called within serialEvent() function
    void resetInput();          //!< hard reset input status, after an inputCompleted

    //! tokenize input into 0..maxInputArgs
    /**
     * \param sep if NULL, blanks are used
     */
    unsigned splitInput(const char *sep = NULL);
    
    //! get last parsed type
    const char *operator[](const unsigned i) const;

    typedef void (*Callback)(const char *value);
    struct Parameter
    {
        const char *name;
        Callback    proc;
    };
    
    //! check completed, split default, and call proc if exists
    void processInput(const Parameter *parameters,
                      const unsigned   num_params);
    
#define MEDIUM_PARAM(NAME)       #NAME, on_##NAME
#define MEDIUM_PARAMETERS(TABLE) TABLE,sizeof(TABLE)/sizeof(TABLE[0])
    
    //! prepare an object for I/O
    Medium();
    
    //! cleanup thru destructor
    ~Medium() throw();

    //! repeating triangle (-T/2,0) -> (0,1) -> (T/2,0)
    static float Triangle(float t, const float T);
    
    //! triangle wave (-T/2,-1) -> (0,1) -> (T/2,1)
    static float TriangleWave(float t, const float T);
    
    //! sine wave
    static float SineWave(float t, const float T);
    
    //! CosWave
    static float CosWave(float t, const float T);
    
    
    //__________________________________________________________________________
    //
    // convert a 10-bits analog input into a 0..voltage value
    //__________________________________________________________________________
    class AnalogReader
    {
    public:
        const float conversion_factor; //!< voltage/1023.0f
        
        AnalogReader(const float voltage) throw();
        AnalogReader(const AnalogReader &) throw();
        virtual ~AnalogReader() throw();
        
        float operator()(const int PIN) const throw();
        
    private:
        MEDIUM_DISABLE_ASSIGN(AnalogReader);
    };
    
  private:
    char *    _input;                //!< allocated input buffer
    unsigned  _inputLength;          //!< current length
    bool      _inputCompleted;       //!< end of line, or max size reached
    char *    _words[maxInputWords]; //!< split words from input
    
    MEDIUM_DISABLE_COPY_AND_ASSIGN(Medium);
    
};

#endif
