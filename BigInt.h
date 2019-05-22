#include <iostream>
#pragma once

struct slot;

class BigInt {
    static const int digitsPerSlot = 8;
    static const int valuePerSlot = 100000000;
public:
    BigInt();
    BigInt(const BigInt & that);
    BigInt(int value);
    BigInt(const char string[]);
    ~BigInt();

    bool isPositive;

    BigInt & operator=(const BigInt & that);

    BigInt & operator+=(const BigInt & that);
    BigInt operator+(const BigInt & that) const;

    BigInt & operator-=(const BigInt & that);
    BigInt operator-(const BigInt & that) const;

    BigInt & operator*=(const BigInt & that);
    BigInt operator*(const BigInt & that) const;

    bool operator==(const BigInt & that) const;
    bool operator!=(const BigInt & that) const;

    bool operator<(const BigInt & that) const;
    bool operator<=(const BigInt & that) const;
    bool operator>(const BigInt & that) const;
    bool operator>=(const BigInt & that) const;

    BigInt & operator++();
    BigInt operator++(int);

    explicit operator bool() const;
    bool operator!() const;

    friend std::ostream & operator<<(std::ostream & os, const BigInt & obj);
private:
    void copy(const BigInt & that);
    void constructPointers();

    slot * start;
    slot * end;
    int numberOfSlots;

    void clear();
    void put(int value);
    void push(int value);

    void add(const BigInt & that);
    void subtract(const BigInt & that);

    void removeLeadingZeros();
};
