//
// Copyright 2014 - 2018 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include <tuple>
#include <vector>
#include <iterator>
#include <iostream>

#include "comms/comms.h"

static_assert(0U < comms::version(), "Invalid version definition");
static_assert(0U < COMMS_MAKE_VERSION(0, 29, 0), "Invalid version definition");

enum MessageType {
    MessageType1,
    UnusedValue1,
    MessageType2,
    UnusedValue2,
    UnusedValue3,
    MessageType3,
    MessageType4,
    MessageType5,
    MessageType6,
    MessageType7,
    MessageType8,
    MessageType9,

    MessageType90 = 90
};

template <typename TTraits>
using TestMessageBase = comms::Message<TTraits>;


template <typename TField>
using FieldsMessage1 =
    std::tuple<
        comms::field::IntValue<TField, std::uint16_t>
    >;

template <typename TMessage>
class Message1 : public
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType1>,
            comms::option::FieldsImpl<FieldsMessage1<typename TMessage::Field> >,
            comms::option::MsgType<Message1<TMessage> >,
            comms::option::HasName
        >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType1>,
            comms::option::FieldsImpl<FieldsMessage1<typename TMessage::Field> >,
            comms::option::MsgType<Message1<TMessage> >,
            comms::option::HasName
        >;
public:
    static const bool AreFieldsVersionDependent = Base::areFieldsVersionDependent();
    static_assert(!AreFieldsVersionDependent, "Fields mustn't be version dependent");

    COMMS_MSG_FIELDS_ACCESS(value1);

    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 2U, "Wrong serialisation length");
    static_assert(MsgMaxLen == 2U, "Wrong serialisation length");

    Message1() = default;

    virtual ~Message1() noexcept = default;

    static const char* doName()
    {
        return "Message1";
    }
};

template <typename TMessage>
class Message2 : public
    comms::MessageBase<
        TMessage,
        comms::option::StaticNumIdImpl<MessageType2>,
        comms::option::ZeroFieldsImpl,
        comms::option::MsgType<Message2<TMessage> >,
        comms::option::HasName
    >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType2>,
            comms::option::ZeroFieldsImpl,
            comms::option::MsgType<Message2<TMessage> >,
            comms::option::HasName
        >;

public:
    virtual ~Message2() noexcept = default;

    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 0U, "Wrong serialisation length");
    static_assert(MsgMaxLen == 0U, "Wrong serialisation length");

    static const char* doName()
    {
        return "Message2";
    }
};

template <typename TField>
using Message3Fields =
    std::tuple<
        comms::field::IntValue<TField, std::uint32_t>,
        comms::field::IntValue<
            TField,
            std::int16_t,
            comms::option::FixedLength<1>,
            comms::option::ValidNumValueRange<-120, 120>,
            comms::option::DefaultNumValue<127> >, // invalid upon creation
        comms::field::BitmaskValue<TField, comms::option::FixedLength<2> >,
        comms::field::BitmaskValue<TField, comms::option::FixedLength<3> >
    >;

template <typename TMessage>
class Message3 : public
    comms::MessageBase<
        TMessage,
        comms::option::StaticNumIdImpl<MessageType3>,
        comms::option::FieldsImpl<Message3Fields<typename TMessage::Field> >,
        comms::option::MsgType<Message3<TMessage> >,
        comms::option::HasName
    >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType3>,
            comms::option::FieldsImpl<Message3Fields<typename TMessage::Field> >,
            comms::option::MsgType<Message3<TMessage> >,
            comms::option::HasName
        >;
