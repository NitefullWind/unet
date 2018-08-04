#ifndef MUDUO_BASE_TIMESTAMPS_H
#define MUDUO_BASE_TIMESTAMPS_H

#include <string>

namespace muduo
{

class Timestamp
{
public:
	Timestamp():
		_microSecondsSinceEpoch(0)
	{
	}

	explicit Timestamp(int64_t microSecondsSinceEpochArg):
		_microSecondsSinceEpoch(microSecondsSinceEpochArg)
	{
	}

	void swap(Timestamp& that)
	{
		std::swap(_microSecondsSinceEpoch, that._microSecondsSinceEpoch);
	}

	std::string toString() const;
	std::string toFormattedString(bool showMicroseconds = true) const;

	bool valid() const { return _microSecondsSinceEpoch > 0; }

	int64_t microSecondsSinceEpoch() const { return _microSecondsSinceEpoch; }
	time_t secondsSinceEpoch() const { return static_cast<time_t>(_microSecondsSinceEpoch / kMicroSecondsPerSecond); }

	static Timestamp now();
	static Timestamp invalid() { return Timestamp(); }

	static Timestamp fromUnixTime(time_t t) { return fromUnixTime(t, 0); }
	static Timestamp fromUnixTime(time_t t, int microseconds) {
		return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
	}

	static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
	int64_t _microSecondsSinceEpoch;
};

inline bool operator < (Timestamp lhs, Timestamp rhs)
{
	return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator == (Timestamp lhs, Timestamp rhs)
{
	return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeDifference(Timestamp high, Timestamp low)
{
	int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
	return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp timestamp, double seconds)
{
	int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
	return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}

#endif // MUDUO_BASE_TIMESTAMPS_H
