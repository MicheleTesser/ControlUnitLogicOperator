#ifndef __GIEI_ENGINE_INTERFACE__
#define __GIEI_ENGINE_INTERFACE__

#if !defined (engine_module_init)
#define engine_module_init() engine_module_init_not_defined()
#endif

#if !defined (engine_set_max_speed)
#define engine_set_max_speed(speed) engine_set_max_speed_not_defined(speed)
#endif

#if !defined (engine_send_torque)
#define engine_send_torque(engine, pos_torque, neg_torque) \
    engine_send_torque_not_defined(engine, pos_torque, neg_torque)
#endif

#if !defined (engine_update_status)
#define engine_update_status(mex) engine_update_status_not_defined(mex)
#endif

#if !defined (engine_inverter_hv_status)
#define engine_inverter_hv_status() engine_inverter_hv_status()
#endif

#if !defined (engine_rtd_procedure)
#define engine_rtd_procedure() engine_rtd_procedure_not_defined()
#endif

#if !defined (engine_shut_down_power)
#define engine_shut_down_power() engine_shut_down_power_not_defined()
#endif

#if !defined (engine_get_info)
#define engine_get_info(engine, info) engine_get_info_not_defined(engine,info)
#endif

#if !defined (engine_max_pos_torque)
#define engine_max_pos_torque(limit_max_pos_torque) \
    engine_max_pos_torque_not_defined(limit_max_pos_torque)
#endif

#if !defined (engine_max_neg_torque)
#define engine_max_neg_torque(limit_max_neg_torque) \
    engine_max_neg_torque_not_defined(limit_max_neg_torque)
#endif

#endif // !__GIEI_ENGINE_INTERFACE__
