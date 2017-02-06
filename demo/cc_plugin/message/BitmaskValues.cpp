//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include <cassert>

#include "BitmaskValues.h"

namespace cc = comms_champion;

namespace demo
{

namespace cc_plugin
{

namespace message
{

namespace
{

typedef demo::message::BitmaskValuesFields<BitmaskValues::Field> BitmaskValuesFields;


QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(
        cc::property::field::ForField<BitmaskValuesFields::field1>()
            .name("field1")
            .add("bit0")
            .add("bit1")
            .add("bit2")
            .add("bit3")
            .add("bit4")
            .asMap());

    assert(
        cc::property::field::BitmaskValue(props.back())
            .bits().size() == (int)BitmaskValuesFields::field1_NumOfValues);

    props.append(
        cc::property::field::ForField<BitmaskValuesFields::field2>()
            .name("field2")
            .add("bit0")
            .add(3, "bit3")
            .add(8, "bit8")
            .add("bit9")
            .asMap());
    assert(
        cc::property::field::BitmaskValue(props.back())
            .bits().size() == (int)BitmaskValuesFields::field2_NumOfValues);

    assert(props.size() == BitmaskValues::FieldIdx_numOfValues);
    return props;
}

}  // namespace

BitmaskValues::BitmaskValues() = default;
BitmaskValues::~BitmaskValues() = default;

BitmaskValues& BitmaskValues::operator=(const BitmaskValues&) = default;
BitmaskValues& BitmaskValues::operator=(BitmaskValues&&) = default;

const char* BitmaskValues::nameImpl() const
{
    static const char* Str = "BitmaskValues";
    return Str;
}

const QVariantList& BitmaskValues::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace cc_plugin

}  // namespace demo

