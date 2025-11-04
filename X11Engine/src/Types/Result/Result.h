#ifndef RESULT_H
#define RESULT_H

#include <variant>

template <typename ResultType, typename ErrorType>
class Result {
   public:
    Result(const ResultType& result) : is_error(false), result(result) {}
    Result(const ErrorType& result) : is_error(true), result(result) {}

    ResultType getResult() const {
        if (is_error) {
            throw std::bad_variant_access();
        }

        return std::get<ResultType>(result);
    }

    ResultType getResultOrDefault(const ResultType& default_value) const {
        if (is_error) return default_value;

        return std::get<ResultType>(result);
    }

    ErrorType getError() const {
        if (is_error == false) {
            throw std::bad_variant_access();
        }

        return std::get<ErrorType>(result);
    }

    bool isOk() const { return !is_error; }

    bool isError() const { return is_error; }

   private:
    bool is_error;

    std::variant<ResultType, ErrorType> result;
};

#endif  // RESULT_H