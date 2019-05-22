#include "BigInt.h"

struct slot {
    int value;
    slot * next;
    slot * previous;
};

BigInt::BigInt() {
    constructPointers();
}

// Shared code among all constructors: make the pointers to the start and end of the list nullpointers and initialise the slotcounter.

void BigInt::constructPointers() {
    numberOfSlots = 0;
    start = nullptr;
    end = nullptr;
    isPositive = true;
}

// Copyconstructor

BigInt::BigInt(const BigInt & that) {
    constructPointers();
    copy(that);
}

// Constructor from int, reads the int into the BigInt.

BigInt::BigInt(int value) {
    constructPointers();
    if (value < 0) {
        isPositive = false;
        value *= -1;
    }

    if (value >= valuePerSlot) {
        put(value / valuePerSlot);
        value %= valuePerSlot;
    }
    put(value);
}

// Constructor from a char array, reads the array into the BigInt. Assumes only digits to be present in the array.

BigInt::BigInt(const char string[]) {
    constructPointers();

    int lengthOfString = 0;
    while (string[lengthOfString] != '\0') {
        lengthOfString++;
    }
    int value = 0;
    int index = 0;
    if (string[0] == '-') {
        isPositive = false;
        lengthOfString--;
        index++;
    }
    while (lengthOfString) {
        if (!(lengthOfString % digitsPerSlot)) {
            put(value);
            value = 0;
        }
        value = value * 10 + (string[index] - '0');
        lengthOfString--;
        index++;
    }
    put(value);
}

BigInt::~BigInt() {
    clear();
}

BigInt & BigInt::operator=(const BigInt & that) {
    if (this != &that) {
        copy(that);
    }
    return *this;
}

void BigInt::copy(const BigInt & that) {
    clear();
    isPositive = that.isPositive;
    slot * currentSlot = that.start;
    while (currentSlot != nullptr) {
        put(currentSlot->value);
        currentSlot = currentSlot->next;
    }
}

// Depending on the signs of LHS and RHS, either addition or subtraction is required.

BigInt & BigInt::operator+=(const BigInt & that) {
    if (isPositive && that.isPositive) {
        add(that);
    }
    else if (isPositive && !that.isPositive) {
        BigInt placeholder(that);
        placeholder.isPositive = true;
        subtract(placeholder);
    } else if (!isPositive && that.isPositive) {
        BigInt placeholder(that);
        placeholder.subtract(*this);
        copy(placeholder);
    } else {
        add(that);
        isPositive = false;
    }

    return *this;
}

// Addition. Adds slots together, remembers any carry (either no carry (0) or a carry (1)) and adds those to the next slots.
// Keeps going until all slots of both BigInts and the carry are empty.

void BigInt::add(const BigInt & that) {
    BigInt placeholder(*this);
    slot * currentSlotThat = that.end;
    slot * currentSlotThis = placeholder.end;

    clear();
    bool carry = false;
    while (currentSlotThis != nullptr || currentSlotThat != nullptr || carry) {
        int thisValue = 0;
        int thatValue = 0;
        if (currentSlotThis != nullptr) {
            thisValue = currentSlotThis->value;
            currentSlotThis = currentSlotThis->previous;
        }
        if (currentSlotThat != nullptr) {
            thatValue = currentSlotThat->value;
            currentSlotThat = currentSlotThat->previous;
        }
        int sum = thisValue + thatValue + carry;
        carry = sum >= valuePerSlot;
        push(sum % (valuePerSlot));
    }
}

void BigInt::subtract(const BigInt & that) {
    // Check in advance whether the subtraction will cause the sign of this to flip, by checking if RHS > LHS
    // If that's the case, replace LHS - RHS with (RHS - LHS) * -1
    if (that > * this) {
        BigInt placeholder(that);
        placeholder.subtract(*this);
        copy(placeholder);
        isPositive = false;
    }
    else {
        BigInt placeholder(*this);
        slot * currentSlotThat = that.end;
        slot * currentSlotThis = placeholder.end;

        clear();

        bool carry = false;
        while (currentSlotThis != nullptr || currentSlotThat != nullptr) {
            int thisValue = 0;
            int thatValue = 0;
            if (currentSlotThis != nullptr) {
                thisValue = currentSlotThis->value;
                currentSlotThis = currentSlotThis->previous;
            }
            if (currentSlotThat != nullptr) {
                thatValue = currentSlotThat->value;
                currentSlotThat = currentSlotThat->previous;
            }
            int diff = thisValue - carry - thatValue;
            if (diff < 0) {
                carry = true;
                diff = -1 * diff;
            } else {
                carry = false;
            }
            push(diff);
        }
        removeLeadingZeros();
    }
}

// Substraction can lead to leading zero valued slots. These need to be removed asap, since this breaks comparison.

void BigInt::removeLeadingZeros() {
    slot * currentSlot = start;
    slot * helper;

    while (currentSlot->next != nullptr && currentSlot->value == 0) {
        helper = currentSlot;
        currentSlot = currentSlot->next;
        delete helper;
    }
    start = currentSlot;
    currentSlot->previous = nullptr;
}

BigInt BigInt::operator+(const BigInt & that) const {
    return BigInt(*this) += that;
}

// subtraction operators are defined as LHS - RHS = LHS + (-1 * RHS)

BigInt & BigInt::operator-=(const BigInt & that) {
    BigInt placeholder(that);
    placeholder.isPositive = !placeholder.isPositive;
    return *this += placeholder;
}

BigInt BigInt::operator-(const BigInt & that) const {
    return BigInt(*this) -= that;
}

