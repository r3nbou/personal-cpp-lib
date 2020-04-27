// Artem Mikheev 2020
// GNU GPLv3 License

#ifndef CPP_LIB_BIG_HPP
#define CPP_LIB_BIG_HPP

typedef uint64_t uint64;
typedef int64_t int64;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint8_t ubyte;
typedef uint16_t uint16;
typedef int16_t int16;
struct uint128 {
	uint64_t a, b;
};

uint128 mult64(uint64 a, uint64 b) {
	asm volatile (
	"mov %[ra], %%rax\n\t"
	"mul %[rb]\n\t"
	"mov %%rdx, %[ra]\n\t"
	"mov %%rax, %[rb]\n\t"
	: [ra]"+r"(a), [rb]"+r"(b)
	::
	"cc", "memory", "rax", "rdx"
	);
	uint128 result{a, b};
	return result;
}

uint128 div128by64(uint128 a, uint64 b) {
	asm volatile (
	"mov %[ra], %%rdx\n\t"
	"mov %[rb], %%rax\n\t"
	"div %[b]\n\t"
	"mov %%rax, %[ra]\n\t"
	"mov %%rdx, %[rb]\n\t"
	: [ra]"+r"(a.a), [rb]"+r"(a.b)
	: [b]"r"(b)
	: "cc", "memory", "rax", "rdx"
	);
	return a;
}

uint64 add64(uint64 a, uint64 b, ubyte *carry) {
	asm volatile(
	"add %[b], %[a]\n\t"
	"jnc nocarry\n\t"
	"movb $0x1, (%[car])\n\t"
	"nocarry:\n\t"
	: [a]"+r"(a), [car]"+r"(carry)
	: [b]"r"(b)
	: "cc", "memory"
	);
	return a;
}

uint128 add64to128(uint128 a, uint64 b) {
	asm volatile(
	"add %[b], %[rb]\n\t"
	"jnc nocarry1\n\t"
	"add $0x1, %[ra]\n\t"
	"nocarry1:\n\t"
	: [ra]"+r"(a.a), [rb]"+r"(a.b)
	: [b]"r"(b)
	: "cc", "memory"
	);
	return a;
}

uint64 sub64(uint64 a, uint64 b, ubyte *borrow) {
	asm volatile(
	"sub %[b], %[a]\n\t"
	"jnc noborrow\n\t"
	"movb $0x1, (%[bor])\n\t"
	"noborrow:\n\t"
	: [a]"+r"(a), [bor]"+r"(borrow)
	: [b]"r"(b)
	: "cc", "memory"
	);
	return a;
}

inline uint64 getReciprocal64(uint64 a) {
	return (div128by64({1, 0}, a).a + 1);
}

inline uint64 div64UsingMult(uint64 a, uint64 recip) {
	return mult64(a, recip).a;
}

inline uint64 getModFromDiv(uint64 a, uint64 b, uint64 c) {
	return a - b * c;
}

class BigInt {
private:
	static const uint64 _cellMax = 0xffffffffffffffffULL;
	Vector<uint64> _data;

	void removeLeadingZeros() {
		while (*size && _data.back() == 0)
			_data.pop();
		if (*size == 0)
			sign = 0;
	}

	void addTwoBigInts(const BigInt *a, ubyte signa, const BigInt *b, ubyte signb, BigInt *res) {
		res->sign = 0;
		if (signa) {
			if (signb) {
				res->sign = 1;
			} else {
				subTwoBigInts(a, 0, b, 0, res);
				return;
			}
		} else if (signb) {
			subTwoBigInts(a, 0, b, 0, res);
			return;
		}
		ubyte carry = 0, extra;
		uint32 end = Algorithm::max(*a->size, *b->size);
		res->_data.resize(end + 1);
		for (int32 i = 0; i < end || carry; i++) {
			extra = carry;
			carry = 0;
			res->_data[i] = add64(
				i < *a->size ? a->_data[i] : 0ULL,
				i < *b->size ? b->_data[i] : 0ULL,
				&carry);
			if (extra) {
				res->_data[i] += 1ULL;
				if (res->_data[i] == 0ULL) carry = 1;
			}
		}
		if (res->_data[*res->size - 1] == 0) res->_data.resize(*res->size - 1);
	}

