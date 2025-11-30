/**
 * @file
 *
 * @brief Contains the definition of the error codes of the library.
 *
 * @authors
 * Copyright (C) 2023 André L. C. Moreira <andrelcmoreira@disroot.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef ERRORS_H_
#define ERRORS_H_

/**
 * @brief Library error mapping.
 */
typedef enum {
  POOL_DAY_SUCCESS,
  POOL_DAY_ERROR_NULL_PARAM
} pool_day_retcode_t;

#endif  // ERRORS_H_
