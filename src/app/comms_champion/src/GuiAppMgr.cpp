//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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


#include "GuiAppMgr.h"

#include <cassert>
#include <memory>

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QtQml>
#include <QtCore/QTimer>

#include "comms_champion/DefaultMessageDisplayHandler.h"
#include "GlobalConstants.h"

#include <iostream>

namespace comms_champion
{

namespace
{

GuiAppMgr::MsgType getMsgType(const MessageInfo& msgInfo)
{
    auto msgTypeVar =
        msgInfo.getExtraProperty(GlobalConstants::msgTypePropertyName());
    assert(msgTypeVar.isValid());
    assert(msgTypeVar.canConvert<int>());
    return static_cast<GuiAppMgr::MsgType>(msgTypeVar.value<int>());
}

}  // namespace

GuiAppMgr* GuiAppMgr::instance()
{
    static std::unique_ptr<GuiAppMgr> mgr(new GuiAppMgr());
    return mgr.get();
}

void GuiAppMgr::configClicked()
{
    assert(!"Config clicked");
}

void GuiAppMgr::recvStartClicked()
{
    MsgMgr::instanceRef().setRecvEnabled(true);
    m_recvState = RecvState::Running;
    emitRecvStateUpdate();
}

void GuiAppMgr::recvStopClicked()
{
    MsgMgr::instanceRef().setRecvEnabled(false);
    m_recvState = RecvState::Idle;
    emitRecvStateUpdate();
}

void GuiAppMgr::recvSaveClicked()
{
    assert(!"Recv save clicked");
}

void GuiAppMgr::recvDeleteClicked()
{
    assert(!recvListEmpty());
    assert(m_selType == SelectionType::Recv);
    assert(m_clickedMsg);

#ifndef NDEBUG

    auto msg = m_clickedMsg->getAppMessage();
    assert(msg);
    std::cout << __FUNCTION__ << ": " << msg->name() << std::endl;
#endif

    MsgMgr::instanceRef().deleteMsg(m_clickedMsg);

    clearDisplayedMessage();
    emit sigRecvDeleteSelectedMsg();
    --m_recvListCount;
    if (recvListEmpty()) {
        emit sigRecvListEmpty(true);
        emit sigRecvMsgSelected(false);
    }
}

void GuiAppMgr::recvClearClicked()
{
    assert(0 < m_recvListCount);
    clearRecvList();
    MsgMgr::instanceRef().deleteAllMsgs();
}

void GuiAppMgr::recvShowRecvToggled(bool checked)
{
    m_recvListShowReceived = checked;
    refreshRecvList();
}

void GuiAppMgr::recvShowSentToggled(bool checked)
{
    m_recvListShowSent = checked;
    refreshRecvList();
}

void GuiAppMgr::sendStartClicked()
{
    m_sendState = SendState::SendingSingle;
    emitSendStateUpdate();
}

void GuiAppMgr::sendStartAllClicked()
{
    m_sendState = SendState::SendingAll;
    emitSendStateUpdate();
}

void GuiAppMgr::sendStopClicked()
{
    m_sendState = SendState::Idle;
    m_msgsToSend.clear();
    emitSendStateUpdate();
}

void GuiAppMgr::sendSaveClicked()
{
    assert(!"Send save clicked");
}

void GuiAppMgr::sendAddClicked()
{
    emit sigNewSendMsgDialog(MsgMgr::instanceRef().getProtocol());
}

void GuiAppMgr::sendEditClicked()
{
    assert(m_clickedMsg);
    emit sigUpdateSendMsgDialog(m_clickedMsg, MsgMgr::instanceRef().getProtocol());
}

void GuiAppMgr::sendDeleteClicked()
{
    assert(!sendListEmpty());
    assert(m_selType == SelectionType::Send);
    assert(m_clickedMsg);

#ifndef NDEBUG

    auto msg = m_clickedMsg->getAppMessage();
    assert(msg);
    std::cout << __FUNCTION__ << ": " << msg->name() << std::endl;
#endif

    clearDisplayedMessage();
    emit sigSendDeleteSelectedMsg();

    --m_sendListCount;
    if (sendListEmpty()) {
        emit sigSendListEmpty(true);
        emit sigSendMsgSelected(false);
    }
}

void GuiAppMgr::sendClearClicked()
{
    emit sigSendClear();
    assert(0 < m_sendListCount);
    bool wasSelected = (m_selType == SelectionType::Send);
    assert((!wasSelected) || (m_clickedMsg));

    m_sendListCount = 0;
    emit sigSendListEmpty(true);

    if (wasSelected) {
        clearDisplayedMessage();
        emit sigSendMsgSelected(false);
    }
}

void GuiAppMgr::recvMsgClicked(MessageInfoPtr msgInfo)
{
    emit sigSendMsgListClearSelection();
    emit sigSendMsgSelected(false);

    msgClicked(msgInfo, SelectionType::Recv);
    if (!m_clickedMsg) {
        emit sigRecvMsgListClearSelection();
    }
    emit sigRecvMsgSelected(static_cast<bool>(m_clickedMsg));
}

void GuiAppMgr::sendMsgClicked(MessageInfoPtr msgInfo)
{
    emit sigRecvMsgListClearSelection();
    emit sigRecvMsgSelected(false);

    msgClicked(msgInfo, SelectionType::Send);
    if (!m_clickedMsg) {
        emit sigSendMsgListClearSelection();
    }
    emit sigSendMsgSelected(static_cast<bool>(m_clickedMsg));
}

void GuiAppMgr::sendMsgDoubleClicked(MessageInfoPtr msgInfo)
{
    // Equivalent to selection + edit
    assert(msgInfo);
    if (msgInfo != m_clickedMsg) {
        sendMsgClicked(msgInfo);
    }
    assert(m_clickedMsg == msgInfo);
    sendEditClicked();
}

GuiAppMgr::RecvState GuiAppMgr::recvState() const
{
    return m_recvState;
}

bool GuiAppMgr::recvMsgListSelectOnAddEnabled()
{
    return m_recvListSelectOnAdd;
}

bool GuiAppMgr::recvListEmpty() const
{
    return m_recvListCount == 0;
}

bool GuiAppMgr::recvListShowsReceived() const
{
    return m_recvListShowReceived;
}

bool GuiAppMgr::recvListShowsSent() const
{
    return m_recvListShowSent;
}

GuiAppMgr::SendState GuiAppMgr::sendState() const
{
    return m_sendState;
}

void GuiAppMgr::sendAddNewMessage(MessageInfoPtr msgInfo)
{
    bool wasEmpty = sendListEmpty();
    ++m_sendListCount;
    emit sigAddSendMsg(msgInfo);
    if (wasEmpty) {
        emit sigSendListEmpty(false);
    }
    sendMsgClicked(msgInfo);
    assert(m_selType == SelectionType::Send);
    assert(m_clickedMsg);
}

void GuiAppMgr::sendUpdateMessage(MessageInfoPtr msgInfo)
{
    assert(!sendListEmpty());
    assert(m_clickedMsg);
    assert(m_clickedMsg == msgInfo);
    emit sigSendMsgUpdated();
    displayMessage(std::move(msgInfo));
}

bool GuiAppMgr::sendListEmpty() const
{
    return m_sendListCount == 0;
}

void GuiAppMgr::sendMessages(MsgInfosList&& msgs)
{
    assert(m_msgsToSend.empty());
    for (auto& msgInfo : msgs) {
        m_msgsToSend.push_back(makeMessageInfoCopy(*msgInfo));
    }

    assert(!m_msgsToSend.empty());
    sendPendingAndWait();
}

GuiAppMgr::GuiAppMgr(QObject* parent)
  : Base(parent),
    m_recvState(RecvState::Idle),
    m_sendState(SendState::Idle)
{
    connect(MsgMgr::instance(), SIGNAL(sigMsgAdded(MessageInfoPtr)),
            this, SLOT(msgAdded(MessageInfoPtr)));
}

void GuiAppMgr::emitRecvStateUpdate()
{
    emit sigSetRecvState(static_cast<int>(m_recvState));
}

void GuiAppMgr::emitSendStateUpdate()
{
    emit sigSetSendState(static_cast<int>(m_sendState));
}

void GuiAppMgr::msgAdded(MessageInfoPtr msgInfo)
{
    assert(msgInfo);
    auto type = getMsgType(*msgInfo);
    assert((type == MsgType::Received) || (type == MsgType::Sent));

#ifndef NDEBUG

    static const char* const RecvPrefix = "<-- ";
    static const char* const SentPrefix = "--> ";

    const char* prefix = RecvPrefix;
    if (type == MsgType::Sent) {
        prefix = SentPrefix;
    }

    auto msg = msgInfo->getAppMessage();
    assert(msg);
    std::cout << prefix << msg->name() << std::endl;
#endif

    if (canAddToRecvList(type)) {
        addMsgToRecvList(msgInfo);
        displayMessageIfNotClicked(msgInfo);
    }
}

void GuiAppMgr::sendPendingAndWait()
{
    auto retrieveIntPropertyFunc =
        [](const MessageInfo& mInfo, const char* property) -> int
        {
            auto delayVar =
                mInfo.getExtraProperty(property);
            assert(delayVar.isValid());
            assert(delayVar.canConvert<int>());
            return delayVar.value<int>();
        };

    auto retrieveDelayFunc =
        [&](const MessageInfo& mInfo) -> int
        {
            return retrieveIntPropertyFunc(
                mInfo, GlobalConstants::msgDelayPropertyName());
        };

    auto iter = m_msgsToSend.begin();
    for (; iter != m_msgsToSend.end(); ++iter) {
        auto& msgInfo = *iter;
        assert(msgInfo);
        auto delay = retrieveDelayFunc(*msgInfo);
        if (delay != 0) {
            break;
        }
    }

    MsgInfosList nextMsgsToSend;
    nextMsgsToSend.splice(
        nextMsgsToSend.end(), m_msgsToSend, m_msgsToSend.begin(), iter);

    MsgMgr::instanceRef().sendMsgs(nextMsgsToSend);

    for (auto& msgToSend : nextMsgsToSend) {
        auto repeatMs =
            retrieveIntPropertyFunc(
                *msgToSend,
                GlobalConstants::msgRepeatDurationPropertyName());

        auto repeatCount =
            retrieveIntPropertyFunc(
                *msgToSend,
                GlobalConstants::msgRepeatCountPropertyName());

        bool reinsert =
            (0 < repeatMs) &&
            ((repeatCount == 0) || (1 < repeatCount));

        if (reinsert) {
            auto newDelay = repeatMs;
            auto iter =
                std::find_if(
                    m_msgsToSend.begin(), m_msgsToSend.end(),
                    [&newDelay, &retrieveDelayFunc](MessageInfoPtr mInfo) mutable -> bool
                    {
                        assert(mInfo);
                        auto mDelay = retrieveDelayFunc(*mInfo);
                        if (newDelay < mDelay) {
                            return true;
                        }
                        newDelay -= mDelay;
                        return false;
                    });

            if (iter != m_msgsToSend.end()) {
                auto& msgToUpdate = *iter;
                assert(msgToUpdate);
                auto mDelay = retrieveDelayFunc(*msgToUpdate);
                msgToUpdate->setExtraProperty(
                    GlobalConstants::msgDelayPropertyName(),
                    QVariant::fromValue(mDelay - newDelay));
            }

            msgToSend->setExtraProperty(
                GlobalConstants::msgDelayPropertyName(),
                QVariant::fromValue(newDelay));

            if (repeatCount != 0) {
                msgToSend->setExtraProperty(
                    GlobalConstants::msgRepeatCountPropertyName(),
                    QVariant::fromValue(repeatCount - 1));
            }

            m_msgsToSend.insert(iter, std::move(msgToSend));
        }
    }

    if (!m_msgsToSend.empty()) {
        auto& msgInfo = m_msgsToSend.front();
        assert(msgInfo);
        auto delay = retrieveDelayFunc(*msgInfo);
        assert(0 < delay);
        msgInfo->setExtraProperty(
            GlobalConstants::msgDelayPropertyName(), QVariant::fromValue(0));
        QTimer::singleShot(delay, this, SLOT(sendPendingAndWait()));
    }
    else {
        sendStopClicked();
    }
}

void GuiAppMgr::msgClicked(MessageInfoPtr msgInfo, SelectionType selType)
{
    assert(msgInfo);
    if (m_clickedMsg == msgInfo) {
#ifndef NDEBUG
        auto msg = m_clickedMsg->getAppMessage();
        assert(msg);
        std::cout << __FUNCTION__ << ": Removing selection for " << msg->name() << std::endl;
#endif

        assert(selType == m_selType);
        m_selType = SelectionType::None;
        clearDisplayedMessage();
        emit sigRecvMsgListSelectOnAddEnabled(true);
        return;
    }

    m_selType = selType;
    m_clickedMsg = msgInfo;
    displayMessage(m_clickedMsg);
    emit sigRecvMsgListSelectOnAddEnabled(false);

#ifndef NDEBUG
    auto msg = m_clickedMsg->getAppMessage();
    assert(msg);
    std::cout << __FUNCTION__ << ": " << msg->name() << std::endl;
#endif

}

void GuiAppMgr::displayMessage(MessageInfoPtr msgInfo)
{
    emit sigDisplayMsg(msgInfo);
}

void GuiAppMgr::displayMessageIfNotClicked(MessageInfoPtr msgInfo)
{
    if (!m_clickedMsg) {
        displayMessage(msgInfo);
    }
}

void GuiAppMgr::clearDisplayedMessage()
{
    m_selType = SelectionType::None;
    m_clickedMsg.reset();
    emit sigClearDisplayedMsg();
}

void GuiAppMgr::refreshRecvList()
{
    auto clickedMsg = m_clickedMsg;
    if (m_selType == SelectionType::Recv) {
        assert(m_clickedMsg);
        recvMsgClicked(m_clickedMsg);
        assert(!m_clickedMsg);
    }
    else if (m_selType != SelectionType::Send) {
        emit sigClearDisplayedMsg();
    }

    clearRecvList();

    auto& allMsgs = MsgMgr::instanceRef().getAllMsgs();
    for (auto& msgInfo : allMsgs) {
        auto type = getMsgType(*msgInfo);

        if (canAddToRecvList(type)) {
            addMsgToRecvList(msgInfo);
            if (msgInfo == clickedMsg) {
                recvMsgClicked(msgInfo);
            }
        }
    }

    if (!m_clickedMsg) {
        emit sigRecvMsgListClearSelection();
    }
}

void GuiAppMgr::addMsgToRecvList(MessageInfoPtr msgInfo)
{
    assert(msgInfo);
    bool wasEmpty = recvListEmpty();
    ++m_recvListCount;
    emit sigAddRecvMsg(msgInfo);
    if (wasEmpty) {
        emit sigRecvListEmpty(false);
    }
}

void GuiAppMgr::clearRecvList()
{
    emit sigRecvClear();

    bool wasSelected = (m_selType == SelectionType::Recv);
    assert((!wasSelected) || (m_clickedMsg));

    m_recvListCount = 0;
    emit sigRecvListEmpty(true);

    if (wasSelected) {
        clearDisplayedMessage();
        emit sigRecvMsgSelected(false);
        emit sigRecvMsgListSelectOnAddEnabled(true);
    }
}

bool GuiAppMgr::canAddToRecvList(MsgType type) const
{
    assert((type == MsgType::Received) || (type == MsgType::Sent));

    return
        ((type == MsgType::Received) && m_recvListShowReceived) ||
        ((type == MsgType::Sent) && m_recvListShowSent);
}

}  // namespace comms_champion

