#include "medium.h"

Medium::Medium() : _input(NULL),
                   _inputLength(0),
                   _inputCompleted(false),
                   _words(),
                   _output(NULL),
                   _outputLength(0),
                   _outputCurrent(0),
                   _outputIsActive(false)
{
    _input = (char *)malloc(maxInputMemory);
    _output = (char *)malloc(maxOutputMemory);
    resetInput();
    resetOutput();
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

void Medium::resetOutput()
{
    memset(_output, 0, maxOutputMemory);
    _outputIsActive = false;
    _outputLength = 0;
    _outputCurrent = 0;
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

void Medium::processSerialOutput()
{
    if (_outputIsActive)
    {
        while (Serial.availableForWrite())
        {
            const unsigned toWrite = _outputLength - _outputCurrent;
            const unsigned written = Serial.write(_output+_outputCurrent,toWrite);
            _outputCurrent += written;
            if(_outputCurrent>=_outputLength)
            {
                resetOutput();
                break;
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
    va_list args;
    va_start(args, fmt);
    vsnprintf(_output, maxOutputLength, fmt, args);
    va_end(args);
    _outputCurrent = 0;
    _outputLength = strlen(_output);
    _outputIsActive = (_outputLength>0);
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