public:
    static const bool AreFieldsVersionDependent = Base::areFieldsVersionDependent();
    static_assert(!AreFieldsVersionDependent, "Fields mustn't be version dependent");

    COMMS_MSG_FIELDS_ACCESS(value1, value2, value3, value4);

    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static const std::size_t MsgMinLen_0_1 = Base::template doMinLengthUntil<FieldIdx_value2>();
    static const std::size_t MsgMaxLen_0_1 = Base::template doMaxLengthUntil<FieldIdx_value2>();
    static const std::size_t MsgMinLen_0_2 = Base::template doMinLengthUntil<FieldIdx_value3>();
    static const std::size_t MsgMaxLen_0_2 = Base::template doMaxLengthUntil<FieldIdx_value3>();
    static const std::size_t MsgMinLen_1_4 = Base::template doMinLengthFrom<FieldIdx_value2>();
    static const std::size_t MsgMaxLen_1_4 = Base::template doMaxLengthFrom<FieldIdx_value2>();
    static const std::size_t MsgMinLen_1_3 = Base::template doMinLengthFromUntil<FieldIdx_value2, FieldIdx_value4>();
    static const std::size_t MsgMaxLen_1_3 = Base::template doMaxLengthFromUntil<FieldIdx_value2, FieldIdx_value4>();


    static_assert(MsgMinLen == 10U, "Wrong serialisation length");
    static_assert(MsgMaxLen == 10U, "Wrong serialisation length");
    static_assert(MsgMinLen_0_1 == 4U, "Wrong serialisation length");
    static_assert(MsgMaxLen_0_1 == 4U, "Wrong serialisation length");
    static_assert(MsgMinLen_0_2 == 5U, "Wrong serialisation length");
    static_assert(MsgMaxLen_0_2 == 5U, "Wrong serialisation length");
    static_assert(MsgMinLen_1_4 == 6U, "Wrong serialisation length");
    static_assert(MsgMaxLen_1_4 == 6U, "Wrong serialisation length");
    static_assert(MsgMinLen_1_3 == 3U, "Wrong serialisation length");
    static_assert(MsgMaxLen_1_3 == 3U, "Wrong serialisation length");

    Message3() = default;

    virtual ~Message3() noexcept = default;

    static const char* doName()
    {
        return "Message3";
    }
};

template <typename TField>
using Message4Fields =
    std::tuple<
        comms::field::BitmaskValue<
            TField,
            comms::option::FixedLength<1> >,
        comms::field::Optional<
            comms::field::IntValue<
                TField,
                std::uint16_t
            >
        >
    >;

template <typename TMessage>
class Message4 : public
    comms::MessageBase<
        TMessage,
        comms::option::StaticNumIdImpl<MessageType4>,
        comms::option::FieldsImpl<Message4Fields<typename TMessage::Field> >,
        comms::option::MsgType<Message4<TMessage> >,
        comms::option::HasDoRefresh,
        comms::option::HasName
    >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType4>,
            comms::option::FieldsImpl<Message4Fields<typename TMessage::Field> >,
            comms::option::MsgType<Message4<TMessage> >,
            comms::option::HasDoRefresh,
            comms::option::HasName
        >;
public:
    static const bool AreFieldsVersionDependent = Base::areFieldsVersionDependent();
    static_assert(!AreFieldsVersionDependent, "Fields mustn't be version dependent");

    COMMS_MSG_FIELDS_ACCESS(value1, value2);

    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static const std::size_t MsgMinLen_1_2 = Base::template doMinLengthFrom<FieldIdx_value2>();
    static const std::size_t MsgMaxLen_1_2 = Base::template doMaxLengthFrom<FieldIdx_value2>();

    static_assert(MsgMinLen == 1U, "Wrong serialisation length");
    static_assert(MsgMaxLen == 3U, "Wrong serialisation length");
    static_assert(MsgMinLen_1_2 == 0U, "Wrong serialisation length");
    static_assert(MsgMaxLen_1_2 == 2U, "Wrong serialisation length");

    Message4()
    {
        auto& optField = field_value2();
        optField.setMissing();
    }

    virtual ~Message4() noexcept = default;

    template <typename TIter>
    comms::ErrorStatus doRead(TIter& iter, std::size_t len)
    {
        auto es = Base::template doReadUntilAndUpdateLen<FieldIdx_value2>(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto expectedNextFieldMode = comms::field::OptionalMode::Missing;
        if ((field_value1().value() & 0x1) != 0) {
            expectedNextFieldMode = comms::field::OptionalMode::Exists;
        }

        field_value2().setMode(expectedNextFieldMode);
        return Base::template doReadFrom<FieldIdx_value2>(iter, len);
    }

    bool doRefresh()
    {
        auto& mask = field_value1();
        auto expectedNextFieldMode = comms::field::OptionalMode::Missing;
        if ((mask.value() & 0x1) != 0) {
            expectedNextFieldMode = comms::field::OptionalMode::Exists;
        }

        auto& optField = field_value2();
        if (optField.getMode() == expectedNextFieldMode) {
            return false;
        }

        optField.setMode(expectedNextFieldMode);
        return true;
    }

    static const char* doName()
    {
        return "Message4";
    }
};


