//
// Copyright 2019 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
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

#include <type_traits>

#include "comms/Message.h"
#include "comms/details/message_check.h"

namespace comms
{

namespace details
{

template <typename TAllMessages, std::size_t TCount>
class DispatchMsgStrongLinearSwitchHelper    
{
    static_assert(TCount <= std::tuple_size<TAllMessages>::value, 
        "Invalid template params");
    static_assert(2 <= TCount, "Invalid invocation");

    static const std::size_t From = std::tuple_size<TAllMessages>::value - TCount;
    using FromElem = typename std::tuple_element<From, TAllMessages>::type;
    static_assert(messageHasStaticNumId<FromElem>(), "Message must define static ID");

public:
    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        using IdType = typename std::decay<decltype(id)>::type;

        static constexpr IdType fromId = static_cast<IdType>(FromElem::doGetId());
        switch(id) {
            case fromId: {
                auto& castedMsg = static_cast<FromElem&>(msg);
                return static_cast<RetType>(handler.handle(castedMsg));
                break;
            }
            default:
                return 
                    DispatchMsgStrongLinearSwitchHelper<TAllMessages, TCount - 1>::dispatch(
                        std::forward<TId>(id), msg, handler);
                
        };
        // dead code (just in case), should not reach here
        return static_cast<RetType>(handler.handle(msg));
    }
};

template <typename TAllMessages>
class DispatchMsgStrongLinearSwitchHelper<TAllMessages, 1>
{
    static_assert(1 <= std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

    static const std::size_t From = std::tuple_size<TAllMessages>::value - 1U;
    using Elem = typename std::tuple_element<From, TAllMessages>::type;
    static_assert(messageHasStaticNumId<Elem>(), "Message must define static ID");

public:
    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        using IdType = typename std::decay<decltype(id)>::type;

        auto elemId = static_cast<IdType>(Elem::doGetId());
        if (id != elemId) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto& castedMsg = static_cast<Elem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }
};

template <typename TAllMessages>
class DispatchMsgStrongLinearSwitchHelper<TAllMessages, 0>
{
public:
    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);

        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        return static_cast<RetType>(handler.handle(msg));
    }
};

template <typename TAllMessages, std::size_t TOrigIdx, std::size_t TRem>
class DispatchMsgLinearSwitchWeakCountFinder
{
    using OrigMsgType = typename std::tuple_element<TOrigIdx, TAllMessages>::type;
    static const std::size_t Idx = std::tuple_size<TAllMessages>::value - TRem;
    using CurrMsgType = typename std::tuple_element<Idx, TAllMessages>::type;
    static const bool IdsMatch = OrigMsgType::doGetId() == CurrMsgType::doGetId();
public:
    static const std::size_t Value = IdsMatch ? DispatchMsgLinearSwitchWeakCountFinder<TAllMessages, TOrigIdx, TRem - 1>::Value + 1 : 0U;
};

template <typename TAllMessages, std::size_t TOrigIdx>
class DispatchMsgLinearSwitchWeakCountFinder<TAllMessages, TOrigIdx, 0U>
{
public:
    static const std::size_t Value = 0U;
};

template <typename TAllMessages, std::size_t TFrom, std::size_t TCount>
class DispatchMsgWeakLinearSwitchHelper    
{
    static_assert(TFrom + TCount <= std::tuple_size<TAllMessages>::value, 
        "Invalid template params");
    static_assert(2 <= TCount, "Invalid invocation");

    using FromElem = typename std::tuple_element<TFrom, TAllMessages>::type;
    static_assert(messageHasStaticNumId<FromElem>(), "Message must define static ID");

    static const std::size_t SameIdsCount = 
        DispatchMsgLinearSwitchWeakCountFinder<
            TAllMessages, 
            TFrom, 
            TCount
        >::Value;
    
    static_assert(SameIdsCount <= TCount, "Invalid template params");
    static_assert(0U < SameIdsCount, "Invalid template params");

public:
    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        using IdType = typename std::decay<decltype(id)>::type;

