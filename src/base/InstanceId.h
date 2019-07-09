/*
 * Copyright (C) 1996-2019 The Squid Software Foundation and contributors
 *
 * Squid software is distributed under GPLv2+ license and includes
 * contributions from numerous individuals and organizations.
 * Please see the COPYING and CONTRIBUTORS files for details.
 */

#ifndef SQUID_BASE_INSTANCE_ID_H
#define SQUID_BASE_INSTANCE_ID_H

#include <iosfwd>

/** Identifier for class instances
 *   - unique IDs for a large number of concurrent instances, but may wrap;
 *   - useful for debugging and insecure request/response matching;
 *   - sequential IDs within a class except when wrapping;
 *   - always positive IDs.
 *  \todo: add creation/destruction debugging?
 */
template <class Class, class ValueType = unsigned int>
class InstanceId
{
public:
    typedef ValueType Value; ///< id storage type

    InstanceId() {change();}

    operator Value() const { return value; }
    bool operator ==(const InstanceId &o) const { return value == o.value; }
    bool operator !=(const InstanceId &o) const { return !(*this == o); }
    void change();

    /// prints class-pecific prefix followed by ID value; \todo: use HEX for value printing?
    std::ostream &print(std::ostream &) const;

    /// returns the class-pecific prefix
    const char * prefix() const;

public:
    Value value = Value(); ///< instance identifier

private:
    InstanceId(const InstanceId &); ///< not implemented; IDs are unique
    InstanceId& operator=(const InstanceId &); ///< not implemented
};

/// 1-parameter instantiation macro (for private use only).
/// In other sources use InstanceIdDefinitions() instead.
#define InstanceIdDefinitions2(Class, pfx) \
    template<> const char * \
    InstanceId<Class>::prefix() const { \
        return pfx; \
    } \
    template<> std::ostream & \
    InstanceId<Class>::print(std::ostream &os) const { \
        return os << pfx << value; \
    } \
    template<> void \
    InstanceId<Class>::change() { \
        static auto Last = InstanceId<Class>::Value(); \
        value = ++Last ? Last : ++Last; \
    }

/// 2-parameter instantiation macro (for private use only).
/// In other sources use InstanceIdDefinitions() instead.
#define InstanceIdDefinitions3(Class, ValueType, pfx) \
    template<> const char * \
    InstanceId<Class, ValueType>::prefix() const { \
        return pfx; \
    } \
    template<> std::ostream & \
    InstanceId<Class, ValueType>::print(std::ostream &os) const { \
        return os << pfx << value; \
    } \
    template<> void \
    InstanceId<Class, ValueType>::change() { \
        static auto Last = InstanceId<Class, ValueType>::Value(); \
        value = ++Last ? Last : ++Last; \
    }

#define GetInstanceIdMacro(_1, _2, _3, MacroName, ...) MacroName

/// convenience macro to instantiate Class-specific stuff in .cc files
#define InstanceIdDefinitions(...) GetInstanceIdMacro(__VA_ARGS__, InstanceIdDefinitions3, InstanceIdDefinitions2)(__VA_ARGS__)

/// print the id
template <class Class, class ValueType>
inline
std::ostream &operator <<(std::ostream &os, const InstanceId<Class, ValueType> &id)
{
    return id.print(os);
}

#endif /* SQUID_BASE_INSTANCE_ID_H */