	void subTwoBigInts(const BigInt *a, ubyte signa, const BigInt *b, ubyte signb, BigInt *res) {
		res->sign = 0;
		if (signa) {
			if (!signb) {
				addTwoBigInts(a, 1, b, 1, res);
				return;
			} else {
				Algorithm::swap(a, b);
			}
		} else if (signb) {
			addTwoBigInts(a, 0, b, 0, res);
			return;
		}
		if (a->operator<(*b)) {
			res->sign = 1;
			Algorithm::swap(a, b);
		}
		res->_data.resize(*a->size);
		ubyte borrow = 0, extra;
		int32 i = 0;
		for (; i < *b->size || borrow; i++) {
			extra = borrow;
			borrow = 0;
			res->_data[i] = sub64(a->_data[i], i < *b->size ? b->_data[i] : 0ULL, &borrow);
			if (extra) {
				res->_data[i] -= 1ULL;
				if (res->_data[i] == _cellMax) borrow = 1;
			}
		}
		for (; i < *a->size; i++)
			res->_data[i] = a->_data[i];
		res->removeLeadingZeros();
	}

	void multTwoBigInts(const BigInt *a, ubyte signa, const BigInt *b, ubyte signb, BigInt *res) {
		res->sign = 0;
		if (signa ^ signb)
			res->sign = 1;
		res->resize(*a->size + *b->size);
		uint128 cur;
		for (int32 i = 0; i < *a->size; i++) {
			uint64 carry = 0;
			for (int32 j = 0; j < *b->size || carry; j++) {
				cur = mult64(a->_data[i], j < *b->size ? b->_data[j] : 0ULL);
				cur = add64to128(cur, res->_data[i + j]);
				cur = add64to128(cur, carry);
				res->_data[i + j] = cur.b;
				carry = cur.a;
			}
		}
		res->removeLeadingZeros();
	}

	// divides BigInt a by uint64 b, storing result in BigInt res and returning a%b
	// IF DIVIDING NEGATIVE BIGINT BY UINT64 THE MODULO WILL BE POSITIVE, WHICH IS INCORRECT
	// IF YOU NEED PROPER MODULO, CONVERT UINT64 TO BIGINT AND THEN DIVIDE
	// IF DIVIDING BY INT64 (SIGNED) THE MODULO RESULT WILL BE CORRECT

	uint64 divByUInt64(const BigInt *a, ubyte signa, uint64 b, ubyte signb, BigInt *res) {
		ubyte ressign = 0;
		if (b == 0) {
			throw std::runtime_error("Can't divide BigInt by zero.");
		}
		if (signa ^ signb)
			ressign = 1;
		uint64 carry = 0;
		uint128 divres;
		if (res != nullptr)
			res->_data.resize(*a->size);
		for (int32 i = *a->size - 1; i >= 0; i--) {
			uint128 cur = {carry, a->_data[i]};
			divres = div128by64(cur, b);
			if (res != nullptr)
				res->_data[i] = divres.a;
			carry = divres.b;
		}
		if (res != nullptr) {
			res->sign = ressign;
			res->removeLeadingZeros();
		}
		// only works if dividing by int64, not uint64
		if (ressign)
			return -(int64) carry;
		else
			return carry;
	}

	void multByUInt64(const BigInt *a, ubyte signa, uint64 b, ubyte signb, BigInt *res) {
		ubyte ressign = 0;
		if (signa ^ signb)
			ressign = 1;
		uint64 carry = 0;
		res->_data.resize(*a->size);
		for (int32 i = 0; i < *a->size || carry; i++) {
			if (i == *a->size)
				res->_data.push(0);
			uint128 cur = mult64(a->_data[i], b);
			cur = add64to128(cur, carry);
			carry = cur.a;
			res->_data[i] = cur.b;
		}
		res->sign = ressign;
		res->removeLeadingZeros();
	}

