#include "mainpage.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <cmath>


class BigInt {
public:
    BigInt(long long n = 0);
    BigInt(const std::string& s);

    bool operator==(const BigInt& o) const;
    bool operator!=(const BigInt& o) const { return !(*this == o); }
    bool operator<(const BigInt& o)  const;
    bool operator<=(const BigInt& o) const { return !(o < *this); }
    bool operator>(const BigInt& o)  const { return o < *this; }
    bool operator>=(const BigInt& o) const { return !(*this < o); }

    BigInt operator+(const BigInt& o) const;
    BigInt operator-(const BigInt& o) const;
    BigInt operator*(const BigInt& o) const;
    BigInt operator*(long long k)     const;
    BigInt operator&(const BigInt& o) const;

    std::pair<BigInt,BigInt> divmod(const BigInt& d) const;
    BigInt operator/(const BigInt& d) const { return divmod(d).first; }
    BigInt operator%(const BigInt& d) const { return divmod(d).second; }

    bool isZero() const;
    std::string toString() const;
    static BigInt gcd(BigInt a, BigInt b);

private:
    std::vector<long long> d_;
    static const long long BASE = 1000000000LL;
    void trim() { while (d_.size() > 1 && d_.back() == 0) d_.pop_back(); }
    BigInt shift(int k) {
        for (int i = 0; i < k; ++i) {
            d_.insert(d_.begin(), 0);
        }
        return *this;
    }
};

BigInt::BigInt(long long n) {
    if (n == 0) { d_.push_back(0); return; }
    while (n > 0) { d_.push_back(n % BASE); n /= BASE; }
}

BigInt::BigInt(const std::string& s) {
    d_.clear();
    int i = (int)s.size();
    while (i > 0) {
        int from = std::max(0, i - 9);
        d_.push_back(std::stoll(s.substr(from, i - from)));
        i = from;
    }
    trim();
}

bool BigInt::isZero() const { return d_.size() == 1 && d_[0] == 0; }

bool BigInt::operator==(const BigInt& o) const { return d_ == o.d_; }

bool BigInt::operator<(const BigInt& o) const {
    if (d_.size() != o.d_.size()) return d_.size() < o.d_.size();
    for (int i = (int)d_.size()-1; i >= 0; --i)
        if (d_[i] != o.d_[i]) return d_[i] < o.d_[i];
    return false;
}

BigInt BigInt::operator+(const BigInt& o) const {
    BigInt res; res.d_.clear();
    long long carry = 0;
    for (size_t i = 0; i < std::max(d_.size(), o.d_.size()) || carry; ++i) {
        long long s = carry;
        if (i < d_.size())   s += d_[i];
        if (i < o.d_.size()) s += o.d_[i];
        res.d_.push_back(s % BASE);
        carry = s / BASE;
    }
    return res;
}

BigInt BigInt::operator-(const BigInt& o) const {
    BigInt res; res.d_.clear();
    long long borrow = 0;
    for (size_t i = 0; i < d_.size(); ++i) {
        long long s = d_[i] - borrow - (i < o.d_.size() ? o.d_[i] : 0);
        borrow = 0;
        if (s < 0) { s += BASE; borrow = 1; }
        res.d_.push_back(s);
    }
    res.trim();
    return res;
}


BigInt BigInt::operator&(const BigInt& o) const {
    BigInt res; res.d_.assign(d_.size() + o.d_.size(), 0);
    for (size_t i = 0; i < d_.size(); ++i) {
        long long carry = 0;
        for (size_t j = 0; j < o.d_.size() || carry; ++j) {
            __int128 cur = (__int128)res.d_[i + j] + carry;
            if (j < o.d_.size()) cur += (__int128)d_[i] * o.d_[j];
            res.d_[i + j] = (long long)(cur % BASE);
            carry = (long long)(cur / BASE);
        }
    }
    res.trim();
    return res;
}

BigInt BigInt::operator*(const BigInt& o) const{
    if (this->d_.size() <= 1 || o.d_.size() <= 1) {
        return *this & o;
    }
    int k = std::min(this->d_.size()/2, o.d_.size()/2);
    BigInt a1; a1.d_ = std::vector<long long>(this->d_.begin(), this->d_.begin() + k);
    BigInt a2; a2.d_ = std::vector<long long>(this->d_.begin() + k, this->d_.end());
    BigInt b1; b1.d_ = std::vector<long long>(o.d_.begin(), o.d_.begin() + k);
    BigInt b2; b2.d_ = std::vector<long long>(o.d_.begin() + k, o.d_.end());
    BigInt res = a1 * b1 + (a1 * b2 + a2 * b1).shift(k) + (a2 * b2).shift(2 * k);
    return res;
}

BigInt BigInt::operator*(long long k) const {
    if (k == 0) return BigInt(0);
    BigInt res; res.d_.clear();
    long long carry = 0;
    for (size_t i = 0; i < d_.size() || carry; ++i) {
        long long cur = carry + (i < d_.size() ? d_[i] * k : 0LL);
        res.d_.push_back(cur % BASE);
        carry = cur / BASE;
    }
    res.trim();
    return res;
}

