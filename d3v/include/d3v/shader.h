#ifndef PENGUIN_D3V_SHADER_H
#define PENGUIN_D3V_SHADER_H

enum _shader_id {
    PENGUIN_MIN_SHADER = 0,
    PENGUIN_SHADER_TEX_LIGHT = PENGUIN_MIN_SHADER,
    PENGUIN_SHADER_SIMPLE_RED,
    PENGUIN_SHADER_COLOR_NO_MODEL,
    PENGUIN_SHADER_COLOR_NO_PROJ,
    PENGUIN_SHADER_TEXT,

    PENGUIN_MAX_SHADER = 100,
    PENGUIN_NUM_SHADER = PENGUIN_MAX_SHADER + 1,
};
typedef enum _shader_id ShaderId;

int get_or_load_shader(ShaderId shader_id);


typedef void (*shader_callback_t)(int program_id, void*data);
void for_each_shader_program(shader_callback_t, void *data);

#endif // PENGUIN_D3V_SHADER_H
