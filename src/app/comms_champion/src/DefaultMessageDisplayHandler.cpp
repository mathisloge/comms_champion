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

#include "comms_champion/DefaultMessageDisplayHandler.h"

#include <QtCore/QVariant>

#include "GlobalConstants.h"

#include "widget/field/IntValueFieldWidget.h"
#include "widget/field/BitmaskValueFieldWidget.h"
#include "widget/field/EnumValueFieldWidget.h"
#include "widget/field/StringFieldWidget.h"
#include "widget/field/BitfieldFieldWidget.h"
#include "widget/field/OptionalFieldWidget.h"
#include "widget/field/UnknownValueFieldWidget.h"

namespace comms_champion
{

DefaultMessageDisplayHandler::MsgWidgetPtr
DefaultMessageDisplayHandler::createMsgWidgetImpl(
    Message& msg)
{
    m_widget.reset(new DefaultMessageWidget(msg));
    msg.display(*this);
    return MsgWidgetPtr(m_widget.release());
}

DefaultMessageDisplayHandler::FieldWidgetPtr
DefaultMessageDisplayHandler::createIntValueFieldWidget(
    field_wrapper::IntValueWrapperPtr&& fieldWrapper)
{
    return
        FieldWidgetPtr(
            new IntValueFieldWidget(std::move(fieldWrapper)));
}

DefaultMessageDisplayHandler::FieldWidgetPtr
DefaultMessageDisplayHandler::createBitmaskValueFieldWidget(
    field_wrapper::BitmaskValueWrapperPtr&& fieldWrapper)
{
    return
        FieldWidgetPtr(
            new BitmaskValueFieldWidget(std::move(fieldWrapper)));
}

DefaultMessageDisplayHandler::FieldWidgetPtr
DefaultMessageDisplayHandler::createEnumValueFieldWidget(
    field_wrapper::EnumValueWrapperPtr&& fieldWrapper)
{
    return
        FieldWidgetPtr(
            new EnumValueFieldWidget(std::move(fieldWrapper)));
}

DefaultMessageDisplayHandler::FieldWidgetPtr
DefaultMessageDisplayHandler::createStringFieldWidget(
    field_wrapper::StringWrapperPtr&& fieldWrapper)
{
    return
        FieldWidgetPtr(
            new StringFieldWidget(std::move(fieldWrapper)));
}

DefaultMessageDisplayHandler::FieldWidgetPtr
DefaultMessageDisplayHandler::createBitfieldFieldWidget(
    field_wrapper::BitfieldWrapperPtr&& fieldWrapper)
{
    return
        FieldWidgetPtr(
            new BitfieldFieldWidget(std::move(fieldWrapper)));
}

DefaultMessageDisplayHandler::FieldWidgetPtr
DefaultMessageDisplayHandler::createOptionalFieldWidget(
    field_wrapper::OptionalWrapperPtr fieldWrapper)
{
    return
        FieldWidgetPtr(
            new OptionalFieldWidget(std::move(fieldWrapper)));
}

DefaultMessageDisplayHandler::FieldWidgetPtr
DefaultMessageDisplayHandler::createUnknownValueFieldWidget(
    field_wrapper::UnknownValueWrapperPtr&& fieldWrapper)
{
    return
        FieldWidgetPtr(
            new UnknownValueFieldWidget(std::move(fieldWrapper)));
}

void DefaultMessageDisplayHandler::bitfieldWidgetAddMember(
    FieldWidget& bitfieldWidget,
    FieldWidgetPtr memberFieldWidget)
{
    auto* castedBitfieldWidget = dynamic_cast<BitfieldFieldWidget*>(&bitfieldWidget);
    if (castedBitfieldWidget == nullptr) {
        assert(!"Wrong cast, expected bitfield widget");
        return;
    }

    castedBitfieldWidget->addMemberField(memberFieldWidget.release());
}

void DefaultMessageDisplayHandler::optionalWidgetSetField(
    FieldWidget& optionalWidget,
    FieldWidgetPtr fieldWidget)
{
    auto* castedOptionalFieldWidget = dynamic_cast<OptionalFieldWidget*>(&optionalWidget);
    if (castedOptionalFieldWidget == nullptr) {
        assert(!"Wrong cast, expected optional widget");
        return;
    }

    castedOptionalFieldWidget->setField(fieldWidget.release());
}

void DefaultMessageDisplayHandler::updateFieldIdxProperty(
    FieldWidget& field,
    std::size_t idx)
{
    field.setProperty(
        GlobalConstants::indexPropertyName(),
        static_cast<uint>(idx));
}

}  // namespace comms_champion


