// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include "../Numbers/parseFloat.hpp"
#include "../Numbers/parseInteger.hpp"
#include "../SerializedValue.hpp"
#include "ArrayFunctions.hpp"
#include "JsonVariantData.hpp"
#include "ObjectFunctions.hpp"
#include "Slot.hpp"

namespace ARDUINOJSON_NAMESPACE {

template <typename T>
inline T variantAsIntegral(const JsonVariantData* var) {
  if (!var) return 0;
  switch (var->type) {
    case JSON_POSITIVE_INTEGER:
    case JSON_BOOLEAN:
      return T(var->content.asInteger);
    case JSON_NEGATIVE_INTEGER:
      return T(~var->content.asInteger + 1);
    case JSON_LINKED_STRING:
    case JSON_OWNED_STRING:
      return parseInteger<T>(var->content.asString);
    case JSON_FLOAT:
      return T(var->content.asFloat);
    default:
      return 0;
  }
}

inline bool variantAsBoolean(const JsonVariantData* var) {
  return variantAsIntegral<int>(var) != 0;
}

// T = float/double
template <typename T>
inline T variantAsFloat(const JsonVariantData* var) {
  if (!var) return 0;
  switch (var->type) {
    case JSON_POSITIVE_INTEGER:
    case JSON_BOOLEAN:
      return static_cast<T>(var->content.asInteger);
    case JSON_NEGATIVE_INTEGER:
      return -static_cast<T>(var->content.asInteger);
    case JSON_LINKED_STRING:
    case JSON_OWNED_STRING:
      return parseFloat<T>(var->content.asString);
    case JSON_FLOAT:
      return static_cast<T>(var->content.asFloat);
    default:
      return 0;
  }
}

inline const char* variantAsString(const JsonVariantData* var) {
  if (!var) return 0;
  if (var &&
      (var->type == JSON_LINKED_STRING || var->type == JSON_OWNED_STRING))
    return var->content.asString;
  else
    return 0;
}

inline JsonArrayData* variantAsArray(JsonVariantData* var) {
  if (var && var->type == JSON_ARRAY)
    return &var->content.asArray;
  else
    return 0;
}

inline const JsonArrayData* variantAsArray(const JsonVariantData* var) {
  if (var && var->type == JSON_ARRAY)
    return &var->content.asArray;
  else
    return 0;
}

inline JsonObjectData* variantAsObject(JsonVariantData* var) {
  if (var && var->type == JSON_OBJECT)
    return &var->content.asObject;
  else
    return 0;
}

inline const JsonObjectData* variantAsObject(const JsonVariantData* var) {
  if (var && var->type == JSON_OBJECT)
    return &var->content.asObject;
  else
    return 0;
}

inline bool variantSetBoolean(JsonVariantData* var, bool value) {
  if (!var) return false;
  var->type = JSON_BOOLEAN;
  var->content.asInteger = static_cast<JsonUInt>(value);
  return true;
}

inline bool variantSetFloat(JsonVariantData* var, JsonFloat value) {
  if (!var) return false;
  var->type = JSON_FLOAT;
  var->content.asFloat = value;
  return true;
}

template <typename T>
inline bool variantSetSignedInteger(JsonVariantData* var, T value) {
  if (!var) return false;
  if (value >= 0) {
    var->type = JSON_POSITIVE_INTEGER;
    var->content.asInteger = static_cast<JsonUInt>(value);
  } else {
    var->type = JSON_NEGATIVE_INTEGER;
    var->content.asInteger = ~static_cast<JsonUInt>(value) + 1;
  }
  return true;
}

inline bool variantSetSignedInteger(JsonVariantData* var, JsonUInt value) {
  if (!var) return false;
  var->type = JSON_POSITIVE_INTEGER;
  var->content.asInteger = static_cast<JsonUInt>(value);
  return true;
}

inline bool variantSetLinkedRaw(JsonVariantData* var,
                                SerializedValue<const char*> value) {
  if (!var) return false;
  var->type = JSON_LINKED_RAW;
  var->content.asRaw.data = value.data();
  var->content.asRaw.size = value.size();
  return true;
}

template <typename T>
inline bool variantSetOwnedRaw(JsonVariantData* var, SerializedValue<T> value,
                               MemoryPool* pool) {
  if (!var) return false;
  const char* dup = makeString(value.data(), value.size()).save(pool);
  if (dup) {
    var->type = JSON_OWNED_RAW;
    var->content.asRaw.data = dup;
    var->content.asRaw.size = value.size();
    return true;
  } else {
    var->type = JSON_NULL;
    return false;
  }
}

template <typename T>
inline bool variantSetString(JsonVariantData* var, T value, MemoryPool* pool) {
  if (!var) return false;
  const char* dup = value.save(pool);
  if (dup) {
    var->type = JSON_OWNED_STRING;
    var->content.asString = dup;
    return true;
  } else {
    var->type = JSON_NULL;
    return false;
  }
}

inline bool variantSetString(JsonVariantData* var, const char* value) {
  if (!var) return false;
  var->type = JSON_LINKED_STRING;
  var->content.asString = value;
  return true;
}

inline bool variantSetString(JsonVariantData* var, const char* value,
                             MemoryPool* pool) {
  return variantSetString(var, makeString(const_cast<char*>(value)), pool);
}

inline void variantSetNull(JsonVariantData* var) {
  if (var) var->type = JSON_NULL;
}

inline JsonArrayData* variantToArray(JsonVariantData* var) {
  if (!var) return 0;
  var->type = JSON_ARRAY;
  var->content.asArray.head = 0;
  var->content.asArray.tail = 0;
  return &var->content.asArray;
}

inline JsonObjectData* variantToObject(JsonVariantData* var) {
  if (!var) return 0;
  var->type = JSON_OBJECT;
  var->content.asObject.head = 0;
  var->content.asObject.tail = 0;
  return &var->content.asObject;
}

inline bool variantCopy(JsonVariantData* dst, const JsonVariantData* src,
                        MemoryPool* pool) {
  if (!dst) return false;
  if (!src) {
    dst->type = JSON_NULL;
    return true;
  }
  switch (src->type) {
    case JSON_ARRAY:
      return arrayCopy(variantToArray(dst), &src->content.asArray, pool);
    case JSON_OBJECT:
      return objectCopy(variantToObject(dst), &src->content.asObject, pool);
    case JSON_OWNED_STRING:
      return variantSetString(dst, src->content.asString, pool);
    case JSON_OWNED_RAW:
      return variantSetOwnedRaw(
          dst,
          serialized(const_cast<char*>(src->content.asRaw.data),
                     src->content.asRaw.size),
          pool);
    default:
      *dst = *src;
      return true;
  }
}

inline bool variantIsInteger(const JsonVariantData* var) {
  return var && (var->type == JSON_POSITIVE_INTEGER ||
                 var->type == JSON_NEGATIVE_INTEGER);
}

inline bool variantIsFloat(const JsonVariantData* var) {
  return var &&
         (var->type == JSON_FLOAT || var->type == JSON_POSITIVE_INTEGER ||
          var->type == JSON_NEGATIVE_INTEGER);
}

inline bool variantIsString(const JsonVariantData* var) {
  return var &&
         (var->type == JSON_LINKED_STRING || var->type == JSON_OWNED_STRING);
}

inline bool variantIsArray(const JsonVariantData* var) {
  return var && var->type == JSON_ARRAY;
}

inline bool variantIsObject(const JsonVariantData* var) {
  return var && var->type == JSON_OBJECT;
}

inline bool variantIsNull(const JsonVariantData* var) {
  return var == 0 || var->type == JSON_NULL;
}

inline bool variantEquals(const JsonVariantData* a, const JsonVariantData* b) {
  if (a == b) return true;
  if (!a || !b) return false;
  if (a->type != b->type) return false;

  switch (a->type) {
    case JSON_LINKED_RAW:
    case JSON_OWNED_RAW:
    case JSON_LINKED_STRING:
    case JSON_OWNED_STRING:
      return !strcmp(a->content.asString, b->content.asString);

    case JSON_BOOLEAN:
    case JSON_POSITIVE_INTEGER:
    case JSON_NEGATIVE_INTEGER:
      return a->content.asInteger == b->content.asInteger;

    case JSON_ARRAY:
      return arrayEquals(&a->content.asArray, &b->content.asArray);

    case JSON_OBJECT:
      return objectEquals(&a->content.asObject, &b->content.asObject);

    case JSON_FLOAT:
      return a->content.asFloat == b->content.asFloat;

    case JSON_NULL:
    default:
      return true;
  }
}
}  // namespace ARDUINOJSON_NAMESPACE
