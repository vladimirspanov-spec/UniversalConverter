#include "mainpage.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>
#include <string>
#include <algorithm>

class BigInt {
public:
    BigInt(int n = 0);
    BigInt(std::string n);
    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;
    BigInt operator+(const BigInt& other);
    BigInt operator-(const BigInt& other);
    BigInt operator*(const BigInt& other);
    std::string toString();
    bool isZero();

private:
    std::vector<int> integer;
    static const int BASE = 1000000000;
    void normalize() {
        while (integer.size() > 1 && integer.back() == 0) {
            integer.pop_back();
        }
    }

};

BigInt::BigInt(int n) {
    if (n == 0) integer.push_back(0);
    while (n > 0) {
        integer.push_back(n % BASE);
        n /= BASE;
    }
}

bool BigInt::operator==(const BigInt& other) const {
    return integer == other.integer;
}

bool BigInt::operator!=(const BigInt& other) const {
    return !(integer == other.integer);
}

bool BigInt::operator<(const BigInt& other) const {
    if (integer.size() != other.integer.size()) {
        return integer.size() < other.integer.size();
    }

    for (int i = integer.size() - 1; i >= 0; --i) {
        if (integer[i] < other.integer[i]) return true;
        else if (integer[i] > other.integer[i]) return false;
    }
    return false;
}

bool BigInt::operator<=(const BigInt& other) const {
    return *this == other || *this < other;
}

bool BigInt::operator>(const BigInt& other) const {
    return !(*this <= other);
}

bool BigInt::operator>=(const BigInt& other) const {
    return !(*this < other);
}

BigInt BigInt::operator+(const BigInt& other) {
    int carry = 0;
    BigInt ans;
    for (size_t i = 0; i < std::max(integer.size(), other.integer.size()) || carry; ++i) {
        if (i == integer.size()) integer.push_back(0);
        long long c = integer[i] + carry + (i < other.integer.size() ? other.integer[i] : 0);
        carry = c / BASE;
        c %= BASE;
        ans.integer.push_back(c);
    }
    return ans;
}

BigInt BigInt::operator-(const BigInt& other) {
    if (other > *this) return BigInt(0);
    int carry = 0;
    BigInt ans;
    for (size_t i = 0; i < other.integer.size() || carry; ++i) {
        long long c = integer[i] - carry - (i < other.integer.size() ? other.integer[i] : 0);
        carry = 0;
        if (c < 0) {
            c += BASE;
            carry = 1;
        }
        ans.integer.push_back(c);
    }
    return ans;
}

BigInt BigInt::operator*(const BigInt& other) {
    BigInt ans;
    ans.integer.resize(integer.size() + other.integer.size(), 0);
    for (size_t i = 0; i < integer.size(); ++i) {
        long long carry = 0;
        for (size_t j = 0; j < other.integer.size(); ++j) {
            long long c = ans.integer[i + j] + carry + (long long)integer[i] * (j < other.integer.size() ? other.integer[j] : 0);
            ans.integer[i + j] = c & BASE;
            carry = c / BASE;
        }
    }
    ans.normalize();
    return ans;
}

MainPage::MainPage(QWidget *parent)
    : QWidget(parent)
{
    this->setFixedSize(500, 500);
    value = new QTextEdit;
    p = new QLineEdit;
    q = new QLineEdit;
    p->setFixedSize(50, 50); q->setFixedSize(50, 50);
    value->setFixedSize(150, 75);
    answer = new QTextEdit;
    answer->setFixedSize(200, 200);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *inputLayput = new QHBoxLayout;
    inputLayput->addWidget(value);
    inputLayput->addWidget(p);
    inputLayput->addWidget(q);
    mainLayout->addLayout(inputLayput);
    mainLayout->addWidget(answer);
    setLayout(mainLayout);
}

MainPage::~MainPage() {}
