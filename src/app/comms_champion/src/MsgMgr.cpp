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


#include "MsgMgr.h"

#include <cassert>

#include <QtCore/QVariant>

#include "GlobalConstants.h"

namespace comms_champion
{

MsgMgr* MsgMgr::instance()
{
    return &(instanceRef());
}

MsgMgr& MsgMgr::instanceRef()
{
    static MsgMgr mgr;
    return mgr;
}

void MsgMgr::addSocket(SocketPtr&& socket)
{
    if (!m_sockets.empty()) {
        auto& lastSocket = m_sockets.back();

        disconnect(
            lastSocket.get(), SIGNAL(sigDataReceived(DataInfoPtr)),
            this, SLOT(socketDataReceived(DataInfoPtr)));

        connect(
            lastSocket.get(), SIGNAL(sigDataReceived(DataInfoPtr)),
            socket.get(), SLOT(feedInData(DataInfoPtr)));

        connect(
            socket.get(), SIGNAL(sigDataToSend(DataInfoPtr)),
            lastSocket.get(), SLOT(sendData(DataInfoPtr)));
    }

    connect(
        socket.get(), SIGNAL(sigDataReceived(DataInfoPtr)),
        this, SLOT(socketDataReceived(DataInfoPtr)));

    m_sockets.push_back(std::move(socket));
}

void MsgMgr::addProtocol(ProtocolPtr&& protocol)
{
    m_protStack.addProtocol(std::move(protocol));
}

void MsgMgr::setRecvEnabled(bool enabled)
{
    m_recvEnabled = enabled;
    if (m_sockets.empty()) {
        return;
    }

    if (enabled) {
        for (auto& s : m_sockets) {
            s->start();
        }
    }
    else {
        for (auto& s : m_sockets) {
            s->stop();
        }
    }
}

void MsgMgr::socketDataReceived(DataInfoPtr dataInfoPtr)
{
    if (!m_recvEnabled) {
        return;
    }

    auto protInfosList = m_protStack.processSocketData(std::move(dataInfoPtr));
    if (protInfosList.empty()) {
        return;
    }

    for (auto& protInfo : protInfosList) {
        auto& msgInfo = protInfo->back();
        assert(msgInfo->getAppMessage());
        // TODO: find better place for this property
        msgInfo->setExtraProperty(
            GlobalConstants::msgNumberPropertyName(),
            QVariant::fromValue(m_nextMsgNum));
        ++m_nextMsgNum;
        emit sigMsgReceived(protInfo);
    }

    m_recvMsgs.reserve(m_recvMsgs.size() + protInfosList.size());
    std::move(protInfosList.begin(), protInfosList.end(), std::back_inserter(m_recvMsgs));
}

MsgMgr::MsgMgr(QObject* parent)
  : Base(parent)
{
}

}  // namespace comms_champion