template <typename TField>
using FieldsMessage5 =
    std::tuple<
        comms::field::IntValue<TField, std::uint16_t>,
        comms::field::IntValue<TField, std::int8_t>
    >;

template <typename TMessage>
class Message5 : public
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType5>,
            comms::option::FieldsImpl<FieldsMessage5<comms::Field<comms::option::BigEndian> > >,
            comms::option::MsgType<Message5<TMessage> >,
            comms::option::HasName
        >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType5>,
            comms::option::FieldsImpl<FieldsMessage5<comms::Field<comms::option::BigEndian> > >,
            comms::option::MsgType<Message5<TMessage> >,
            comms::option::HasName
        >;
public:
    static const bool AreFieldsVersionDependent = Base::areFieldsVersionDependent();
    static_assert(!AreFieldsVersionDependent, "Fields mustn't be version dependent");

    COMMS_MSG_FIELDS_ACCESS(value1, value2);

    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 3U, "Wrong serialisation length");
    static_assert(MsgMaxLen == 3U, "Wrong serialisation length");

    Message5() = default;

    virtual ~Message5() noexcept = default;

    static const char* doName()
    {
        return "Message5";
    }
};

template <typename TField>
struct Message6Fields
{
    class field : public
        comms::field::Bundle<
            TField,
            std::tuple<
                comms::field::BitmaskValue<TField, comms::option::FixedLength<1> >,
                comms::field::Optional<
                    comms::field::IntValue<TField, std::uint16_t>,
                    comms::option::MissingByDefault
                >
            >,
            comms::option::HasCustomRead,
            comms::option::HasCustomRefresh
        >
    {
        using Base =
            comms::field::Bundle<
                TField,
                std::tuple<
                    comms::field::BitmaskValue<TField, comms::option::FixedLength<1> >,
                    comms::field::Optional<
                        comms::field::IntValue<TField, std::uint16_t>,
                        comms::option::MissingByDefault
                    >
                >,
                comms::option::HasCustomRead,
                comms::option::HasCustomRefresh
            >;

    public:
        COMMS_FIELD_MEMBERS_ACCESS(mask, val);

        template <typename TIter>
        comms::ErrorStatus read(TIter& iter, std::size_t len)
        {
            auto es = field_mask().read(iter, len);
            if (es != comms::ErrorStatus::Success) {
                return es;
            }

            comms::field::OptionalMode mode = comms::field::OptionalMode::Missing;
            if ((field_mask().value() & 0x1) != 0) {
                mode = comms::field::OptionalMode::Exists;
            }

            field_val().setMode(mode);
            return field_val().read(iter, len - field_mask().length());
        }

        bool refresh()
        {
            comms::field::OptionalMode mode = comms::field::OptionalMode::Missing;
            if ((field_mask().value() & 0x1) != 0) {
                mode = comms::field::OptionalMode::Exists;
            }

            if (mode == field_val().getMode()) {
                return false;
            }

            field_val().setMode(mode);
            return true;
        }
    };

    using All = std::tuple<
        field
    >;

};

template <typename TMessage>
class Message6 : public
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType6>,
            comms::option::FieldsImpl<typename Message6Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message6<TMessage> >,
            comms::option::HasName
        >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType6>,
            comms::option::FieldsImpl<typename Message6Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message6<TMessage> >,
            comms::option::HasName
        >;
public:
    static const bool AreFieldsVersionDependent = Base::areFieldsVersionDependent();
    static_assert(!AreFieldsVersionDependent, "Fields mustn't be version dependent");

    COMMS_MSG_FIELDS_ACCESS(value1);

    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 1U, "Wrong serialisation length");
    static_assert(MsgMaxLen == 3U, "Wrong serialisation length");

    Message6() = default;

    ~Message6() noexcept = default;

    static const char* doName()
    {
        return "Message6";
    }
};

