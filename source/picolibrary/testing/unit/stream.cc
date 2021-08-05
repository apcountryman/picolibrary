/**
 * picolibrary
 *
 * Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
 * contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * \file
 * \brief picolibrary::Testing::Unit::Stream implementation.
 */

#include "picolibrary/testing/unit/stream.h"

namespace picolibrary::Testing::Unit {

Mock_Stream_Buffer::Mock_Stream_Buffer()
{
}

Mock_Stream_Buffer::~Mock_Stream_Buffer() noexcept
{
}

Mock_Output_Stream::Mock_Output_Stream()
{
    set_buffer( &m_buffer );
}

Mock_Output_Stream::~Mock_Output_Stream() noexcept
{
}

} // namespace picolibrary::Testing::Unit
