#pragma once
#include <string>
#include <array>
#include <set>
#include <map>
#include <type_traits>
#include <concepts>
#include <source_location>

#include "ANSI.h"
// FUNCTIONS
namespace m0st4fa {

	inline namespace utility {
		// STRING
		template <typename T>
		concept ConvertableToString = std::is_convertible_v<T, std::string>;
		template <typename T>
		concept NumConvertableToString = requires (T a) { std::to_string(a); };

		template <typename T>
		// require the object to be iterable and its elements be convertible to strings
			requires requires (T iterable) {
				{*iterable.begin()} -> ConvertableToString;
		}
		std::string toString(const T& iterable, bool asList = true) {
			std::string separator = asList ? ", " : "\n";

			std::string temp = "{ ";

			if (iterable.empty())
				return temp += " }";

			temp += (std::string)*iterable.begin();

			for (size_t i = 0; const auto & element : iterable) {
				// skip the first element
				if (-not i) {
					i++;
					continue;
				}

				temp += separator + (std::string)element;
			}

			return temp += " }";
		}

		template <typename T>
		// require the object to be iterable and its elements be convertible to strings
			requires requires (T iterable) {
				{*iterable.begin()} -> NumConvertableToString;
		}
		std::string toString(const T& iterable, bool asList = true) {
			std::string separator = asList ? ", " : "\n";

			std::string temp = "{ ";

			if (iterable.empty())
				return temp += " }";

			temp += std::to_string(*iterable.begin());

			for (size_t i = 0; const auto & element : iterable) {
				// skip the first element
				if (-not i) {
					i++;
					continue;
				}

				temp += separator + std::to_string(element);
			}

			return temp += " }";
		}

		// require the object to be a 2D array and its elements be convertible to strings
		template <NumConvertableToString T, size_t xdim, size_t ydim>
		std::string toString(const std::array<std::array<T, ydim>, xdim>& array, bool asList = true) {
			std::string separator = asList ? ", " : "\n";

			std::string temp = "{ ";

			if (array.empty())
				return temp += " }";

			// if the array is not empty

			for (size_t x = 0; const std::array<T, ydim>&subarr : array) {

				if (subarr.empty()) {
					x++;
					continue;
				}

				for (size_t y = 0; T element : subarr) {
					if (!element) {
						y++;
						continue;
					}

					temp += std::format("[{}][{}] = {}\n", x, y, subarr.at(y));
					y++;
				}

				x++;
			}

			return temp += " }";
		}


		template<typename K, typename V>
		std::string toString(const std::map<K, V>& map) {
			using MapType = std::map<K, V>;
			std::string res;

			for (const auto& pair : map)
				// FORMAT:
				// Key : Value
				res += std::format("{} : {}\n", pair.first, toString(pair.second));

			return res;
		};

		std::string toString(const std::source_location&, bool = false);
			
		// INTEGER

		/**
		* extract the first integer from the string and convert it into size_t
		* the integer may not necessarily be at the beginning except if the boolean is set.
		*/
		size_t toInteger(const std::string&, bool = false);
		size_t pow(size_t, size_t);

		// ITERABLE
		/**
		* Inserts elements from `from` to `to` except any element that equals `except`
		* If the type of `except` is nullptr_t, it is ignored and all elements are added
		*/
		template <typename IterableT, typename ExceptT>
		bool insertAndAssert(const IterableT& from, IterableT& to, ExceptT except) {
			bool added = false;

			for (auto element : from) {
				if (element == except)
					continue;

				auto p = to.insert(element);

				if (p.second)
					added = true;
			}

			return added;
		};

		template <typename IterableT>
		bool insertAndAssert(const IterableT& from, IterableT& to) {
			bool added = false;

			for (auto element : from) {
				auto p = to.insert(element);

				if (p.second)
					added = true;
			}

			return added;
		};

		template <typename ElementT, typename IterableT>
		bool isIn(const ElementT element, const IterableT& iterable) {
			for (const ElementT& e : iterable)
				if (e == element)
					return true;

			return false;
		}

		/**
		* works with iterables that have method `contains`
		*/
		template<typename IterableT>
			requires requires (IterableT a) {
			a.contains(*a.begin());
		}
		bool operator==(const IterableT& lhs, const IterableT& rhs) {
			// if they do not have the same size, they are not coequal
			if (lhs.size() != rhs.size())
				return false;

			// if they have the same number of elements

			// if, for every element `a` in `lhs`, `a` is also an element
			// of `rhs`, then they are coequal

			for (const auto& lhsElement : lhs) {
				// check whether `rhs` contains that element
				if (rhs.contains(lhsElement))
					continue;

				// if `rhs` does not contain that element
				return false;
			}

			return true;
		};

		/**
		* works with iterables that do not have method `contains`, but that have method `at`
		*/
		template<typename IterableT>
			requires requires (IterableT a) {
			a.at(0);
		}
		bool operator==(const IterableT& lhs, const IterableT& rhs) {
			const size_t lhsSize = lhs.size();
			const size_t rhsSize = rhs.size();

			// if they have different sizes, they are not equal
			if (lhsSize != rhsSize)
				return false;

			// if there exists at least a single element in `lhs`
			// that does not exist in `rhs`, in order, they are not equal
			for (size_t i = 0; i < lhsSize; i++) {
				const auto& lhsElement = lhs.at(i);
				const auto& rhsElement = rhs.at(i);

				// if the current elements in each prod body are not equal,
				// the two productions are not equal
				if (lhsElement != rhsElement)
					return false;
			}

			// if we reach here, it means the two iterables are not `not-equal`
			// hence they must be equal
			return true;
		}

		// RANGES
		template<typename T>
			requires requires(T a, T b) {
			a < b;
			a > b;
		}
		inline bool withinRange(T element, T b1, T b2, bool inclusive = false) {
			if (inclusive)
				return (element >= b1 && element <= b2);

			return (element > b1 && element < b2);
		};
	}

}