template <typename TField>
struct Message7Fields
{
    using field1 =
            comms::field::IntValue<TField, std::uint16_t>;

    using field2 =
        comms::field::Optional<
            comms::field::IntValue<TField, std::uint16_t>,
            comms::option::ExistsByDefault,
            comms::option::ExistsBetweenVersions<5, 10>
        >;

    static_assert(field2::isVersionDependent(), "field2 must be version dependent");

    using All = std::tuple<
        field1,
        field2
    >;

};

template <typename TMessage>
class Message7 : public
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType7>,
            comms::option::FieldsImpl<typename Message7Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message7<TMessage> >,
            comms::option::HasName
        >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType7>,
            comms::option::FieldsImpl<typename Message7Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message7<TMessage> >,
            comms::option::HasName
        >;
public:

    static const bool AreFieldsVersionDependent = Base::areFieldsVersionDependent();
    static_assert(AreFieldsVersionDependent, "Fields must be version dependent");

    COMMS_MSG_FIELDS_ACCESS(value1, value2);

    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 2U, "Wrong serialisation length");
    static_assert(MsgMaxLen == 4U, "Wrong serialisation length");

    Message7() = default;

    ~Message7() noexcept = default;

    static const char* doName()
    {
        return "Message7";
    }
};

template <typename TField>
struct Message8Fields
{
    enum class Field1Val : std::uint16_t
    {
        V1 = 0,
        V2 = 128
    };

    using field1 =
            comms::field::EnumValue<
                TField, 
                Field1Val,
                comms::option::VarLength<1, 2>
            >;

    using All = std::tuple<
        field1
    >;

};

template <typename TMessage>
class Message8 : public
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType8>,
            comms::option::FieldsImpl<typename Message8Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message8<TMessage> >,
            comms::option::HasName
        >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType8>,
            comms::option::FieldsImpl<typename Message8Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message8<TMessage> >,
            comms::option::HasName
        >;
public:

    static const bool AreFieldsVersionDependent = Base::areFieldsVersionDependent();
    static_assert(!AreFieldsVersionDependent, "Fields not must be version dependent");

    COMMS_MSG_FIELDS_ACCESS(value1);

    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 1U, "Wrong serialisation length");
    static_assert(MsgMaxLen == 2U, "Wrong serialisation length");

    Message8() = default;

    ~Message8() noexcept = default;

    static const char* doName()
    {
        return "Message8";
    }
};

template <typename TField>
struct Message9Fields
{
    class field1 : public
        comms::field::Bundle<
            TField,
            std::tuple<
                comms::field::IntValue<TField, std::uint8_t>,
                comms::field::String<TField>
            >,
            comms::option::RemLengthMemberField<0>
        >
    {
#ifdef COMMS_MUST_DEFINE_BASE
        using Base =
            comms::field::Bundle<
                TField,
                std::tuple<
                    comms::field::IntValue<TField, std::uint8_t>,
                    comms::field::String<TField>
                >,
                comms::option::RemLengthMemberField<0>
            >;
#endif // #ifdef COMMS_MUST_DEFINE_BASE
    public:
        COMMS_FIELD_MEMBERS_ACCESS(len, str);
    };

    using All = std::tuple<
        field1
    >;

};

template <typename TMessage>
class Message9 : public
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType9>,
            comms::option::FieldsImpl<typename Message9Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message9<TMessage> >,
            comms::option::HasName
        >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType9>,
            comms::option::FieldsImpl<typename Message9Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message9<TMessage> >,
            comms::option::HasName
        >;
public:

    static const bool AreFieldsVersionDependent = Base::areFieldsVersionDependent();
    static_assert(!AreFieldsVersionDependent, "Fields not must be version dependent");

    COMMS_MSG_FIELDS_ACCESS(f1);

    static const std::size_t MsgMinLen = Base::doMinLength();
    static_assert(MsgMinLen == 1U, "Wrong serialisation length");

    Message9() = default;

    ~Message9() noexcept = default;

    static const char* doName()
    {
        return "Message9";
    }
};



template <typename TField>
struct Message90_1Fields
{
    using typeField =
            comms::field::IntValue<
                TField, 
                std::uint8_t,
                comms::option::ValidNumValue<0>,
                comms::option::FailOnInvalid<>
            >;

