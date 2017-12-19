#include "medium.h"

Medium::Medium() : _input(NULL),
                   _inputLength(0),
                   _inputCompleted(false),
                   _words(),
                   outputQ(),
                   outputP(),
                   nodes(NULL)
{
    _input = (char *)malloc(maxInputMemory);
    const unsigned long nodesMemory = maxOutputLength * (unsigned long)sizeof(CharNode);
    nodes = (CharNode *)malloc(nodesMemory);
    memset(nodes, 0, nodesMemory);

    for (unsigned i = 0; i < maxOutputLength; ++i)
    {
        CharNode *node = &nodes[i];
        outputP.store(node);
    }
    resetInput();
}

Medium::~Medium() throw()
{
    free(_input);
}

void Medium::resetInput()
{
    _inputCompleted = false;
    memset(_input, 0, maxInputMemory);
    _inputLength = 0;
    memset(_words, 0, sizeof(_words));
}

static bool isEOL(const char C)
{
    return (C == '\n' || C == '\r');
}

void Medium::serialEventCallback()
{
    while (Serial.available())
    {
        // get the new char
        const char ch = (char)Serial.read();
        if (!_inputCompleted)
        {
            // add it to the input string if available!
            _input[_inputLength++] = ch;

            // if the incoming character is a newline, set a flag so the main loop can
            // do something about it:
            if (ch == '\n' || _inputLength >= maxInputLength)
            {
                // clear the string
                while (_inputLength > 0 && isEOL(_input[_inputLength - 1]))
                {
                    _input[--_inputLength] = 0;
                }
                _inputCompleted = true;
            }
        }
    }
}

static const char __blanks[] = " \t";
unsigned Medium::findInputWords(const char *sep)
{
    // initialize loop
    unsigned count = 0;
    if (!sep)
        sep = __blanks;

    //  initiliaze token
    _words[0] = strtok(_input, sep);
    if (NULL != _words[0])
    {
        count = 1;
        while (count < maxInputWords)
        {
            _words[count] = strtok(NULL, sep);
            if (NULL == _words[count])
            {
                break;
            }
            ++count;
        }
    }
    return count;
}

const char *Medium::getInputWord(const unsigned i) const
{
    return _words[i % maxInputWords];
}

void Medium::print(const char *fmt, ...)
{
    char buff[maxOutputLength];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff) - 1, fmt, args);
    va_end(args);
    const unsigned len = strlen(buff);

    // make some room
    while (outputP.size < len)
    {
        if (Serial.availableForWrite())
        {
            CharNode *node = outputQ.pop_back();
            Serial.write(&(node->data), 1);
            outputP.store(node);
        }
    }

    // push data
    for (unsigned i = 0; i < len; ++i)
    {
        CharNode *node = outputP.query();
        node->data = buff[i];
        outputQ.push_front(node);
    }
}

void Medium::loopCallback()
{
    while (outputQ.size > 0)
    {
        if (!Serial.availableForWrite())
            break;
        CharNode *node = outputQ.pop_back();
        Serial.write( &(node->data), 1);
        outputP.store(node);
    }
}

float Medium::Triangle(float t, const float T)
{
    const float half = 0.5f * T;
    t = t - T * floorf((t / T) + 0.5f);
    if (t <= 0)
    {
        return (t + half) / half;
    }
    else
    {
        return (half - t) / half;
    }
}

float Medium::SineWave(float t, const float T)
{
    static const float two_pi = 2.0f * (float)M_PI;
    return sinf((two_pi * t) / T);
}