        static constexpr IdType fromId = static_cast<IdType>(FromElem::doGetId());
        switch(id) {
            case fromId:
                return 
                    DispatchMsgWeakLinearSwitchHelper<TAllMessages, TFrom, SameIdsCount>::
                        dispatchOffset(offset, msg, handler);

            default:
                return 
                    DispatchMsgWeakLinearSwitchHelper<TAllMessages, TFrom + SameIdsCount, TCount - SameIdsCount>::
                        dispatch(std::forward<TId>(id), offset, msg, handler);
        };
        // dead code (just in case), should not reach here
        COMMS_ASSERT(0);
        return static_cast<RetType>(handler.handle(msg));
    }

    template <
        typename TMsg,
        typename THandler>
    static auto dispatchOffset(std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        switch(offset) {
            case 0:
            {
                auto& castedMsg = static_cast<FromElem&>(msg);
                return static_cast<RetType>(handler.handle(castedMsg));
            }
            default:
                return 
                    DispatchMsgWeakLinearSwitchHelper<TAllMessages, TFrom + 1, TCount -1>::
                        dispatchOffset(offset - 1, msg, handler);
        };

        // dead code (just in case), should not reach here
        return static_cast<RetType>(handler.handle(msg));
    }
};

template <typename TAllMessages, std::size_t TFrom>
class DispatchMsgWeakLinearSwitchHelper<TAllMessages, TFrom, 1>
{
    static_assert(TFrom + 1 <= std::tuple_size<TAllMessages>::value, 
        "Invalid template params");

    using Elem = typename std::tuple_element<TFrom, TAllMessages>::type;
    static_assert(messageHasStaticNumId<Elem>(), "Message must define static ID");

public:
    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        if (offset != 0U) {
            return static_cast<RetType>(handler.handle(msg));
        }

        using IdType = typename std::decay<decltype(id)>::type;

        auto elemId = static_cast<IdType>(Elem::doGetId());
        if (id != elemId) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto& castedMsg = static_cast<Elem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }

    template <
        typename TMsg,
        typename THandler>
    static auto dispatchOffset(std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;

        if (offset != 0U) {
            return static_cast<RetType>(handler.handle(msg));
        }

        auto& castedMsg = static_cast<Elem&>(msg);
        return static_cast<RetType>(handler.handle(castedMsg));
    }
};

template <typename TAllMessages, std::size_t TFrom>
class DispatchMsgWeakLinearSwitchHelper<TAllMessages, TFrom, 0>
{
public:
    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(id);
        static_cast<void>(offset);

        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        return static_cast<RetType>(handler.handle(msg));
    }

    template <
        typename TMsg,
        typename THandler>
    static auto dispatchOffset(std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        static_cast<void>(offset);

        using RetType = 
            MessageInterfaceDispatchRetType<
                typename std::decay<decltype(handler)>::type>;
        return static_cast<RetType>(handler.handle(msg));
    }
};

template <typename TAllMessages>
class DispatchMsgLinearSwitchHelper    
{
    struct StrongTag {};
    struct WeakTag {};

    using BinSearchTag = 
        typename std::conditional<
            allMessagesAreStrongSorted<TAllMessages>(),
            StrongTag,
            WeakTag
        >::type;

public:
    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(MsgType::hasGetId(), 
            "The used message object must provide polymorphic ID retrieval function");
        return dispatch(msg.getId(), msg, handler, BinSearchTag());
    }

    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchInternal(std::forward<TId>(id), msg, handler, BinSearchTag());
    }

    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatch(TId&& id, std::size_t offset, TMsg& msg, THandler& handler) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchInternal(std::forward<TId>(id), offset, msg, handler, BinSearchTag());
    }

private:
    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatchInternal(TId&& id, TMsg& msg, THandler& handler, StrongTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return 
            DispatchMsgStrongLinearSwitchHelper<TAllMessages, std::tuple_size<TAllMessages>::value>::
                dispatch(std::forward<TId>(id), msg, handler);
    }

    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatchInternal(TId&& id, std::size_t offset, TMsg& msg, THandler& handler, StrongTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        if (offset != 0U) {
            using RetType = 
                MessageInterfaceDispatchRetType<
                    typename std::decay<decltype(handler)>::type>;
            return static_cast<RetType>(handler.handle(msg));

        }
        return 
            DispatchMsgStrongLinearSwitchHelper<TAllMessages, std::tuple_size<TAllMessages>::value>::
                dispatch(std::forward<TId>(id), msg, handler);

    }

    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatchInternal(TId&& id, TMsg& msg, THandler& handler, WeakTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return dispatchInternal(std::forward<TId>(id), 0U, msg, handler, WeakTag());
    }

    template <
        typename TId,
        typename TMsg,
        typename THandler>
    static auto dispatchInternal(TId&& id, std::size_t offset, TMsg& msg, THandler& handler, WeakTag) ->
        MessageInterfaceDispatchRetType<
            typename std::decay<decltype(handler)>::type>
    {
        return 
            DispatchMsgWeakLinearSwitchHelper<TAllMessages, 0, std::tuple_size<TAllMessages>::value>::
                dispatch(std::forward<TId>(id), offset, msg, handler);

    }
};

} // namespace details

} // namespace comms