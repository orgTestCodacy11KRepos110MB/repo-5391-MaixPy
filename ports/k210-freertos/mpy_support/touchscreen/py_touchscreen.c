


#include <mp.h>
#include "touchscreen.h"


mp_obj_t py_touchscreen_init(uint n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum{
        ARG_i2c,
        ARG_cal
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_cal, MP_ARG_OBJ, {.u_obj = mp_const_none} }
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    touchscreen_config_t config;
    config.i2c = (machine_hard_i2c_obj_t*)args[ARG_i2c].u_obj;
    if( config.i2c == mp_const_none)
    {
        config.i2c = NULL;
    }
    mp_obj_t cal = args[ARG_cal].u_obj;
    if( cal !=  mp_const_none)
    {
        size_t size;
        mp_obj_t* tuple_data;
        mp_obj_tuple_get(cal, &size, &tuple_data);
        if(size != CALIBRATION_SIZE)
            mp_raise_ValueError("tuple size must be 7");
        for(uint8_t i=0; i<CALIBRATION_SIZE; ++i)
            config.calibration[i] = mp_obj_get_int(tuple_data[i]);
    }
    else
    {
        config.calibration[0] = 65;
        config.calibration[1] = 5853;
        config.calibration[2] = -1083592;
        config.calibration[3] = -4292;
        config.calibration[4] = -15;
        config.calibration[5] = 16450115;
        config.calibration[6] = 65536;
    }
    int ret = touchscreen_init((void*)&config);
    if( ret != 0)
        mp_raise_OSError(ret);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_touchscreen_init_obj, 0, py_touchscreen_init);

mp_obj_t py_touchscreen_deinit(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    int ret = touchscreen_deinit();
    if( ret != 0)
        mp_raise_OSError(ret);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_touchscreen_deinit_obj, 0, py_touchscreen_deinit);

mp_obj_t py_touchscreen_read(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    int ret, type=0, x=0, y=0;
    ret = touchscreen_read(&type, &x, &y);
    if( ret != 0)
        mp_raise_OSError(ret);
    mp_obj_t value[3];
    value[0] = mp_obj_new_int(type);
    value[1] = mp_obj_new_int(x);
    value[2] = mp_obj_new_int(y);
    mp_obj_t t = mp_obj_new_tuple(3, value);
    return t;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_touchscreen_read_obj, 0, py_touchscreen_read);

mp_obj_t py_touchscreen_calibrate(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    int ret = touchscreen_calibrate();
    if( ret!=0 )
        mp_raise_OSError(ret);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_touchscreen_calibrate_obj, 0, py_touchscreen_calibrate);

STATIC const mp_map_elem_t globals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_touchscreen)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&py_touchscreen_init_obj},
    {MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&py_touchscreen_deinit_obj},
    {MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&py_touchscreen_read_obj},
    {MP_OBJ_NEW_QSTR(MP_QSTR_calibrate), (mp_obj_t)&py_touchscreen_calibrate_obj},
    { MP_ROM_QSTR(MP_QSTR_TYPE_IDLE),   MP_ROM_INT(TOUCHSCREEN_TYPE_IDLE) },
    { MP_ROM_QSTR(MP_QSTR_TYPE_RELEASE),   MP_ROM_INT(TOUCHSCREEN_TYPE_RELEASE) },
    { MP_ROM_QSTR(MP_QSTR_TYPE_PRESS),   MP_ROM_INT(TOUCHSCREEN_TYPE_PRESS) },
};

STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t mp_module_touchscreen = {
    .base = {&mp_type_module},
    //just support one touchscreen so we don't need make_new function
    .globals = (mp_obj_t)&globals_dict,
};
