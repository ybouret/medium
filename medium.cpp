#include "medium.h"

Medium::Medium() : _input(NULL),
                   _inputLength(0),
                   _inputCompleted(false),
                   _words()
{
    _input = (char *)malloc(maxInputMemory);
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

void Medium::processSerialInput()
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
    return _words[i%maxInputWords];
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