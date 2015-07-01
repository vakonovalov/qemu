/*
 * replay-input.c
 *
 * Copyright (c) 2010-2015 Institute for System Programming
 *                         of the Russian Academy of Sciences.
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#include "qemu-common.h"
#include "replay.h"
#include "replay-internal.h"
#include "ui/input.h"
#include "qapi/qmp-output-visitor.h"
#include "qapi/qmp-input-visitor.h"
#include "qapi-visit.h"

static InputEvent *qapi_clone_InputEvent(InputEvent *src)
{
    QmpOutputVisitor *qov;
    QmpInputVisitor *qiv;
    Visitor *ov, *iv;
    QObject *obj;
    InputEvent *dst = NULL;

    qov = qmp_output_visitor_new();
    ov = qmp_output_get_visitor(qov);
    visit_type_InputEvent(ov, &src, NULL, &error_abort);
    obj = qmp_output_get_qobject(qov);
    qmp_output_visitor_cleanup(qov);
    if (!obj) {
        return NULL;
    }

    qiv = qmp_input_visitor_new(obj);
    iv = qmp_input_get_visitor(qiv);
    visit_type_InputEvent(iv, &dst, NULL, &error_abort);
    qmp_input_visitor_cleanup(qiv);
    qobject_decref(obj);

    return dst;
}

void replay_save_input_event(InputEvent *evt)
{
    replay_put_dword(evt->kind);

    switch (evt->kind) {
    case INPUT_EVENT_KIND_KEY:
        replay_put_dword(evt->key->key->kind);

        switch (evt->key->key->kind) {
        case KEY_VALUE_KIND_NUMBER:
            replay_put_qword(evt->key->key->number);
            replay_put_byte(evt->key->down);
            break;
        case KEY_VALUE_KIND_QCODE:
            replay_put_dword(evt->key->key->qcode);
            replay_put_byte(evt->key->down);
            break;
        case KEY_VALUE_KIND_MAX:
            /* keep gcc happy */
            break;
        }
        break;
    case INPUT_EVENT_KIND_BTN:
        replay_put_dword(evt->btn->button);
        replay_put_byte(evt->btn->down);
        break;
    case INPUT_EVENT_KIND_REL:
        replay_put_dword(evt->rel->axis);
        replay_put_qword(evt->rel->value);
        break;
    case INPUT_EVENT_KIND_ABS:
        replay_put_dword(evt->abs->axis);
        replay_put_qword(evt->abs->value);
        break;
    case INPUT_EVENT_KIND_MAX:
        /* keep gcc happy */
        break;
    }
}

InputEvent *replay_read_input_event(void)
{
    InputEvent evt;
    KeyValue keyValue;
    InputKeyEvent key;
    key.key = &keyValue;
    InputBtnEvent btn;
    InputMoveEvent rel;
    InputMoveEvent abs;

    evt.kind = replay_get_dword();
    switch (evt.kind) {
    case INPUT_EVENT_KIND_KEY:
        evt.key = &key;
        evt.key->key->kind = replay_get_dword();

        switch (evt.key->key->kind) {
        case KEY_VALUE_KIND_NUMBER:
            evt.key->key->number = replay_get_qword();
            evt.key->down = replay_get_byte();
            break;
        case KEY_VALUE_KIND_QCODE:
            evt.key->key->qcode = (QKeyCode)replay_get_dword();
            evt.key->down = replay_get_byte();
            break;
        case KEY_VALUE_KIND_MAX:
            /* keep gcc happy */
            break;
        }
        break;
    case INPUT_EVENT_KIND_BTN:
        evt.btn = &btn;
        evt.btn->button = (InputButton)replay_get_dword();
        evt.btn->down = replay_get_byte();
        break;
    case INPUT_EVENT_KIND_REL:
        evt.rel = &rel;
        evt.rel->axis = (InputAxis)replay_get_dword();
        evt.rel->value = replay_get_qword();
        break;
    case INPUT_EVENT_KIND_ABS:
        evt.abs = &abs;
        evt.abs->axis = (InputAxis)replay_get_dword();
        evt.abs->value = replay_get_qword();
        break;
    case INPUT_EVENT_KIND_MAX:
        /* keep gcc happy */
        break;
    }

    return qapi_clone_InputEvent(&evt);
}

void replay_input_event(QemuConsole *src, InputEvent *evt)
{
    if (replay_mode == REPLAY_MODE_PLAY) {
        /* Nothing */
    } else if (replay_mode == REPLAY_MODE_RECORD) {
        replay_add_input_event(qapi_clone_InputEvent(evt));
    } else {
        qemu_input_event_send_impl(src, evt);
    }
}

void replay_input_sync_event(void)
{
    if (replay_mode == REPLAY_MODE_PLAY) {
        /* Nothing */
    } else if (replay_mode == REPLAY_MODE_RECORD) {
        replay_add_input_sync_event();
    } else {
        qemu_input_event_sync_impl();
    }
}
