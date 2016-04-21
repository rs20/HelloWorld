// AlgorithmRegistration.h is part of BOTH the simulation project and the Algorithm projects
#ifndef __ALGORITHM__REGISTRATION__H
#define __ALGORITHM__REGISTRATION__H

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

// There's no make_unique in c++11 (nova's gcc compiler c++), so there's no make_unique
#ifdef __linux__
template <typename T, typename... Args>
std::unique_ptr<T> make_unique_helper(std::false_type, Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
template <typename T, typename... Args>
std::unique_ptr<T> make_unique_helper(std::true_type, Args&&... args) {
	static_assert(std::extent<T>::value == 0,
		"make_unique<T[N]>() is forbidden, please use make_unique<T[]>().");

	typedef typename std::remove_extent<T>::type U;
	return std::unique_ptr<T>(new U[sizeof...(Args)]{ std::forward<Args>(args)... });
}
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
	return make_unique_helper<T>(std::is_array<T>(), std::forward<Args>(args)...);
}
#endif

class AlgorithmRegistration {
public:
	AlgorithmRegistration(std::function<unique_ptr<AbstractAlgorithm>()>);
};

// only the algorithms use this macro (the simulation is not)
// class_name is the name of the algorithm class (_313178576_A for exmaple)
// \ is used to tell the macro to continue in a new line
// ## is for cancatenation
// register_me_##class_name is the name of the object algorithm (it is unique!)
// notice that the parameters for the constructor is a function = lambda expression (that accepts void)
// dynamic allocation in here! (make_unique)
#define REGISTER_ALGORITHM(class_name) \
AlgorithmRegistration register_me_##class_name( \
	[]{return make_unique<class_name>();} \
);

#endif // __ALGORITHM__REGISTRATION__H