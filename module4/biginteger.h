//
// Created by Артем Чумаченко on 2018-12-04.
//

#ifndef BIGINTEGER_BIGINTEGER_H
#define BIGINTEGER_BIGINTEGER_H

#include <vector>

class BigInteger{
public:
    BigInteger();
    ~BigInteger() = default;
    BigInteger(const BigInteger&) = default;
    BigInteger(BigInteger&&) noexcept = default;
    BigInteger& operator=(const BigInteger& bigInt);
    BigInteger& operator=(BigInteger&& bigInt) noexcept;

    BigInteger(const long long& number);
    explicit BigInteger(const std::string& str);

    friend BigInteger operator+(const BigInteger& a, const BigInteger& b);
    friend BigInteger operator-(const BigInteger& a, const BigInteger& b);
    friend BigInteger operator*(const BigInteger& a, const BigInteger& b);
    friend BigInteger operator/(const BigInteger& a, const BigInteger& b);
    friend BigInteger operator%(const BigInteger& a, const BigInteger& b);

    const BigInteger operator-() const;
    BigInteger& operator--();
    const BigInteger operator--(int);
    BigInteger& operator++();
    const BigInteger operator++(int);

    BigInteger& operator+=(const BigInteger& b);
    BigInteger& operator-=(const BigInteger& b);
    BigInteger& operator*=(const BigInteger& b);
    BigInteger& operator/=(const BigInteger& b);
    BigInteger& operator%=(const BigInteger& b);

    friend bool operator==(const BigInteger& a, const BigInteger& b);
    friend bool operator!=(const BigInteger& a, const BigInteger& b);
    friend bool operator<(const BigInteger& a, const BigInteger& b);
    friend bool operator<=(const BigInteger& a, const BigInteger& b);
    friend bool operator>(const BigInteger& a, const BigInteger& b);
    friend bool operator>=(const BigInteger& a, const BigInteger& b);

    friend std::ostream& operator<<(std::ostream& output, const BigInteger& a);
    friend std::istream& operator>>(std::istream& input, BigInteger& a);
    std::string toString() const;

    explicit operator bool() const;

    long long operator[](size_t it) const;
    size_t size() const;

    BigInteger abs() const;

private:
    std::vector<long long> data;
    const static long long kFactor = 1'000'000;
    const static long long kLogFactor = 6;
    bool isNegative = false;

    void m_deleteZeros();
    void m_toNormal();
};


BigInteger::BigInteger() {
    data.push_back(0);
}

BigInteger& BigInteger::operator=(const BigInteger& bigInt) {
    data = bigInt.data;
    isNegative = bigInt.isNegative;
    return *this;
}

BigInteger& BigInteger::operator=(BigInteger&& bigInt) noexcept {
    data = std::move(bigInt.data);
    isNegative = bigInt.isNegative;
    return *this;
}

BigInteger::BigInteger(const long long& number) {
    data.push_back(number);
    m_toNormal();
}

BigInteger::BigInteger(const std::string& str) {
    if(str.empty()) {
        data.push_back(0);
    }
    else if((str.size() == 1) && (str[0] == '-')) {
        data.push_back(0);
    }
    else {
        long long beginPosition = 0;
        if (str[0] == '-') {
            isNegative = true;
            beginPosition = 1;
        }

        for (ssize_t i = str.size(); (i - kLogFactor) >= beginPosition; i -= kLogFactor) {
            std::string temp;
            for (short j = kLogFactor; j > 0; --j) {
                temp += str[i - j];
            }
            data.push_back(std::stoll(temp));
        }

        if ((str.size() - beginPosition) % kLogFactor != 0) {
            std::string temp;
            for (size_t j = 0; j < (str.size() - beginPosition) % kLogFactor; ++j) {
                temp += str[j + beginPosition];
            }
            data.push_back(std::stoll(temp));
        }

        m_toNormal();
    }
}

BigInteger operator+(const BigInteger& a, const BigInteger& b) {
    bool aBigger = a.size() > b.size();
    BigInteger c;
    c.data.assign(aBigger ? a.size() : b.size(), 0);
    for(size_t i = 0; i < (aBigger ? a.size() : b.size()); ++i) {
        c.data[i] = a[i] * (a.isNegative ? -1 : 1) + b[i] * (b.isNegative ? -1 : 1);
    }
    c.m_toNormal();
    return c;
}

