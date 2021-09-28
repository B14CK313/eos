//
// Created by jakob on 27.09.21.
//

#pragma once

#include <cstddef>
#include <array>
#include <stdexcept>
#include <fmt/core.h>

namespace eos {

	template<typename T, std::size_t dim> requires (std::is_arithmetic_v<T> && dim > 0)
	struct Vec {
		typedef T value_type;
		static constexpr std::size_t dimension = dim;

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		explicit Vec(U value) {
			for (int i{0}; i < dim; ++i) {
				values_[i] = static_cast<T>(value);
			}
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		explicit Vec(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] = static_cast<T>(vec[i]);
			}
		}

		template<typename... U>
		requires ((std::is_arithmetic_v<U> && std::is_convertible_v<U, T>) && ...)
		explicit Vec(U... values) : values_{static_cast<T>(values)...} {}


		// Unary arithmetic operators
		constexpr Vec<T, dim>& operator=(const Vec<T, dim>& vec) = default;

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator=(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] = static_cast<T>(vec[i]);
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator+=(U scalar) {
			for (int i{0}; i < dim; ++i) {
				values_[i] += static_cast<T>(scalar);
			}
			return *this;
		}

		template<typename U>
		requires (dim > 1 && std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator+=(const Vec<U, 1>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] += static_cast<T>(vec.x());
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator+=(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] += static_cast<T>(vec[i]);
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator-=(U scalar) {
			for (int i{0}; i < dim; ++i) {
				values_[i] -= static_cast<T>(scalar);
			}
			return *this;
		}

		template<typename U>
		requires (dim > 1 && std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator-=(const Vec<U, 1>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] -= static_cast<T>(vec.x());
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator-=(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] -= static_cast<T>(vec[i]);
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator*=(U scalar) {
			for (int i{0}; i < dim; ++i) {
				values_[i] *= static_cast<T>(scalar);
			}
			return *this;
		}

		template<typename U>
		requires (dim > 1 && std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator*=(const Vec<U, 1>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] *= static_cast<T>(vec.x());
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator*=(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] *= static_cast<T>(vec[i]);
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator/=(U scalar) {
			T temp{static_cast<T>(scalar)};
#ifdef EOS_VEC_ERROR_CHECKS
			if(temp == 0) throw std::domain_error{"Vec::operator/=: Division by 0 is not defined"};
#endif //EOS_VEC_ERROR_CHECKS
			for (int i{0}; i < dim; ++i) {
				values_[i] /= temp;
			}
			return *this;
		}

		template<typename U>
		requires (dim > 1 && std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator/=(const Vec<U, 1>& vec) {
			T temp{static_cast<T>(vec.x())};
#ifdef EOS_VEC_ERROR_CHECKS
			if(temp == 0) throw std::domain_error{"Vec::operator/=: Division by 0 is not defined"};
#endif //EOS_VEC_ERROR_CHECKS
			for (int i{0}; i < dim; ++i) {
				values_[i] /= temp;
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator/=(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
#ifdef EOS_VEC_ERROR_CHECKS
				T temp{static_cast<T>(vec[i])};
				if(temp == 0) throw std::domain_error("Vec::operator/=: Division by 0 is not defined");
				values_[i] /= temp;
#else //EOS_VEC_ERROR_CHECKS
				values_[i] /= static_cast<T>(vec[i]);
#endif //EOS_VEC_ERROR_CHECKS
			}
			return *this;
		}


		// Increment & decrement arithmetic operators
		// prefix
		constexpr Vec<T, dim>& operator++() {
			for (int i{0}; i < dim; ++i) {
				++values_[i];
			}
			return *this;
		}

		constexpr Vec<T, dim>& operator--() {
			for (int i{0}; i < dim; ++i) {
				--values_[i];
			}
			return *this;
		}

		// postfix
		constexpr const Vec<T, dim> operator++(int) {
			Vec<T, dim> old{*this};
			operator++();
			return old;
		}

		constexpr const Vec<T, dim> operator--(int) {
			Vec<T, dim> old{*this};
			operator--();
			return old;
		}


		// Unary bit operators
		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator%=(U scalar) {
			T temp{static_cast<T>(scalar)};
#ifdef EOS_VEC_ERROR_CHECKS
			if(temp == 0) throw std::domain_error{"Vec::operator%=: Division by 0 is not defined"};
#endif //EOS_VEC_ERROR_CHECKS
			for (int i{0}; i < dim; ++i) {
				values_[i] %= temp;
			}
			return *this;
		}

		template<typename U>
		requires (dim > 1 && std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator%=(const Vec<U, 1>& vec) {
			T temp{static_cast<T>(vec.x())};
#ifdef EOS_VEC_ERROR_CHECKS
			if(temp == 0) throw std::domain_error{"Vec::operator%=: Division by 0 is not defined"};
#endif //EOS_VEC_ERROR_CHECKS
			for (int i{0}; i < dim; ++i) {
				values_[i] %= temp;
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator%=(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
#ifdef EOS_VEC_ERROR_CHECKS
				T temp{static_cast<T>(vec[i])};
				if(temp == 0) throw std::domain_error("Vec::operator%=: Division by 0 is not defined");
				values_[i] %= temp;
#else //EOS_VEC_ERROR_CHECKS
				values_[i] %= static_cast<T>(vec[i]);
#endif //EOS_VEC_ERROR_CHECKS
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator&=(U scalar) {
			for (int i{0}; i < dim; ++i) {
				values_[i] &= static_cast<T>(scalar);
			}
			return *this;
		}

		template<typename U>
		requires (dim > 1 && std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator&=(const Vec<U, 1>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] &= static_cast<T>(vec.x());
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator&=(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] &= static_cast<T>(vec[i]);
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator|=(U scalar) {
			for (int i{0}; i < dim; ++i) {
				values_[i] |= static_cast<T>(scalar);
			}
			return *this;
		}

		template<typename U>
		requires (dim > 1 && std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator|=(const Vec<U, 1>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] |= static_cast<T>(vec.x());
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator|=(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] |= static_cast<T>(vec[i]);
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator^=(U scalar) {
			for (int i{0}; i < dim; ++i) {
				values_[i] ^= static_cast<T>(scalar);
			}
			return *this;
		}

		template<typename U>
		requires (dim > 1 && std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator^=(const Vec<U, 1>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] ^= static_cast<T>(vec.x());
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator^=(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] ^= static_cast<T>(vec[i]);
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator<<=(U scalar) {
			for (int i{0}; i < dim; ++i) {
				values_[i] <<= static_cast<T>(scalar);
			}
			return *this;
		}

		template<typename U>
		requires (dim > 1 && std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator<<=(const Vec<U, 1>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] <<= static_cast<T>(vec.x());
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator<<=(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] <<= static_cast<T>(vec[i]);
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator>>=(U scalar) {
			for (int i{0}; i < dim; ++i) {
				values_[i] >>= static_cast<T>(scalar);
			}
			return *this;
		}

		template<typename U>
		requires (dim > 1 && std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator>>=(const Vec<U, 1>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] >>= static_cast<T>(vec.x());
			}
			return *this;
		}

		template<typename U>
		requires (std::is_arithmetic_v<U> && std::is_convertible_v<U, T>)
		constexpr Vec<T, dim>& operator>>=(const Vec<U, dim>& vec) {
			for (int i{0}; i < dim; ++i) {
				values_[i] >>= static_cast<T>(vec[i]);
			}
			return *this;
		}


		// member access
		constexpr T& x() requires (dim > 0) { return values_[0]; };

		[[nodiscard]] constexpr const T& x() const requires (dim > 0) { return values_[0]; };

		constexpr T& r() requires (dim > 0) { return values_[0]; };

		[[nodiscard]] constexpr const T& r() const requires (dim > 0) { return values_[0]; };

		constexpr T& s() requires (dim > 0) { return values_[0]; };

		[[nodiscard]] constexpr const T& s() const requires (dim > 0) { return values_[0]; };

		constexpr T& y() requires (dim > 1) { return values_[1]; };

		[[nodiscard]] constexpr const T& y() const requires (dim > 1) { return values_[1]; };

		constexpr T& g() requires (dim > 1) { return values_[1]; };

		[[nodiscard]] constexpr const T& g() const requires (dim > 1) { return values_[1]; };

		constexpr T& t() requires (dim > 1) { return values_[1]; };

		[[nodiscard]] constexpr const T& t() const requires (dim > 1) { return values_[1]; };

		constexpr T& z() requires (dim > 2) { return values_[2]; };

		[[nodiscard]] constexpr const T& z() const requires (dim > 2) { return values_[2]; };

		constexpr T& b() requires (dim > 2) { return values_[2]; };

		[[nodiscard]] constexpr const T& b() const requires (dim > 2) { return values_[2]; };

		constexpr T& p() requires (dim > 2) { return values_[2]; };

		[[nodiscard]] constexpr const T& p() const requires (dim > 2) { return values_[2]; };

		constexpr T& w() requires (dim > 3) { return values_[3]; };

		[[nodiscard]] constexpr const T& w() const requires (dim > 3) { return values_[3]; };

		constexpr T& a() requires (dim > 3) { return values_[3]; };

		[[nodiscard]] constexpr const T& a() const requires (dim > 3) { return values_[3]; };

		constexpr T& q() requires (dim > 3) { return values_[3]; };

		[[nodiscard]] constexpr const T& q() const requires (dim > 3) { return values_[3]; };

		// Returns a reference to the element at index. No bounds checking is performed.
		constexpr T& operator[](std::size_t index) {
			return values_[index];
		};

		[[nodiscard]] constexpr const T& operator[](std::size_t index) const {
			return values_[index];
		};

		// Returns a reference to the element at index, with bounds checking.
		constexpr T& at(std::size_t index) {
			if (index >= dim) {
				throw std::out_of_range{fmt::format("Vec::operator[]: index {} >= dim {}", index, dim)};
			}
			return values_[index];
		}

		[[nodiscard]] constexpr const T& at(std::size_t index) const {
			if (index >= dim) {
				throw std::out_of_range{fmt::format("Vec::operator[]: index {} >= dim {}", index, dim)};
			}
			return values_[index];
		}

		template<std::size_t index>
		constexpr auto& get() {
			return values_[index];
		}

		template<std::size_t index>
		[[nodiscard]] constexpr const auto& get() const {
			return values_[index];
		}

	private:
		T values_[dim];
	};


	// template deduction guide
	template<typename T, typename... U>
	Vec(T, U...) -> Vec<std::enable_if_t<std::is_arithmetic_v<T> && (std::is_same_v<T, U> && ...), T>,
			1 + sizeof...(U)>;


	// Unary operators
	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator+(const Vec<T, dim>& vec) {
		return vec;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator-(const Vec<T, dim>& vec) {
		return Vec<T, dim>{0} - vec;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator~(const Vec<T, dim>& vec) {
		Vec<T, dim> temp{};
		for (int i{0}; i < dim; ++i) {
			temp[i] = ~vec[i];
		}
		return temp;
	}


	// Binary arithmetic operators
	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator+(const Vec<T, dim>& vec, const T& scalar) {
		return Vec<T, dim>{vec} += scalar;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator+(const Vec<T, dim>& vec1, const Vec<T, 1>& vec2) {
		return Vec<T, dim>{vec1} += vec2;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator+(const Vec<T, 1>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1.x()} += vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator+(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1} += vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator-(const Vec<T, dim>& vec, const T& scalar) {
		return Vec<T, dim>{vec} -= scalar;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator-(const Vec<T, dim>& vec1, const Vec<T, 1>& vec2) {
		return Vec<T, dim>{vec1} -= vec2;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator-(const Vec<T, 1>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1.x()} -= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator-(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1} -= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator*(const Vec<T, dim>& vec, const T& scalar) {
		return Vec<T, dim>{vec} *= scalar;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator*(const Vec<T, dim>& vec1, const Vec<T, 1>& vec2) {
		return Vec<T, dim>{vec1} *= vec2;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator*(const Vec<T, 1>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1.x()} *= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator*(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1} *= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator/(const Vec<T, dim>& vec, const T& scalar) {
		return Vec<T, dim>{vec} /= scalar;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator/(const Vec<T, dim>& vec1, const Vec<T, 1>& vec2) {
		return Vec<T, dim>{vec1} /= vec2;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator/(const Vec<T, 1>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1.x()} /= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator/(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1} /= vec2;
	}


	// Binary bit operators
	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator%(const Vec<T, dim>& vec, const T& scalar) {
		return Vec<T, dim>{vec} %= scalar;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator%(const Vec<T, dim>& vec1, const Vec<T, 1>& vec2) {
		return Vec<T, dim>{vec1} %= vec2;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator%(const Vec<T, 1>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1.x()} %= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator%(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1} %= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator&(const Vec<T, dim>& vec, const T& scalar) {
		return Vec<T, dim>{vec} &= scalar;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator&(const Vec<T, dim>& vec1, const Vec<T, 1>& vec2) {
		return Vec<T, dim>{vec1} &= vec2;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator&(const Vec<T, 1>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1.x()} &= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator&(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1} &= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator|(const Vec<T, dim>& vec, const T& scalar) {
		return Vec<T, dim>{vec} |= scalar;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator|(const Vec<T, dim>& vec1, const Vec<T, 1>& vec2) {
		return Vec<T, dim>{vec1} |= vec2;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator|(const Vec<T, 1>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1.x()} |= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator|(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1} |= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator^(const Vec<T, dim>& vec, const T& scalar) {
		return Vec<T, dim>{vec} ^= scalar;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator^(const Vec<T, dim>& vec1, const Vec<T, 1>& vec2) {
		return Vec<T, dim>{vec1} ^= vec2;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator^(const Vec<T, 1>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1.x()} ^= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator^(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1} ^= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator<<(const Vec<T, dim>& vec, const T& scalar) {
		return Vec<T, dim>{vec} <<= scalar;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator<<(const Vec<T, dim>& vec1, const Vec<T, 1>& vec2) {
		return Vec<T, dim>{vec1} <<= vec2;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator<<(const Vec<T, 1>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1.x()} <<= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator<<(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1} <<= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator>>(const Vec<T, dim>& vec, const T& scalar) {
		return Vec<T, dim>{vec} >>= scalar;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator>>(const Vec<T, dim>& vec1, const Vec<T, 1>& vec2) {
		return Vec<T, dim>{vec1} >>= vec2;
	}

	template<typename T, std::size_t dim>
	requires (dim > 1)
	[[nodiscard]] constexpr Vec<T, dim> operator>>(const Vec<T, 1>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1.x()} >>= vec2;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr Vec<T, dim> operator>>(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		return Vec<T, dim>{vec1} >>= vec2;
	}


	// Boolean operators
	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr bool operator==(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		for (int i{0}; i < dim; ++i) {
			if (vec1[i] != vec2[i]) return false;
		}
		return true;
	}

	template<typename T, std::size_t dim>
	[[nodiscard]] constexpr bool operator!=(const Vec<T, dim>& vec1, const Vec<T, dim>& vec2) {
		return !operator==(vec1, vec2);
	}

	template<std::size_t dim>
	[[nodiscard]] constexpr Vec<bool, dim> operator&&(const Vec<bool, dim>& vec1, const Vec<bool, dim>& vec2) {
		Vec<bool, dim> temp{};
		for (int i{0}; i < dim; ++i) {
			temp[i] = vec1[i] && vec2[i];
		}
		return temp;
	}

	template<std::size_t dim>
	[[nodiscard]] constexpr Vec<bool, dim> operator||(const Vec<bool, dim>& vec1, const Vec<bool, dim>& vec2) {
		Vec<bool, dim> temp{};
		for (int i{0}; i < dim; ++i) {
			temp[i] = vec1[i] || vec2[i];
		}
		return temp;
	}


#ifdef EOS_VEC_ALIAS_DIMENSIONS
	template<typename T> using Vec1 = Vec<T, 1>;
	template<typename T> using Vec2 = Vec<T, 2>;
	template<typename T> using Vec3 = Vec<T, 3>;
	template<typename T> using Vec4 = Vec<T, 4>;
#endif //EOS_VEC_ALIAS_DIMENSIONS

#ifdef EOS_VEC_ALIAS_TYPES
	template<std::size_t dim> using BoolVec = Vec<bool, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using BoolVec1 = Vec1<bool>;
	using BoolVec2 = Vec2<bool>;
	using BoolVec3 = Vec3<bool>;
	using BoolVec4 = Vec4<bool>;
#endif //EOS_VEC_ALIAS_DIMENSIONS


	template<std::size_t dim> using IntVec = Vec<int, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using IntVec1 = Vec1<int>;
	using IntVec2 = Vec2<int>;
	using IntVec3 = Vec3<int>;
	using IntVec4 = Vec4<int>;
#endif //EOS_VEC_ALIAS_DIMENSIONS

	template<std::size_t dim> using Int8Vec = Vec<int8_t, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using Int8Vec1 = Vec1<uint8_t>;
	using Int8Vec2 = Vec2<uint8_t>;
	using Int8Vec3 = Vec3<uint8_t>;
	using Int8Vec4 = Vec4<uint8_t>;
#endif //EOS_VEC_ALIAS_DIMENSIONS

	template<std::size_t dim> using Int16Vec = Vec<int16_t, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using Int16Vec1 = Vec1<uint16_t>;
	using Int16Vec2 = Vec2<uint16_t>;
	using Int16Vec3 = Vec3<uint16_t>;
	using Int16Vec4 = Vec4<uint16_t>;
#endif //EOS_VEC_ALIAS_DIMENSIONS

	template<std::size_t dim> using Int32Vec = Vec<int32_t, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using Int32Vec1 = Vec1<uint32_t>;
	using Int32Vec2 = Vec2<uint32_t>;
	using Int32Vec3 = Vec3<uint32_t>;
	using Int32Vec4 = Vec4<uint32_t>;
#endif //EOS_VEC_ALIAS_DIMENSIONS

	template<std::size_t dim> using Int64Vec = Vec<int64_t, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using Int64Vec1 = Vec1<uint64_t>;
	using Int64Vec2 = Vec2<uint64_t>;
	using Int64Vec3 = Vec3<uint64_t>;
	using Int64Vec4 = Vec4<uint64_t>;
#endif //EOS_VEC_ALIAS_DIMENSIONS


	template<std::size_t dim> using UintVec = Vec<unsigned int, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using UintVec1 = Vec1<unsigned int>;
	using UintVec2 = Vec2<unsigned int>;
	using UintVec3 = Vec3<unsigned int>;
	using UintVec4 = Vec4<unsigned int>;
#endif //EOS_VEC_ALIAS_DIMENSIONS

	template<std::size_t dim> using Uint8Vec = Vec<uint8_t, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using Uint8Vec1 = Vec1<uint8_t>;
	using Uint8Vec2 = Vec2<uint8_t>;
	using Uint8Vec3 = Vec3<uint8_t>;
	using Uint8Vec4 = Vec4<uint8_t>;
#endif //EOS_VEC_ALIAS_DIMENSIONS

	template<std::size_t dim> using Uint16Vec = Vec<uint16_t, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using Uint16Vec1 = Vec1<uint16_t>;
	using Uint16Vec2 = Vec2<uint16_t>;
	using Uint16Vec3 = Vec3<uint16_t>;
	using Uint16Vec4 = Vec4<uint16_t>;
#endif //EOS_VEC_ALIAS_DIMENSIONS

	template<std::size_t dim> using Uint32Vec = Vec<uint32_t, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using Uint32Vec1 = Vec1<uint32_t>;
	using Uint32Vec2 = Vec2<uint32_t>;
	using Uint32Vec3 = Vec3<uint32_t>;
	using Uint32Vec4 = Vec4<uint32_t>;
#endif //EOS_VEC_ALIAS_DIMENSIONS

	template<std::size_t dim> using Uint64Vec = Vec<uint64_t, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using Uint64Vec1 = Vec1<uint64_t>;
	using Uint64Vec2 = Vec2<uint64_t>;
	using Uint64Vec3 = Vec3<uint64_t>;
	using Uint64Vec4 = Vec4<uint64_t>;
#endif //EOS_VEC_ALIAS_DIMENSIONS


	template<std::size_t dim> using FloatVec = Vec<float, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using FloatVec1 = Vec1<float>;
	using FloatVec2 = Vec2<float>;
	using FloatVec3 = Vec3<float>;
	using FloatVec4 = Vec4<float>;
#endif //EOS_VEC_ALIAS_DIMENSIONS

	template<std::size_t dim> using DoubleVec = Vec<double, dim>;
#ifdef EOS_VEC_ALIAS_DIMENSIONS
	using DoubleVec1 = Vec1<double>;
	using DoubleVec2 = Vec2<double>;
	using DoubleVec3 = Vec3<double>;
	using DoubleVec4 = Vec4<double>;
#endif //EOS_VEC_ALIAS_DIMENSIONS

#endif //EOS_VEC_ALIAS_TYPES
}

namespace std {
	template<typename T, std::size_t dim>
	struct tuple_size<eos::Vec<T, dim>> : public std::integral_constant<std::size_t, dim> {
	};

	template<std::size_t index, typename T, std::size_t dim>
	struct tuple_element<index, eos::Vec<T, dim>> {
		using type = T;
	};
}