    using field1 = comms::field::IntValue<TField, std::uint32_t>;

    using All = std::tuple<
        typeField,
        field1
    >;

};

template <typename TMessage>
class Message90_1 : public
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType90>,
            comms::option::FieldsImpl<typename Message90_1Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message90_1<TMessage> >,
            comms::option::HasName
        >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType90>,
            comms::option::FieldsImpl<typename Message90_1Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message90_1<TMessage> >,
            comms::option::HasName
        >;
public:

    static const bool AreFieldsVersionDependent = Base::areFieldsVersionDependent();
    static_assert(!AreFieldsVersionDependent, "Fields not must be version dependent");

    COMMS_MSG_FIELDS_ACCESS(type, value1);

    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 5U, "Wrong serialisation length");
    static_assert(MsgMaxLen == 5U, "Wrong serialisation length");

    Message90_1() = default;

    ~Message90_1() noexcept = default;

    static const char* doName()
    {
        return "Message90 (1)";
    }
};

template <typename TField>
struct Message90_2Fields
{
    using typeField =
            comms::field::IntValue<
                TField, 
                std::uint8_t,
                comms::option::ValidNumValue<1>,
                comms::option::FailOnInvalid<>
            >;

    using field1 = comms::field::IntValue<TField, std::uint8_t>;

    using All = std::tuple<
        typeField,
        field1
    >;

};

template <typename TMessage>
class Message90_2 : public
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType90>,
            comms::option::FieldsImpl<typename Message90_2Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message90_2<TMessage> >,
            comms::option::HasName
        >
{
    using Base =
        comms::MessageBase<
            TMessage,
            comms::option::StaticNumIdImpl<MessageType90>,
            comms::option::FieldsImpl<typename Message90_2Fields<typename TMessage::Field>::All>,
            comms::option::MsgType<Message90_2<TMessage> >,
            comms::option::HasName
        >;
public:

    static const bool AreFieldsVersionDependent = Base::areFieldsVersionDependent();
    static_assert(!AreFieldsVersionDependent, "Fields not must be version dependent");

    COMMS_MSG_FIELDS_ACCESS(type, value1);

    static const std::size_t MsgMinLen = Base::doMinLength();
    static const std::size_t MsgMaxLen = Base::doMaxLength();
    static_assert(MsgMinLen == 2U, "Wrong serialisation length");
    static_assert(MsgMaxLen == 2U, "Wrong serialisation length");

    Message90_2() = default;

    ~Message90_2() noexcept = default;

    static const char* doName()
    {
        return "Message90 (2)";
    }
};


template <typename TMessage>
using AllMessages =
    std::tuple<
        Message1<TMessage>,
        Message2<TMessage>,
        Message3<TMessage>
    >;

namespace details
{

struct HasLengthTag {};
struct NoLengthTag {};

template <typename TFrame, typename TMsg>
void verifyFrameLengthIfPossible(TFrame& frame, const TMsg& msg, std::size_t expLength, HasLengthTag)
{
    TS_ASSERT_EQUALS(expLength, frame.length(msg));
}

template <typename TFrame, typename TMsg>
void verifyFrameLengthIfPossible(TFrame& frame, const TMsg& msg, std::size_t expLength, NoLengthTag)
{
    static_cast<void>(frame);
    static_cast<void>(msg);
    static_cast<void>(expLength);
    // nothing to do
}

}

template <typename TFrame, typename TMsg>
void verifyFrameLengthIfPossible(TFrame& frame, const TMsg& msg, std::size_t expLength)
{
    using MsgType = typename std::decay<decltype(msg)>::type;
    using Tag = 
        typename std::conditional<
            MsgType::hasLength(),
            details::HasLengthTag,
            details::NoLengthTag
        >::type;


    return details::verifyFrameLengthIfPossible(frame, msg, expLength, Tag());
}