std::pair<BigInt,BigInt> BigInt::divmod(const BigInt& dv) const {
    if (dv.isZero() || *this < dv) return {BigInt(0), *this};
    BigInt q, r;
    q.d_.resize(d_.size(), 0);
    for (int i = (int)d_.size()-1; i >= 0; --i) {
        r.d_.insert(r.d_.begin(), d_[i]);
        r.trim();
        long long lo = 0, hi = BASE - 1;
        while (lo < hi) {
            long long mid = lo + (hi - lo + 1) / 2;
            if (dv * mid <= r) lo = mid; else hi = mid - 1;
        }
        q.d_[i] = lo;
        r = r - dv * lo;
    }
    q.trim();
    return {q, r};
}

std::string BigInt::toString() const {
    std::string s = std::to_string(d_.back());
    for (int i = (int)d_.size()-2; i >= 0; --i) {
        std::string part = std::to_string(d_[i]);
        s += std::string(9 - part.size(), '0') + part;
    }
    return s;
}

BigInt BigInt::gcd(BigInt a, BigInt b) {
    while (!b.isZero()) { BigInt t = a % b; a = b; b = t; }
    return a;
}

// ============================================================


class BigFraction {
public:
    BigFraction(BigInt num = BigInt(0), BigInt den = BigInt(1));

    static BigFraction fromDigits(
        const std::vector<long long>& intDigits,
        const std::vector<long long>& fracDigits,
        int periodStart,   // -1 = нет периода
        long long base);

    std::string toBase(long long q, size_t maxLen) const;
    bool isZero() const { return num_.isZero(); }

private:
    BigInt num_, den_;
    void reduce();
    static std::string digitToStr(long long v);
};

BigFraction::BigFraction(BigInt num, BigInt den) : num_(num), den_(den) { reduce(); }

void BigFraction::reduce() {
    BigInt g = BigInt::gcd(num_, den_);
    if (!g.isZero() && g != BigInt(1)) { num_ = num_ / g; den_ = den_ / g; }
}

BigFraction BigFraction::fromDigits(
    const std::vector<long long>& intDigits,
    const std::vector<long long>& fracDigits,
    int periodStart,
    long long base)
{
    BigInt B(base);

    BigInt intVal(0);
    for (long long dv : intDigits) intVal = intVal * B + BigInt(dv);

    if (fracDigits.empty()) return BigFraction(intVal, BigInt(1));

    int preLen    = (periodStart < 0) ? (int)fracDigits.size() : periodStart;
    int periodLen = (periodStart < 0) ? 0 : (int)fracDigits.size() - periodStart;

    BigInt preValue(0);
    for (int i = 0; i < preLen; ++i)
        preValue = preValue * B + BigInt(fracDigits[i]);

    BigInt Bpre(1);
    for (int i = 0; i < preLen; ++i) Bpre = Bpre * B;

    if (periodLen == 0) {
        return BigFraction(intVal * Bpre + preValue, Bpre);
    }

    BigInt periodValue(0);
    for (int i = periodStart; i < (int)fracDigits.size(); ++i)
        periodValue = periodValue * B + BigInt(fracDigits[i]);

    // Bper = base^periodLen
    BigInt Bper(1);
    for (int i = 0; i < periodLen; ++i) Bper = Bper * B;

    // Формула: x = intVal + preValue/Bpre + periodValue / (Bpre * (Bper - 1))
    BigInt Bper1 = Bper - BigInt(1);
    BigInt denom = Bpre * Bper1;
    BigInt numer = intVal * denom + preValue * Bper1 + periodValue;
    return BigFraction(numer, denom);
}

std::string BigFraction::digitToStr(long long v) {
    if (v < 10) return std::string(1, '0' + (char)v);
    if (v < 36) return std::string(1, 'A' + (char)(v - 10));
    return "[" + std::to_string(v) + "]";
}

std::string BigFraction::toBase(long long q, size_t maxLen) const {
    BigInt Q(q);
    auto [intQ, rem] = num_.divmod(den_);

    // Целая часть → строка
    std::string intStr;
    if (intQ.isZero()) {
        intStr = "0";
    } else {
        BigInt tmp = intQ;
        while (!tmp.isZero()) {
            auto [qt, rm] = tmp.divmod(Q);
            intStr = digitToStr(std::stoll(rm.toString())) + intStr;
            tmp = qt;
        }
    }

    std::string result = intStr;
    if (rem.isZero()) return result;

    result += ".";
    size_t len = result.size();

    std::map<std::string, size_t> seen;
    std::vector<std::string> fracParts;
    bool truncated = false;

    while (!rem.isZero()) {
        std::string key = rem.toString();
        if (seen.count(key)) {
            size_t pos = seen[key];
            std::string pre, per;
            for (size_t i = 0;   i < pos;             ++i) pre += fracParts[i];
            for (size_t i = pos; i < fracParts.size(); ++i) per += fracParts[i];
            result += pre + "(" + per + ")";
            return result;
        }
        seen[key] = fracParts.size();

        rem = rem * Q;
        auto [dq, dr] = rem.divmod(den_);
        std::string ds = digitToStr(std::stoll(dq.toString()));
        len += ds.size();
        if (len > maxLen) { truncated = true; break; }
        fracParts.push_back(ds);
        rem = dr;
    }

    for (auto& s : fracParts) result += s;
    if (truncated) result += "...";
    return result;
}

