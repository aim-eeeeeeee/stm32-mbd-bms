/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: BmsSupervisor.h
 *
 * Code generated for Simulink model 'BmsSupervisor'.
 *
 * Model version                  : 1.13
 * Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
 * C/C++ source code generated on : Thu Aug  6 21:12:09 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef BmsSupervisor_h_
#define BmsSupervisor_h_
#ifndef BmsSupervisor_COMMON_INCLUDES_
#define BmsSupervisor_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                                 /* BmsSupervisor_COMMON_INCLUDES_ */

#include "BmsSupervisor_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  uint32_T entry_timer_ms;             /* '<S8>/UV_Warning_Chart' */
  uint32_T clear_timer_ms;             /* '<S8>/UV_Warning_Chart' */
  uint32_T entry_timer_ms_m;           /* '<S7>/UV_Fault_Chart' */
  uint32_T clear_timer_ms_p;           /* '<S7>/UV_Fault_Chart' */
  uint32_T entry_timer_ms_n;           /* '<S4>/OV_Warning_Chart' */
  uint32_T clear_timer_ms_f;           /* '<S4>/OV_Warning_Chart' */
  uint32_T entry_timer_ms_f;           /* '<S3>/OV_Fault_Chart' */
  uint32_T clear_timer_ms_k;           /* '<S3>/OV_Fault_Chart' */
  uint8_T is_active_c1_BmsSupervisor;  /* '<S8>/UV_Warning_Chart' */
  uint8_T is_c1_BmsSupervisor;         /* '<S8>/UV_Warning_Chart' */
  uint8_T is_active_c2_BmsSupervisor;  /* '<S7>/UV_Fault_Chart' */
  uint8_T is_c2_BmsSupervisor;         /* '<S7>/UV_Fault_Chart' */
  uint8_T is_active_c3_BmsSupervisor;  /* '<Root>/Supervisor_Stateflow' */
  uint8_T is_c3_BmsSupervisor;         /* '<Root>/Supervisor_Stateflow' */
  uint8_T is_active_c4_BmsSupervisor;  /* '<S4>/OV_Warning_Chart' */
  uint8_T is_c4_BmsSupervisor;         /* '<S4>/OV_Warning_Chart' */
  uint8_T is_active_c5_BmsSupervisor;  /* '<S3>/OV_Fault_Chart' */
  uint8_T is_c5_BmsSupervisor;         /* '<S3>/OV_Fault_Chart' */
} DW_BmsSupervisor_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  uint16_T min_cell_mv;                /* '<Root>/min_cell_mv' */
  uint16_T max_cell_mv;                /* '<Root>/max_cell_mv' */
  boolean_T measurement_valid;         /* '<Root>/measurement_valid' */
  boolean_T measurement_stale;         /* '<Root>/measurement_stale' */
  uint16_T step_ms;                    /* '<Root>/step_ms' */
} ExtU_BmsSupervisor_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  uint8_T state;                       /* '<Root>/state' */
  uint16_T warning_flags;              /* '<Root>/warning_flags' */
  uint16_T fault_flags;                /* '<Root>/fault_flags' */
  boolean_T charge_allowed_shadow;     /* '<Root>/charge_allowed_shadow' */
  boolean_T discharge_allowed_shadow;  /* '<Root>/discharge_allowed_shadow' */
} ExtY_BmsSupervisor_T;

/* Real-time Model Data Structure */
struct tag_RTM_BmsSupervisor_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_BmsSupervisor_T BmsSupervisor_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_BmsSupervisor_T BmsSupervisor_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_BmsSupervisor_T BmsSupervisor_Y;

/* Model entry point functions */
extern void BmsSupervisor_initialize(void);
extern void BmsSupervisor_step(void);
extern void BmsSupervisor_terminate(void);

/* Real-time Model object */
extern RT_MODEL_BmsSupervisor_T *const BmsSupervisor_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'BmsSupervisor'
 * '<S1>'   : 'BmsSupervisor/Flag_Builder'
 * '<S2>'   : 'BmsSupervisor/Measurement_Quality'
 * '<S3>'   : 'BmsSupervisor/OV_Fault_Detector'
 * '<S4>'   : 'BmsSupervisor/OV_Warning_Detector'
 * '<S5>'   : 'BmsSupervisor/Permission_Logic'
 * '<S6>'   : 'BmsSupervisor/Supervisor_Stateflow'
 * '<S7>'   : 'BmsSupervisor/UV_Fault_Detector'
 * '<S8>'   : 'BmsSupervisor/UV_Warning_Detector'
 * '<S9>'   : 'BmsSupervisor/OV_Fault_Detector/OV_Fault_Chart'
 * '<S10>'  : 'BmsSupervisor/OV_Warning_Detector/OV_Warning_Chart'
 * '<S11>'  : 'BmsSupervisor/Permission_Logic/Is_Normal'
 * '<S12>'  : 'BmsSupervisor/Permission_Logic/Is_Warning'
 * '<S13>'  : 'BmsSupervisor/UV_Fault_Detector/UV_Fault_Chart'
 * '<S14>'  : 'BmsSupervisor/UV_Warning_Detector/UV_Warning_Chart'
 */
#endif                                 /* BmsSupervisor_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
