#pragma once

#include <mutex>
#include <utility>

namespace tarragon
{
	// Provides synchronized access to a value
	template <typename T, typename Mtx = std::mutex>
	class Synchronized final
	{
	public:
		template <typename TLocked>
		class Locked
		{
		private:
			std::lock_guard<Mtx> _lock;
			TLocked& _inner;

		public:
			Locked() = delete;

			Locked(Mtx& mutex, TLocked& inner) noexcept
				: _lock{ mutex }
				, _inner{ inner }
			{ }

			Locked(Locked&& other) noexcept
				: _lock{ std::move(other._lock) }
				, _inner{ other._inner }
			{ }

			Locked& operator=(Locked&& other)
			{
				Locked tmp{ std::move(other) };
				std::swap(tmp, *this);
				return *this;
			}

			Locked(const Locked&) = delete;
			Locked& operator=(const Locked&) = delete;

			~Locked() = default;

			inline TLocked& operator*() noexcept { return _inner; }
			inline TLocked* operator->() noexcept { return std::addressof(_inner); }
			inline auto end() noexcept { return std::end(_inner); }
			inline auto cend() const noexcept { return std::cend(_inner); }
			inline auto begin() noexcept { return std::begin(_inner); }
			inline auto cbegin() const noexcept { return std::cbegin(_inner); }
		};

	private:
		mutable Mtx _mutex;
		T _value;

	public:
		template <typename... Args>
		Synchronized(Args... args) noexcept
			: _mutex{}
			, _value{ std::forward<Args>(args)... }
		{ }

		Locked<T> acquire() noexcept
		{
			return Locked<T>{ _mutex, _value };
		}

		Locked<T const> acquire() const noexcept
		{
			return Locked<T const>{ _mutex, _value };
		}

		template <typename TFunc>
		auto apply(TFunc func)
		{
			std::lock_guard lock{ _mutex };
			return func(_value);
		}

		template <typename TFunc>
		auto apply(TFunc func) const
		{
			std::lock_guard lock{ _mutex };
			return func(_value);
		}
	};
}
