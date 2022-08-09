#include <exchange/message_circular_buffer.h>


message_buffer_t *mb_init(size_t size) {
    circular_buffer_config_t config = {
        .max_elements = size,
        .size_of_data = sizeof(message_t)
    };

    return cb_init(config);
}

void mb_delete(message_buffer_t *buffer) {
    return cb_delete(buffer);
}


size_t mb_is_full(message_buffer_t *buffer) {
    return cb_is_full(buffer);
}

size_t mb_is_empty(message_buffer_t *buffer) {
    return cb_is_empty(buffer);
}

void mb_insert_item(message_buffer_t *buffer, const message_t* message) {
    return cb_insert_item(buffer, message);
}

message_t* mb_remove_item(message_buffer_t *buffer, message_t* message) {
    return cb_remove_item(buffer, message);
}

void mb_print_util(message_t *message) {
    printf("source: %d\ndestination: %d\nline: %d\nquit: %d\nmessage: %s\n",
    message->source,
    message->destination,
    message->line,
    message->quit,
    message->message);
}