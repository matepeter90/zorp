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

#ifndef ZORP_MODULES_SMTP_SMTPMSG_H
#define ZORP_MODULES_SMTP_SMTPMSG_H

typedef struct
{
  char *code;
  char *long_desc;
}SmtpMessage;

typedef enum
{
  SMTP_MSG_READY_TO_STARTTLS,
  SMTP_MSG_SYNTAX_ERROR_NO_PARAM,
  SMTP_MSG_TLS_NOT_AVAILABLE,
  SMTP_N_MSGS
}SmtpMessageTypes;

#endif