BigInteger operator-(const BigInteger& a, const BigInteger& b) {
    bool aBigger = a.size() > b.size();
    BigInteger c;
    c.data.assign(aBigger ? a.size() : b.size(), 0);
    for(size_t i = 0; i < (aBigger ? a.size() : b.size()); ++i) {
        c.data[i] = a[i] * (a.isNegative ? -1 : 1) - b[i] * (b.isNegative ? -1 : 1);
    }
    c.m_toNormal();
    return c;
}

BigInteger operator*(const BigInteger& a, const BigInteger& b) {
    size_t maxSize = a.size() > b.size() ? a.size() : b.size();
    BigInteger c;
    if(maxSize == 1) {
        c.data[0] = a[0] * b[0];
    }
    else if(b.size() == 1) {
        c = a;
        for(auto& d: c.data) {
            d *= b[0];
        }
    }
    else if(a.size() == 1) {
        c = b;
        for(auto& d: c.data) {
            d *= a[0];
        }
    }
    else {
        c.data.assign(a.size() + b.size() + 1, 0);
        for(size_t i = 0; i < a.size(); ++i) {
            for(size_t j = 0; j < b.size(); ++j) {
                c.data[i + j + 1] += (a[i] * b[j] + c.data[i + j]) / BigInteger::kFactor;
                c.data[i + j] = (a[i] * b[j] + c.data[i + j]) % BigInteger::kFactor;
            }
            for(size_t j = i + b.size(); j < a.size() + b.size(); ++j) {
                c.data[j + 1] += c.data[j] / BigInteger::kFactor;
                c.data[j] = c.data[j] % BigInteger::kFactor;
            }
        }
    }

    c.isNegative = (a.isNegative != b.isNegative);
    c.m_toNormal();

    return c;
}

BigInteger operator/(const BigInteger& a, const BigInteger& b) {
    BigInteger c;
    if(a.size() < b.size()) {
        return c;
    }

    if(a.size() == 1) {
        c.data[0] = a[0] / b[0];
    }
    else if(b.size() == 1) {
        c.data.assign(a.size(), 0);
        long long r = 0;
        for(size_t i = a.size(); i > 0; --i) {
            long long temp = (a[i - 1] + r * BigInteger::kFactor) / b[0];
            r = (a[i - 1] + r * BigInteger::kFactor) % b[0];
            c.data[i - 1] = temp;
        }
    }
    else {
        BigInteger b_hat = b.abs();
        BigInteger simA;
        simA.data.clear();
        c.data.assign(a.size(), 0);
        for(size_t i = a.size(); i  > 0; --i) {
            simA.data.insert(simA.data.begin(), a[i - 1]);

            if(simA >= b_hat) {
                long long q_hat = 0, left = 0, right = BigInteger::kFactor;
                while(left <= right) {
                    long long m = (left + right) / 2;
                    BigInteger temp = b_hat * m;
                    if(temp <= simA) {
                        q_hat = m;
                        left = m + 1;
                    }
                    else {
                        right = m - 1;
                    }
                }
                c.data[i - 1] = q_hat;
                simA -= b_hat * q_hat;
            }
        }
    }

    c.isNegative = (a.isNegative != b.isNegative);
    c.m_toNormal();

    return c;
}

BigInteger operator%(const BigInteger& a, const BigInteger& b) {
    return a - (a / b) * b;
}

const BigInteger BigInteger::operator-() const {
    BigInteger copy = *this;
    copy.isNegative = !isNegative;
    copy.m_toNormal();
    return copy;
}

BigInteger& BigInteger::operator--() {
    if(isNegative) {
        data[0]++;
    }
    else {
        data[0]--;
    }
    m_toNormal();
    return *this;
}

const BigInteger BigInteger::operator--(int) {
    BigInteger old = *this;
    if(isNegative) {
        data[0]++;
    }
    else {
        data[0]--;
    }
    m_toNormal();
    return old;
}

BigInteger& BigInteger::operator++() {
    if(isNegative) {
        data[0]--;
    }
    else {
        data[0]++;
    }
    m_toNormal();
    return *this;
}

