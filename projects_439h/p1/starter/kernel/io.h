#pragma once

template<typename T> class OutputStream {
public:
    virtual void put(T v) = 0;
};

template<typename T> class InputStream {
public:
    virtual T get() = 0;
};

