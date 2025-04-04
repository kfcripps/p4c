/**
 * Copyright (C) 2024 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.  See the License for the specific language governing permissions
 * and limitations under the License.
 *
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BACKENDS_TOFINO_BF_ASM_EXENAME_H_
#define BACKENDS_TOFINO_BF_ASM_EXENAME_H_

/** Attempt to determine the executable name and return a static path to it.  Will use
 * argv0 if provided and nothing better can be found */
const char *exename(const char *argv0 = nullptr);

#endif /* BACKENDS_TOFINO_BF_ASM_EXENAME_H_ */