template <typename TProtStack>
typename TProtStack::MsgPtr commonReadWriteMsgTest(
    TProtStack& stack,
    const char* const buf,
    std::size_t bufSize,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    using MsgPtr = typename TProtStack::MsgPtr;

    MsgPtr msg;
    auto readIter = buf;
    auto es = stack.read(msg, readIter, bufSize);
    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return std::move(msg);
    }

    TS_ASSERT(msg);

    auto actualBufSize = static_cast<std::size_t>(std::distance(buf, readIter));
    verifyFrameLengthIfPossible(stack, *msg, actualBufSize);
    std::unique_ptr<char []> outCheckBuf(new char[actualBufSize]);
    auto writeIter = &outCheckBuf[0];
    es = stack.write(*msg, writeIter, actualBufSize);
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT(std::equal(buf, buf + actualBufSize, static_cast<const char*>(&outCheckBuf[0])));
    return std::move(msg);
}

template <typename TProtStack>
typename TProtStack::MsgPtr commonReadWriteMsgTest(
    TProtStack& stack,
    typename TProtStack::AllFields& fields,
    const char* const buf,
    std::size_t bufSize,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    using MsgPtr = typename TProtStack::MsgPtr;

    MsgPtr msg;
    auto readIter = buf;
    auto es = stack.readFieldsCached(fields, msg, readIter, bufSize);
    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return std::move(msg);
    }

    TS_ASSERT(msg);

    auto actualBufSize = static_cast<std::size_t>(std::distance(buf, readIter));
    TS_ASSERT_EQUALS(actualBufSize, stack.length(*msg));
    std::unique_ptr<char []> outCheckBuf(new char[actualBufSize]);
    auto writeIter = &outCheckBuf[0];
    typename TProtStack::AllFields writtenFields;
    es = stack.writeFieldsCached(writtenFields, *msg, writeIter, actualBufSize);
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT(std::equal(buf, buf + actualBufSize, static_cast<const char*>(&outCheckBuf[0])));
    TS_ASSERT_EQUALS(fields, writtenFields);
    return std::move(msg);
}

template <typename TProtStack>
typename TProtStack::MsgPtr vectorBackInsertReadWriteMsgTest(
    TProtStack& stack,
    const char* const buf,
    std::size_t bufSize,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    using MsgPtr = typename TProtStack::MsgPtr;

    MsgPtr msg;
    auto readIter = buf;
    auto es = stack.read(msg, readIter, bufSize);
    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return std::move(msg);
    }

    TS_ASSERT(msg);

    auto actualBufSize = static_cast<std::size_t>(std::distance(buf, readIter));
    TS_ASSERT_EQUALS(actualBufSize, stack.length(*msg));
    std::vector<char> outCheckBuf;
    auto writeIter = std::back_inserter(outCheckBuf);
    es = stack.write(*msg, writeIter, actualBufSize);
    if (es == comms::ErrorStatus::UpdateRequired) {
        assert(!outCheckBuf.empty());
        auto updateIter = &outCheckBuf[0];
        es = stack.update(updateIter, actualBufSize);
    }
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT_EQUALS(outCheckBuf.size(), actualBufSize);
    TS_ASSERT_EQUALS(outCheckBuf.size(), stack.length(*msg));
    bool resultAsExpected = std::equal(buf, buf + actualBufSize, outCheckBuf.cbegin());
    if (!resultAsExpected) {
        std::cout << "Original buffer:\n\t" << std::hex;
        std::copy_n(buf, actualBufSize, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << "\n\nWritten buffer:\n\t";
        std::copy_n(&outCheckBuf[0], actualBufSize, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << std::dec << std::endl;
    }
    TS_ASSERT(resultAsExpected);
    return std::move(msg);
}


template <typename TProtStack, typename TMessage>
void commonWriteReadMsgTest(
    TProtStack& stack,
    TMessage msg,
    char* buf,
    std::size_t bufSize,
    const char* expectedBuf,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    auto writeIter = buf;
    auto es = stack.write(msg, writeIter, bufSize);
    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return;
    }

    assert(expectedBuf != nullptr);
    auto constBuf = static_cast<const char*>(buf);
    TS_ASSERT(std::equal(constBuf, constBuf + bufSize, &expectedBuf[0]));

    using MsgPtr = typename TProtStack::MsgPtr;
    MsgPtr msgPtr;
    auto readIter = expectedBuf;
    es = stack.read(msgPtr, readIter, bufSize);
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT(msgPtr);
    TS_ASSERT_EQUALS(msgPtr->getId(), msg.getId());
    auto* castedMsg = dynamic_cast<TMessage*>(msgPtr.get());
    TS_ASSERT(castedMsg != nullptr);
    TS_ASSERT_EQUALS(*castedMsg, msg);
}

