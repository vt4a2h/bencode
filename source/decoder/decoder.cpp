//
// MIT License
//
// Copyright (c) 2022-present Vitaly Fanaskov
//
// bencode -- A simple library for decoding bencoded data
// Project home: https://github.com/vt4a2h/bencode
//
// See LICENSE file for the further details.
//
#include <bencode/decoder/decoder.hpp>
#include <bencode/details/delimiters.hpp>

#include <charconv>
#include <stack>
#include <functional>

namespace bencode {

using namespace details;

template <class E>
class Stack : public std::stack<E> {
public:
    [[nodiscard]]
    E take() {
        auto e = std::move(*(std::prev(std::end(this->c))));
        this->pop();
        return e;
    }
};

using ElementStack = Stack<Element>;
using RawDataStack = Stack<std::string_view::iterator>;

inline static constexpr bool isNonZeroDigit(char c) {
    switch (c) {
        case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': return true;
        default: return false;
    }
}

inline static constexpr bool isDigit(char c) {
    return c == '0' || isNonZeroDigit(c);
}

inline static constexpr bool allDigits(auto begin, auto end) {
    return std::all_of(begin, end, isDigit);
}

inline static constexpr bool validStrLenFormat(auto begin, auto end) {
    switch (std::distance(begin, end)) {
        case 0: return false;
        case 1: return isDigit(*begin);
        default: return isNonZeroDigit(*begin) && allDigits(std::next(begin), end);
    }
}

inline static constexpr bool validIntFormat(auto begin, auto end) {
    switch (std::distance(begin, end)) {
        case 0: return false;
        case 1: return isDigit(*begin);
        default:
            switch (*begin) {
                case '-':
                    return isNonZeroDigit(*std::next(begin)) && allDigits(std::next(begin, 2), end);
                default:
                    return isNonZeroDigit(*begin) && allDigits(std::next(begin), end);
            }
    }
}

Error makeDecodingError(std::string description) {
    return {ErrorType::DecodingError, std::move(description)};
}

using SvIt = std::string_view::iterator;
using SvItElem = std::pair<SvIt, Element>;

static Expected<SvItElem> extractIntElement(auto begin, auto end) {
    auto strEnd = std::find(begin, end, EndDelim);

    if (strEnd == end)
        return makeDecodingError("wrong end character of the integer element");

    if (strEnd == begin)
        return makeDecodingError("no characters between int delimiters");

    if (!validIntFormat(begin, strEnd))
        return makeDecodingError("invalid integer format");

    Type::Int result{};
    auto [ptr, errorCode] = std::from_chars(&*begin, &*strEnd, result);
    if (errorCode != std::errc() || ptr != &*strEnd)
        return makeDecodingError("wrong integer");

    return std::make_pair(strEnd, Element{result});
}

static Expected<SvItElem> extractStringElement(auto begin, auto end) {
    auto lenEnd = std::find(begin, end, StringDelim);

    if (lenEnd == end)
        return makeDecodingError("string delimiter not found");

      if (!validStrLenFormat(begin, lenEnd))
        return makeDecodingError("invalid string len format");

    Type::Int bytesCount{};
    auto [ptr, errorCode] = std::from_chars(&*begin, &*lenEnd, bytesCount);
    if (errorCode != std::errc() || ptr != &*lenEnd)
        return makeDecodingError("wrong string len");

    auto dataBegin = std::next(lenEnd);
    if (std::distance(dataBegin, end) < bytesCount)
        return makeDecodingError("not enough bytes to read");

    auto dataEnd = std::next(dataBegin, bytesCount);

    return SvItElem(dataEnd, Type::String{dataBegin, dataEnd});
}

static std::optional<Error> addElementToStack(auto extractor, auto &stack, auto &it, auto end) {
    if (auto result = std::invoke(extractor, it, end); result.hasValue()) {
        auto &&[endIt, element] = result.value();
        stack.push(std::move(element));
        it = std::prev(endIt); // +1 in the loop
    } else {
        return result.error();
    }

    return std::nullopt;
}

static void createNewList(ElementStack &stack) { stack.emplace<Type::List>({}); }
static void createNewDict(ElementStack &stack) { stack.emplace<Type::Dict>({}); }

static void addListElement(Element &currentElement, Element &previousElement,
                           RawDataStack &rawDataStack, std::string_view::iterator rawDataEnd)
{
    auto &list = previousElement.as<Type::List>();
    list.emplace_back(std::move(currentElement));
    list.back().raw = std::string_view(rawDataStack.take(), std::next(rawDataEnd));
}

static std::optional<Error> addDictElement(Element &currentElement, ElementStack &stack,
                                           RawDataStack &rawDataStack, std::string_view::iterator rawDataEnd)
{
    if (!currentElement.is<Type::String>())
        return makeDecodingError("wrong key type");

    currentElement.raw = std::string_view(rawDataStack.take(), std::next(rawDataEnd));
    stack.push(std::move(currentElement).as<Type::String>());
    stack.emplace<Type::Null>({});

    return std::nullopt;
}

static std::optional<Error> addDictKeyValueElement(Element &currentElement, ElementStack &stack,
                                                   RawDataStack &rawDataStack, std::string_view::iterator rawDataEnd)
{
    stack.pop();
    if (stack.empty())
        return makeDecodingError("inconsistent dictionary");

    auto &previousElem = stack.top();
    if (!previousElem.is<Type::String>())
        return makeDecodingError("wrong key type");

    currentElement.raw = std::string_view(rawDataStack.take(), std::next(rawDataEnd));
    auto keyValuePair = std::make_pair(std::move(previousElem).as<Type::String>(), std::move(currentElement));
    stack.pop();

    if (stack.empty() || !stack.top().is<Type::Dict>())
        return makeDecodingError("no dict to add key");

    auto &dict = stack.top().as<Type::Dict>();
    dict.insert(std::move(keyValuePair));

    return std::nullopt;
}

static std::optional<Error> addElement(Elements &elements, ElementStack &stack, RawDataStack &rawDataStack) {
    if (stack.empty())
        return makeDecodingError("wrong end delimiter");

    auto currentElem = stack.take();
    auto rawDataEnd = rawDataStack.take();

    if (stack.empty()) {
        elements.emplace_back(std::move(currentElem));
        elements.back().raw = std::string_view(rawDataStack.take(), std::next(rawDataEnd));
    } else {
        auto &previousElem = stack.top();

        switch (details::indexType(previousElem)) {
            break; case details::BaseElementIndexType::List:
                addListElement(currentElem, previousElem, rawDataStack, rawDataEnd);

            break; case details::BaseElementIndexType::Dict:
                if (auto err = addDictElement(currentElem, stack, rawDataStack, rawDataEnd); err.has_value())
                    return err;

            break; case details::BaseElementIndexType::Null:
                if (auto err = addDictKeyValueElement(currentElem, stack, rawDataStack, rawDataEnd); err.has_value())
                    return err;

            break; default: return makeDecodingError("invalid element type");
        }
    }

    return std::nullopt;
}

// NOTE: the format is very simple and the data is also not a stream.
//       I don't think that would make much sense to use a separate
//       tokenizer here.
static Expected<Elements> decodeImpl(SvIt it, SvIt end) {
    ElementStack stack;
    Elements elements;

    RawDataStack rawDataStack;

    for (; it != end; ++it) {
        switch (*it) {
            break; case IntStartDelim:
                rawDataStack.push(it);
                if (auto err = addElementToStack(extractIntElement<SvIt, SvIt>, stack, ++it, end); err.has_value())
                    return err.value();

            break; case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                rawDataStack.push(it);
                if (auto err = addElementToStack(extractStringElement<SvIt, SvIt>, stack, it, end); err.has_value())
                    return err.value();

            [[fallthrough]]; case EndDelim:
                rawDataStack.push(it);
                if (auto err = addElement(elements, stack, rawDataStack); err.has_value())
                    return err.value();

            break; case ListStartDelim:
                rawDataStack.push(it);
                createNewList(stack);

            break; case DictStartDelim:
                rawDataStack.push(it);
                createNewDict(stack);

            break; default:
                return makeDecodingError("unexpected element");
        }
    }

    return stack.empty() ? Expected<Elements>(std::move(elements)) : makeDecodingError("inconsistent elements");
}

Expected<Elements> decode(std::string_view input) {
    return decodeImpl(std::begin(input), std::end(input));
}

} // namespace bencode