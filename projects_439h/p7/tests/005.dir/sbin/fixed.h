#ifndef FIXED_H
#define FIXED_H

#include "stdint.h"

class fixed {
private:
	int32_t val;
public:
	static constexpr int32_t order = 20;
	static constexpr int32_t magnitude = 1 << order;
	
	fixed() : val(0) {}
	fixed(int32_t const& _val) : val(_val * magnitude) {}
	fixed(fixed const& that) : val(that.val) {}
	
	static fixed ticks(int32_t const& _val) {
		fixed out{};
		out.val = _val;
		return out;
	}

	template <typename thatType>
	fixed operator+(thatType const& that) const {
		return ticks(val + fixed(that).val);
	}
	template <typename thatType>
	fixed operator-(thatType const& that) const {
		return ticks(val - fixed(that).val);
	}
	template <typename thatType>
	fixed operator*(thatType const& that) const {
		return ticks((int32_t) (((int64_t) val * (int64_t) fixed(that).val) / (int64_t) magnitude));
	}
	template <typename thatType>
	fixed operator/(thatType const& that) const {
		// workaround because 64 bit division is not supported
		return ticks((int32_t) (((int64_t) magnitude * (int64_t) val) / (int64_t) fixed(that).val));
	}
	template <typename thatType>
	fixed operator+=(thatType const& that) {
		val += fixed(that).val;
		return *this;
	}
	template <typename thatType>
	fixed operator-=(thatType const& that) {
		val -= fixed(that).val;
		return *this;
	}
	template <typename thatType>
	fixed operator=(thatType const& that) {
		val = fixed(that).val;
		return *this;
	}
	template <typename thatType>
	bool operator<(thatType const& that) {
		return val < fixed(that).val;
	}
	template <typename thatType>
	bool operator==(thatType const& that) {
		return val == fixed(that).val;
	}
	template <typename thatType>
	bool operator>(thatType const& that) {
		return val > fixed(that).val;
	}
	template <typename thatType>		
	bool operator<=(thatType const& that) {
		return val <= fixed(that).val;
	}
	template <typename thatType>
	bool operator>=(thatType const& that) {
		return val >= fixed(that).val;
	}
	template <typename thatType>
	bool operator!=(thatType const& that) {
		return val != fixed(that).val;
	}

	void print() {
		bool sign = val < 0;
		int32_t abs_val = sign ? -val : val;
		const char* sign_str = sign ? "-" : "";
	
		int32_t integer_part = abs_val / magnitude;

		int32_t fractional_part = abs_val % magnitude;
		constexpr int32_t decimal_places = order / 4;
		char fractional_part_str[decimal_places + 1];
		fractional_part_str[decimal_places] = '\0';
		for (int32_t d = 0; d < decimal_places; d++) {
			fractional_part *= 10;
			int32_t digit = fractional_part / magnitude;
			fractional_part = fractional_part % magnitude;
			fractional_part_str[d] = ((char) digit) + '0';
		}

		printf("*** %s%ld.%s\n", sign_str, integer_part, fractional_part_str);
	}
};

#endif