template <typename TProtStack, typename TMessage>
void vectorBackInsertWriteReadMsgTest(
    TProtStack& stack,
    TMessage msg,
    const char* expectedBuf,
    std::size_t bufSize,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    std::vector<char> buf;
    auto writeIter = std::back_inserter(buf);
    auto es = stack.write(msg, writeIter, buf.max_size());
    if (expectedEs != comms::ErrorStatus::Success) {
        TS_ASSERT_EQUALS(es, expectedEs);
        return;
    }

    if (es == comms::ErrorStatus::UpdateRequired) {
        auto updateIter = &buf[0];
        es = stack.update(updateIter, buf.size());
        TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    }

    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return;
    }

    assert(expectedBuf != nullptr);
    TS_ASSERT_EQUALS(buf.size(), bufSize);
    bool bufEquals = std::equal(buf.cbegin(), buf.cend(), &expectedBuf[0]);
    if (!bufEquals) {
        std::cout << "ERROR: Buffers are not equal:\nexpected: " << std::hex;
        std::copy_n(&expectedBuf[0], bufSize, std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << "\nwritten: ";
        std::copy(buf.cbegin(), buf.cend(), std::ostream_iterator<unsigned>(std::cout, " "));
        std::cout << std::dec << std::endl;
    }
    TS_ASSERT(bufEquals);

    using MsgPtr = typename TProtStack::MsgPtr;
    MsgPtr msgPtr;
    const char* readIter = &buf[0];
    es = stack.read(msgPtr, readIter, buf.size());
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT(msgPtr);
    TS_ASSERT_EQUALS(msgPtr->getId(), msg.getId());
    auto* castedMsg = dynamic_cast<TMessage*>(msgPtr.get());
    TS_ASSERT(castedMsg != nullptr);
    TS_ASSERT_EQUALS(*castedMsg, msg);
}

template <typename TProtStack, typename TMsg>
void commonReadWriteMsgDirectTest(
    TProtStack& stack,
    TMsg& msg,
    const char* const buf,
    std::size_t bufSize,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    auto readIter = buf;
    auto es = stack.read(msg, readIter, bufSize);
    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return;
    }

    auto actualBufSize = static_cast<std::size_t>(std::distance(buf, readIter));
    TS_ASSERT_EQUALS(actualBufSize, stack.length(msg));
    std::unique_ptr<char []> outCheckBuf(new char[actualBufSize]);
    auto writeIter = &outCheckBuf[0];
    es = stack.write(msg, writeIter, actualBufSize);
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT(std::equal(buf, buf + actualBufSize, static_cast<const char*>(&outCheckBuf[0])));
}

template <typename TProtStack, typename TMsg>
void commonReadWriteMsgDirectTest(
    TProtStack& stack,
    typename TProtStack::AllFields& fields,
    TMsg& msg,
    const char* const buf,
    std::size_t bufSize,
    comms::ErrorStatus expectedEs = comms::ErrorStatus::Success)
{
    auto readIter = buf;
    auto es = stack.readFieldsCached(fields, msg, readIter, bufSize);
    TS_ASSERT_EQUALS(es, expectedEs);
    if (es != comms::ErrorStatus::Success) {
        return;
    }

    auto actualBufSize = static_cast<std::size_t>(std::distance(buf, readIter));
    TS_ASSERT_EQUALS(actualBufSize, stack.length(msg));
    std::unique_ptr<char []> outCheckBuf(new char[actualBufSize]);
    typename TProtStack::AllFields writtenFields;
    auto writeIter = &outCheckBuf[0];
    es = stack.writeFieldsCached(writtenFields, msg, writeIter, actualBufSize);
    TS_ASSERT_EQUALS(es, comms::ErrorStatus::Success);
    TS_ASSERT_EQUALS(fields, writtenFields);
    TS_ASSERT(std::equal(buf, buf + actualBufSize, static_cast<const char*>(&outCheckBuf[0])));
}