	void shiftLeft(BigInt *a, uint32 shiftSize, BigInt *res) {
		uint32 newStart = (shiftSize >> 6);
		uint32 newSize = *a->size + newStart + 1;
		uint32 oldSize = *a->size;
		res->_data.resize(newSize);
		ubyte upper = shiftSize & 0b111111;
		ubyte lower = 64 - upper;
		for (int32 i = oldSize - 1, j = 0; i >= 0; i--, j++) {
			res->_data[newStart + j + 1] |= (a->_data[j] >> lower);
			res->_data[newStart + j] |= (a->_data[j] << upper);
		}
		res->removeLeadingZeros();
	}

public:
	const sizeT *size = _data.size;
	ubyte sign = 0;

	BigInt()
		: sign(0),
		  _data() {}

	// add back only if you don't have 64bit processor
	/*
	BigInt(int32_t value)
	: BigInt() {
		if (value < 0) {
			sign = 1;
			value = -value;
		}
		ldiv_t dv;
		while (value) {
			dv = ldiv(value, _cellMax);
			_data.push(dv.rem);
			value = dv.quot;
		}
	}
	*/

	BigInt(int64 value)
		: BigInt() {
		if (value < 0) {
			sign = 1;
			value = -value;
		}
		_data.push(value);
	}

	BigInt(uint64 value)
		: BigInt() {
		_data.push(value);
	}

	BigInt(String value) {
		if (value[0] == U'-')
			sign = 1;
		int32 i = sign;
		BigInt tmp;
		tmp.resize(1);
		int32 mod18tmp = *value.size % 18;
		if (mod18tmp != 0) {
			tmp[0] = value.substr(0, mod18tmp).toUInt();
			addTwoBigInts(this, 0, &tmp, 0, this);
			i += mod18tmp;
		}
		for (; i < *value.size; i += 18) {
			multByUInt64(this, 0, 1000000000000000000ULL, 0, this);
			tmp[0] = value.substr(i, 18).toUInt();
			addTwoBigInts(this, 0, &tmp, 0, this);
		}
		removeLeadingZeros();
	}

	BigInt(const BigInt &other)
		: _data(other._data) {
		sign = other.sign;
		size = _data.size;
	}

	BigInt &operator=(const BigInt &other) {
		_data = other._data;
		sign = other.sign;
		size = _data.size;
		return *this;
	}

	bool operator==(const BigInt &other) {
		return _data == other._data;
	}

	bool operator<(const BigInt &other) const {
		if (*size < *other.size) return true;
		if (*size > *other.size) return false;
		for (int32_t i = *size - 1; i >= 0; i--) {
			if (_data[i] == other._data[i])
				continue;
			if (_data[i] < other._data[i])
				return true;
			else if (_data[i] > other._data[i])
				return false;
		}
		return false;
	}

	bool operator>=(const BigInt &other) const {
		return other.operator<(*this);
	}

	bool operator>(const BigInt &other) const {
		if (*size > *other.size) return true;
		if (*size < *other.size) return false;
		for (int32_t i = *size - 1; i >= 0; i--) {
			if (_data[i] == other._data[i])
				continue;
			if (_data[i] > other._data[i])
				return true;
			else if (_data[i] < other._data[i])
				return false;
		}
		return false;
	}

	bool operator<=(const BigInt &other) const {
		return other.operator<(*this);
	}

	BigInt operator+(const BigInt &other) {
		BigInt result;
		addTwoBigInts(this, sign, &other, other.sign, &result);
		return result;
	}

	BigInt &operator+=(const BigInt &other) {
		addTwoBigInts(this, sign, &other, other.sign, this);
		return *this;
	}

	BigInt operator-(const BigInt &other) {
		BigInt result;
		subTwoBigInts(this, sign, &other, other.sign, &result);
		return result;
	}

	BigInt &operator-=(const BigInt &other) {
		subTwoBigInts(this, sign, &other, other.sign, this);
		return *this;
	}