const BigInteger BigInteger::operator++(int) {
    BigInteger old = *this;
    if(isNegative) {
        data[0]--;
    }
    else {
        data[0]++;
    }
    m_toNormal();
    return old;
}

BigInteger& BigInteger::operator+=(const BigInteger& b) {
    return (*this = *this + b);
}

BigInteger& BigInteger::operator-=(const BigInteger& b) {
    return (*this = *this - b);
}

BigInteger& BigInteger::operator*=(const BigInteger& b) {
    return (*this = *this * b);
}

BigInteger& BigInteger::operator/=(const BigInteger& b) {
    return (*this = *this / b);
}

BigInteger& BigInteger::operator%=(const BigInteger& b) {
    return (*this = *this % b);
}

bool operator==(const BigInteger& a, const BigInteger& b) {
    return (a.isNegative == b.isNegative) && (a.data == b.data);
}

bool operator!=(const BigInteger& a, const BigInteger& b) {
    return !(a == b);
}

bool operator<(const BigInteger& a, const BigInteger& b) {
    return (a - b).isNegative && (a != b);
}

bool operator<=(const BigInteger& a, const BigInteger& b) {
    return (a == b) || (a - b).isNegative;
}

bool operator>(const BigInteger& a, const BigInteger& b) {
    return (b - a).isNegative && (a != b);
}

bool operator>=(const BigInteger& a, const BigInteger& b) {
    return (a == b) || (b - a).isNegative;
}

std::ostream& operator<<(std::ostream& output, const BigInteger& a) {
    output << a.toString();
    return output;
}

std::istream& operator>>(std::istream& input, BigInteger& a) {
    std::string temp;
    input >> temp;
    a = BigInteger(temp);

    return input;
}

std::string BigInteger::toString() const {
    std::string res;

    if(isNegative) {
        res += '-';
    }

    for(size_t i = data.size(); i > 0; --i) {
        std::string temp = std::to_string(data[i - 1]);
        if(i < data.size()) {
            while(temp.size() < kLogFactor) {
                temp.insert(temp.begin(), '0');
            }
        }
        res += temp;
    }

    return res;
}

BigInteger::operator bool() const {
    return !((data.size() == 1) && (data[0] == 0));
}

long long BigInteger::operator[](size_t it) const {
    if(it < data.size()) {
        return data[it];
    }
    return 0;
}

size_t BigInteger::size() const {
    return data.size();
}

BigInteger BigInteger::abs() const {
    BigInteger a;
    a.data = data;
    a.isNegative = false;
    return a;
}

void BigInteger::m_deleteZeros() {
    size_t it = data.size();
    while(data[it - 1] == 0) {
        data.pop_back();
        --it;
        if(it == 0) {
            data.push_back(0);
            isNegative = false;
            break;
        }
    }
}

void BigInteger::m_toNormal() {
    m_deleteZeros();
    if(data.size() == 1) {
        if(data[0] == 0) {
            isNegative = false;
            return;
        }
    }

    // Prepare data to check sign
    for(size_t i = 0; i < data.size() - 1; ++i) {
        if(data[i] >= kFactor) {
            data[i + 1] += data[i] / kFactor;
            data[i] %= kFactor;
        }
        else if(data[i] < 0) {
            --data[i + 1];
            data[i] += kFactor;
        }
    }

    // If sign of last 'digit' is negative, number has the opposite sign.
    if(data.back() < 0) {
        data.back() = -data.back();
        if(data.size() > 1) {
            data.back()--;
            for(size_t i = data.size() - 1; i > 1; --i) {
                data[i - 1] = kFactor - data[i - 1] - 1;
            }
            data[0] = kFactor - data[0];
        }
        isNegative = !isNegative;
    }

    for(size_t i = 0; i < data.size(); ++i) {
        if(data[i] >= kFactor) {
            if(i + 1 >= data.size()) {
                data.push_back(data[i] / kFactor);
            }
            else {
                data[i + 1] += data[i] / kFactor;
            }
            data[i] %= kFactor;
        }
    }

    m_deleteZeros();
}

#endif //BIGINTEGER_BIGINTEGER_H