// Cross-multiplication of all slots, with adding the sum of the slotcount worth of zero-slots afterwards. Accumulating all the results.

BigInt & BigInt::operator*=(const BigInt & that) {
    BigInt placeholder(*this);
    slot * currentSlotThis = placeholder.end;

    BigInt prodPlaceholder;

    clear();

    int thisSlotCounter = 0;
    while (currentSlotThis != nullptr) {
        int thatSlotCounter = 0;
        slot * currentSlotThat = that.end;
        while (currentSlotThat != nullptr) {
            prodPlaceholder.clear();
            long long prod = (long long) currentSlotThis->value * (long long) currentSlotThat->value;
            if (prod >= valuePerSlot) {
                int overflow = (int) (prod / valuePerSlot);
                prodPlaceholder.put(overflow);
                prod %= valuePerSlot;
            }
            prodPlaceholder.put((int) prod);
            for (int numberOfZeroSlots = 0; numberOfZeroSlots < thisSlotCounter + thatSlotCounter; numberOfZeroSlots++) {
                prodPlaceholder.put(0);
            }
            *this += prodPlaceholder;
            thatSlotCounter++;
            currentSlotThat = currentSlotThat->previous;
        }
        thisSlotCounter++;
        currentSlotThis = currentSlotThis->previous;
    }
    isPositive = !(placeholder.isPositive ^ that.isPositive);

    return *this;
}

BigInt BigInt::operator*(const BigInt & that) const {
    return BigInt(*this) *= that;
}

// Equality check, first check for same amount of slots. If that differs, numbers can't be equal. Second checks for signs, then checks if all slots themselves have equal value.

bool BigInt::operator==(const BigInt & that) const {
    if (this->numberOfSlots != that.numberOfSlots) {
        return false;
    }
    if (isPositive != that.isPositive) {
        return false;
    }
    slot * currentSlotThis = end;
    slot * currentSlotThat = that.end;

    while (currentSlotThis != nullptr) {
        if (currentSlotThis->value != currentSlotThat->value) {
            return false;
        }
        currentSlotThat = currentSlotThat->previous;
        currentSlotThis = currentSlotThis->previous;
    }
    return true;
}

bool BigInt::operator!=(const BigInt & that) const {

    return !(*this == that);
}

// Relational operator. First compares the number of slots, to see if there is already an answer, same for signs. Else it starts at the head of the list to test if the values are equal. 
// When they are not equal anymore, they can be compared for a result.

bool BigInt::operator<(const BigInt & that) const {
    if (this->numberOfSlots != that.numberOfSlots || this->isPositive != that.isPositive) {
        return this->numberOfSlots * (this->isPositive - 0.5) < that.numberOfSlots * (that.isPositive - 0.5);
    }

    slot * currentSlotThis = start;
    slot * currentSlotThat = that.start;
    while (currentSlotThis->next != nullptr && currentSlotThis->value == currentSlotThat->value) {
        currentSlotThat = currentSlotThat->next;
        currentSlotThis = currentSlotThis->next;
    }
    return currentSlotThis->value < currentSlotThat->value;
}

bool BigInt::operator<=(const BigInt & that) const {
    return !(that < *this);
}

bool BigInt::operator>(const BigInt & that) const {
    return that < *this;
}

bool BigInt::operator>=(const BigInt & that) const {
    return !(*this < that);
}

BigInt & BigInt::operator++() {
    return *this += BigInt(1);
}

BigInt BigInt::operator++(int) {
    BigInt temp(*this);
    operator++();
    return temp;
}

BigInt::operator bool() const {
    return *this != 0;
}

bool BigInt::operator!() const {
    return !bool(*this);
}

// Remove and delete all slots from an object.

void BigInt::clear() {
    slot * currentSlot = start;
    slot * placeholder;
    while (currentSlot != nullptr) {
        placeholder = currentSlot;
        currentSlot = placeholder->next;
        delete placeholder;
    }
    start = nullptr;
    end = nullptr;
    numberOfSlots = 0;
}

// Put a slot at the end of the list.

void BigInt::put(int value) {
    slot * newslot = new slot;
    newslot->value = value;
    newslot->next = nullptr;
    slot * endOfList = end;
    if (numberOfSlots) {
        endOfList->next = newslot;
    } else {
        start = newslot;
    }
    end = newslot;
    newslot->previous = endOfList;
    numberOfSlots++;
}

// Push a slot to the start of the list.

void BigInt::push(int value) {
    slot * newslot = new slot;
    newslot->value = value;
    newslot->next = start;
    slot * startOfList = start;
    if (numberOfSlots) {
        startOfList->previous = newslot;
    } else {
        end = newslot;
    }
    start = newslot;
    newslot->previous = nullptr;
    numberOfSlots++;
}

std::ostream & operator<<(std::ostream & os, const BigInt & obj) {
    if (!obj.isPositive) {
        os.put('-');
    }

    slot * currentSlot = obj.start;
    while (currentSlot != nullptr) {
        int value = currentSlot->value;
        int fullValue = value;
        int numberOfPaddingZeros = 1;
        int digit = 10000000;
        if (currentSlot == obj.start && (fullValue == 0)) {
            os.put('0');
        } else {
            while (digit) {
                // Pad the value in the slot with leading zeros to fill the entire slot, except when it is the first slot.
                if (currentSlot != obj.start || (fullValue >= digit)) {
                    os.put((value / digit) + '0');
                }

                value %= digit;
                digit /= 10;
            }
        }
        currentSlot = currentSlot->next;
    }
    return os;
}
