// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

namespace ARDUINOJSON_NAMESPACE {
class JsonArray;
class JsonObject;
class JsonVariant;

// A metafunction that returns the type of the value returned by
// JsonVariant::to<T>()
template <typename T>
struct JsonVariantTo {};

template <>
struct JsonVariantTo<JsonArray> {
  typedef JsonArray type;
};
template <>
struct JsonVariantTo<JsonObject> {
  typedef JsonObject type;
};
template <>
struct JsonVariantTo<JsonVariant> {
  typedef JsonVariant type;
};

}  // namespace ARDUINOJSON_NAMESPACE