	BigInt operator*(const BigInt &other) {
		BigInt result;
		multTwoBigInts(this, sign, &other, other.sign, &result);
		return result;
	}

	BigInt operator*=(const BigInt &other) {
		multTwoBigInts(this, sign, &other, other.sign, this);
		return *this;
	}

	BigInt operator*(uint64 a) {
		BigInt result;
		multByUInt64(this, sign, a, 0, &result);
		return result;
	}

	BigInt operator*(int64 a) {
		BigInt result;
		multByUInt64(this, sign, a, a < 0, &result);
		return result;
	}

	BigInt operator*=(uint64 a) {
		multByUInt64(this, sign, a, 0, this);
		return *this;
	}

	BigInt operator*=(int64 a) {
		multByUInt64(this, sign, a, a < 0, this);
		return *this;
	}

	BigInt operator/(uint64 a) {
		BigInt result;
		divByUInt64(this, sign, a, 0, &result);
		return result;
	}

	BigInt operator/(int64 a) {
		BigInt result;
		divByUInt64(this, sign, a, a < 0, &result);
		return result;
	}

	BigInt &operator/=(uint64 a) {
		divByUInt64(this, sign, a, 0, this);
		return *this;
	}

	BigInt &operator/=(int64 a) {
		divByUInt64(this, sign, a, a < 0, this);
		return *this;
	}

	BigInt operator<<(uint32 a) {
		BigInt result;
		shiftLeft(this, a, &result);
		return result;
	}

	BigInt operator<<=(uint32 a) {
		*this = this->operator<<(a);
		return *this;
	}

	uint64 operator%(uint64 a) {
		return divByUInt64(this, sign, a, 0, nullptr);
	}

	int64 operator%(int64 a) {
		return divByUInt64(this, sign, a, a < 0, nullptr);
	}

	Pair<BigInt, uint64> divrem(uint64 a) {
		BigInt result;
		uint64 res = divByUInt64(this, sign, a, 0, &result);
		return {result, res};
	}

	Pair<BigInt, int64> divrem(int64 a) {
		BigInt result;
		int64 res = divByUInt64(this, sign, a, a < 0, &result);
		return {result, res};
	}

	BigInt &operator%=(uint64 a) {
		uint64 res = divByUInt64(this, sign, a, 0, nullptr);
		_data.resize(1);
		_data[0] = res;
		return *this;
	}

	BigInt &operator%=(int64 a) {
		int64 res = divByUInt64(this, sign, a, a > 0, nullptr);
		_data.resize(1);
		if (res < 0) {
			sign = 1;
			res = -res;
		}
		_data[0] = res;
		return *this;
	}

	uint64 &operator[](int32 i) const {
		return _data[i];
	}

	uint64 at(int32 i) const {
		return _data.at(i);
	}

	void resize(uint32 sz) {
		_data.resize(sz);
	}

	String getValue() {
		String result;
		BigInt tmpn;
		uint64 dmod = divByUInt64(this, 0, 1000000000000000000ULL, 0, &tmpn);
		uint64 recip = getReciprocal64(10ULL);
		while (*tmpn.size) {
			for (int32 i = 0; i < 18; i++) {
				uint64 newcur = div64UsingMult(dmod, recip);
				uint64 mod = getModFromDiv(dmod, 10ULL, newcur);
				result += String(1, char('0' + mod));
				dmod = newcur;
			}
			dmod = divByUInt64(&tmpn, 0, 1000000000000000000ULL, 0, &tmpn);
		}
		while (dmod) {
			uint64 newcur = div64UsingMult(dmod, recip);
			uint64 mod = getModFromDiv(dmod, 10ULL, newcur);
			result += String(1, char('0' + mod));
			dmod = newcur;
		}
		if (*result.size == 0)
			result += String(1, '0');
		if (sign)
			result += String(1, '-');
		Algorithm::reverse(result, *result.size);
		return result;
	}
};

#endif //CPP_LIB_BIG_HPP