// ============================================================


static std::vector<long long> parseDigits(const std::string& s, long long base,
                                          size_t& pos, bool stopAtDot, bool stopAtParen)
{
    std::vector<long long> digits;
    while (pos < s.size()) {
        char c = s[pos];
        if (stopAtDot   && c == '.')             break;
        if (stopAtParen && (c == '(' || c == ')')) break;
        long long val = -1;
        if (c == '[') {
            ++pos;
            std::string num;
            while (pos < s.size() && s[pos] != ']') num += s[pos++];
            if (pos >= s.size()) throw std::runtime_error("Незакрытая скобка [");
            ++pos;
            val = std::stoll(num);
        } else if (c >= '0' && c <= '9') {
            val = c - '0'; ++pos;
        } else if (c >= 'a' && c <= 'z') {
            val = c - 'a' + 10; ++pos;
        } else if (c >= 'A' && c <= 'Z') {
            val = c - 'A' + 10; ++pos;
        } else {
            break;
        }
        if (val < 0 || val >= base)
            throw std::runtime_error("Цифра " + std::to_string(val) +
                                     " >= основания " + std::to_string(base));
        digits.push_back(val);
    }
    return digits;
}

struct ParsedNumber {
    std::vector<long long> intDigits, fracDigits;
    int periodStart = -1;
};

static ParsedNumber parseNumber(const std::string& s, long long base) {
    ParsedNumber pn;
    size_t pos = 0;
    pn.intDigits = parseDigits(s, base, pos, true, false);
    if (pn.intDigits.empty()) throw std::runtime_error("Отсутствует целая часть");
    if (pos >= s.size()) return pn;

    if (s[pos] == '.') {
        ++pos;
        if (pos >= s.size()) throw std::runtime_error("Отсутствует дробная часть после точки");
        auto pre = parseDigits(s, base, pos, false, true);
        if (pos < s.size() && s[pos] == '(') {
            ++pos;
            if (pos >= s.size() || s[pos] == ')')
                throw std::runtime_error("Пустой период");
            auto period = parseDigits(s, base, pos, false, true);
            if (pos >= s.size() || s[pos] != ')')
                throw std::runtime_error("Незакрытая скобка периода");
            ++pos;
            pn.periodStart = (int)pre.size();
            pn.fracDigits = pre;
            for (auto d : period) pn.fracDigits.push_back(d);
        } else {
            if (pre.empty()) throw std::runtime_error("Пустая дробная часть");
            pn.fracDigits = pre;
        }
    }
    if (pos != s.size()) throw std::runtime_error("Лишние символы в конце");
    return pn;
}

// ============================================================


static const size_t MAX_OUTPUT_LENGTH = 100000;

MainPage::MainPage(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(580, 420);
    this->setWindowTitle("Universal Converter");

    value = new QTextEdit;
    value->setFixedSize(250, 60);
    value->setPlaceholderText("Введите число");

    p = new QLineEdit; p->setFixedSize(60, 32); p->setPlaceholderText("p");
    q = new QLineEdit; q->setFixedSize(60, 32); q->setPlaceholderText("q");

    auto* btnConvert = new QPushButton("Перевести");
    btnConvert->setFixedHeight(32);

    answer = new QTextEdit;
    answer->setReadOnly(true);
    answer->setFixedSize(540, 270);
    answer->setPlaceholderText("Результат появится здесь...");

    auto* topRow = new QHBoxLayout;
    topRow->addWidget(new QLabel("Число:"));  topRow->addWidget(value);
    topRow->addWidget(new QLabel("p:"));      topRow->addWidget(p);
    topRow->addWidget(new QLabel("q:"));      topRow->addWidget(q);
    topRow->addWidget(btnConvert);

    auto* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topRow);
    mainLayout->addWidget(answer);
    setLayout(mainLayout);

    connect(btnConvert, &QPushButton::clicked, this, &MainPage::onConvert);
}

MainPage::~MainPage() {}

void MainPage::onConvert()
{
    QString inputStr = value->toPlainText().trimmed();
    bool okP, okQ;
    long long pVal = p->text().trimmed().toLongLong(&okP);
    long long qVal = q->text().trimmed().toLongLong(&okQ);

    if (!okP || !okQ || pVal < 2 || pVal > 500 || qVal < 2 || qVal > 500) {
        answer->setText("Ошибка: основания p и q — целые числа от 2 до 500.");
        return;
    }
    try {
        ParsedNumber pn = parseNumber(inputStr.toStdString(), pVal);
        BigFraction frac = BigFraction::fromDigits(
            pn.intDigits, pn.fracDigits, pn.periodStart, pVal);
        answer->setText(QString::fromStdString(frac.toBase(qVal, MAX_OUTPUT_LENGTH)));
    } catch (const std::exception& e) {
        answer->setText(QString("Ошибка: ") + e.what());
    }
}
