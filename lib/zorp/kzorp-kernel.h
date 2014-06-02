/***************************************************************************
 *
 * Copyright (c) 2000-2014 BalaBit IT Ltd, Budapest, Hungary
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation.
 *
 * Note that this permission is granted for only version 2 of the GPL.
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 ***************************************************************************/

#ifndef ZORP_KZORP_KERNEL_H_INCLUDED
#define ZORP_KZORP_KERNEL_H_INCLUDED

#define KZ_ATTR_NAME_MAX_LENGTH 1023
#define SO_KZORP_RESULT 1678333

struct z_kzorp_lookup_result {
        u_int64_t cookie;
        char czone_name[KZ_ATTR_NAME_MAX_LENGTH + 1];
        char szone_name[KZ_ATTR_NAME_MAX_LENGTH + 1];
        char dispatcher_name[KZ_ATTR_NAME_MAX_LENGTH + 1];
        char service_name[KZ_ATTR_NAME_MAX_LENGTH + 1];
};

#endif
