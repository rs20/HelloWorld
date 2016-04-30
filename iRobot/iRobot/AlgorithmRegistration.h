#include <functional>
#include <memory>

#ifndef __cpp_lib_make_unique
#include <type_traits>
#include <utility>

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

#define REGISTER_ALGORITHM(class_name) \
AlgorithmRegistration register_me_##class_name( \
	[]{return make_unique<class_name>();} \
);
