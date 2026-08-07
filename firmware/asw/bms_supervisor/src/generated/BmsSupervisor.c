/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: BmsSupervisor.c
 *
 * Code generated for Simulink model 'BmsSupervisor'.
 *
 * Model version                  : 1.13
 * Simulink Coder version         : 26.1 (R2026a) 20-Nov-2025
 * C/C++ source code generated on : Thu Aug  6 22:11:03 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "BmsSupervisor.h"
#include "rtwtypes.h"

/* Named constants for Chart: '<S3>/OV_Fault_Chart' */
#define BmsSupervisor_IN_ACTIVE        ((uint8_T)1U)
#define BmsSupervisor_IN_INACTIVE      ((uint8_T)2U)

/* Named constants for Chart: '<Root>/Supervisor_Stateflow' */
#define BmsSupervisor_IN_FAULT         ((uint8_T)1U)
#define BmsSupervisor_IN_NORMAL        ((uint8_T)2U)
#define BmsSupervisor_IN_SENSOR_ERROR  ((uint8_T)3U)
#define BmsSupervisor_IN_WARNING       ((uint8_T)4U)

/* Block states (default storage) */
DW_BmsSupervisor_T BmsSupervisor_DW;

/* External inputs (root inport signals with default storage) */
ExtU_BmsSupervisor_T BmsSupervisor_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_BmsSupervisor_T BmsSupervisor_Y;

/* Real-time model */
static RT_MODEL_BmsSupervisor_T BmsSupervisor_M_;
RT_MODEL_BmsSupervisor_T *const BmsSupervisor_M = &BmsSupervisor_M_;

/* Model step function */
void BmsSupervisor_step(void)
{
  uint32_T qY;
  uint32_T qY_tmp;
  uint16_T tmp;
  uint16_T tmp_0;
  uint16_T tmp_1;
  boolean_T rtb_AND;
  boolean_T rtb_ov_warning_active;
  boolean_T rtb_uv_fault_active;
  boolean_T rtb_uv_warning_active;

  /* Logic: '<S2>/AND' incorporates:
   *  Inport: '<Root>/measurement_stale'
   *  Inport: '<Root>/measurement_valid'
   *  Logic: '<S2>/NOT1'
   */
  rtb_AND = (BmsSupervisor_U.measurement_valid &&
             !BmsSupervisor_U.measurement_stale);

  /* Chart: '<S8>/UV_Warning_Chart' incorporates:
   *  Inport: '<Root>/min_cell_mv'
   *  Inport: '<Root>/step_ms'
   */
  if (BmsSupervisor_DW.is_active_c1_BmsSupervisor == 0) {
    BmsSupervisor_DW.is_active_c1_BmsSupervisor = 1U;
    BmsSupervisor_DW.is_c1_BmsSupervisor = BmsSupervisor_IN_INACTIVE;
    rtb_uv_warning_active = false;
    BmsSupervisor_DW.clear_timer_ms = 0U;
  } else if (BmsSupervisor_DW.is_c1_BmsSupervisor == BmsSupervisor_IN_ACTIVE) {
    rtb_uv_warning_active = true;
    qY_tmp = BmsSupervisor_DW.clear_timer_ms + /*MW:OvSatOk*/
      BmsSupervisor_U.step_ms;
    qY = qY_tmp;
    if (qY_tmp < BmsSupervisor_DW.clear_timer_ms) {
      qY = MAX_uint32_T;
    }

    if (rtb_AND && (BmsSupervisor_U.min_cell_mv >= 3300) && (qY >= 1000U)) {
      BmsSupervisor_DW.is_c1_BmsSupervisor = BmsSupervisor_IN_INACTIVE;
      rtb_uv_warning_active = false;
      BmsSupervisor_DW.clear_timer_ms = 0U;
    } else if (rtb_AND) {
      if (BmsSupervisor_U.min_cell_mv >= 3300) {
        if (qY_tmp < BmsSupervisor_DW.clear_timer_ms) {
          qY_tmp = MAX_uint32_T;
        }

        BmsSupervisor_DW.clear_timer_ms = qY_tmp;
      } else {
        BmsSupervisor_DW.clear_timer_ms = 0U;
      }
    }
  } else {
    /* case IN_INACTIVE: */
    rtb_uv_warning_active = false;
    qY_tmp = BmsSupervisor_DW.entry_timer_ms + /*MW:OvSatOk*/
      BmsSupervisor_U.step_ms;
    qY = qY_tmp;
    if (qY_tmp < BmsSupervisor_DW.entry_timer_ms) {
      qY = MAX_uint32_T;
    }

    if (rtb_AND && (BmsSupervisor_U.min_cell_mv <= 3200) && (qY >= 1000U)) {
      BmsSupervisor_DW.is_c1_BmsSupervisor = BmsSupervisor_IN_ACTIVE;
      rtb_uv_warning_active = true;
      BmsSupervisor_DW.entry_timer_ms = 0U;
    } else if (rtb_AND) {
      if (BmsSupervisor_U.min_cell_mv <= 3200) {
        if (qY_tmp < BmsSupervisor_DW.entry_timer_ms) {
          qY_tmp = MAX_uint32_T;
        }

        BmsSupervisor_DW.entry_timer_ms = qY_tmp;
      } else {
        BmsSupervisor_DW.entry_timer_ms = 0U;
      }
    }
  }

  /* End of Chart: '<S8>/UV_Warning_Chart' */

  /* Chart: '<S4>/OV_Warning_Chart' incorporates:
   *  Inport: '<Root>/max_cell_mv'
   *  Inport: '<Root>/step_ms'
   */
  if (BmsSupervisor_DW.is_active_c4_BmsSupervisor == 0) {
    BmsSupervisor_DW.is_active_c4_BmsSupervisor = 1U;
    BmsSupervisor_DW.is_c4_BmsSupervisor = BmsSupervisor_IN_INACTIVE;
    rtb_ov_warning_active = false;
    BmsSupervisor_DW.clear_timer_ms_f = 0U;

    /* Switch: '<S1>/OV_Warning_Mask' incorporates:
     *  Constant: '<S1>/Constant2'
     */
    tmp = 0U;
  } else if (BmsSupervisor_DW.is_c4_BmsSupervisor == BmsSupervisor_IN_ACTIVE) {
    rtb_ov_warning_active = true;
    qY_tmp = BmsSupervisor_DW.clear_timer_ms_f + /*MW:OvSatOk*/
      BmsSupervisor_U.step_ms;
    qY = qY_tmp;
    if (qY_tmp < BmsSupervisor_DW.clear_timer_ms_f) {
      qY = MAX_uint32_T;
    }

    if (rtb_AND && (BmsSupervisor_U.max_cell_mv <= 4000) && (qY >= 1000U)) {
      BmsSupervisor_DW.is_c4_BmsSupervisor = BmsSupervisor_IN_INACTIVE;
      rtb_ov_warning_active = false;
      BmsSupervisor_DW.clear_timer_ms_f = 0U;

      /* Switch: '<S1>/OV_Warning_Mask' incorporates:
       *  Constant: '<S1>/Constant2'
       */
      tmp = 0U;
    } else {
      if (rtb_AND) {
        if (BmsSupervisor_U.max_cell_mv <= 4000) {
          if (qY_tmp < BmsSupervisor_DW.clear_timer_ms_f) {
            qY_tmp = MAX_uint32_T;
          }

          BmsSupervisor_DW.clear_timer_ms_f = qY_tmp;
        } else {
          BmsSupervisor_DW.clear_timer_ms_f = 0U;
        }
      }

      /* Switch: '<S1>/OV_Warning_Mask' incorporates:
       *  Constant: '<S1>/Constant3'
       */
      tmp = 2U;
    }
  } else {
    /* case IN_INACTIVE: */
    rtb_ov_warning_active = false;
    qY_tmp = BmsSupervisor_DW.entry_timer_ms_n + /*MW:OvSatOk*/
      BmsSupervisor_U.step_ms;
    qY = qY_tmp;
    if (qY_tmp < BmsSupervisor_DW.entry_timer_ms_n) {
      qY = MAX_uint32_T;
    }

    if (rtb_AND && (BmsSupervisor_U.max_cell_mv >= 4100) && (qY >= 1000U)) {
      BmsSupervisor_DW.is_c4_BmsSupervisor = BmsSupervisor_IN_ACTIVE;
      rtb_ov_warning_active = true;
      BmsSupervisor_DW.entry_timer_ms_n = 0U;

      /* Switch: '<S1>/OV_Warning_Mask' incorporates:
       *  Constant: '<S1>/Constant3'
       */
      tmp = 2U;
    } else {
      if (rtb_AND) {
        if (BmsSupervisor_U.max_cell_mv >= 4100) {
          if (qY_tmp < BmsSupervisor_DW.entry_timer_ms_n) {
            qY_tmp = MAX_uint32_T;
          }

          BmsSupervisor_DW.entry_timer_ms_n = qY_tmp;
        } else {
          BmsSupervisor_DW.entry_timer_ms_n = 0U;
        }
      }

      /* Switch: '<S1>/OV_Warning_Mask' incorporates:
       *  Constant: '<S1>/Constant2'
       */
      tmp = 0U;
    }
  }

  /* End of Chart: '<S4>/OV_Warning_Chart' */

  /* S-Function (sfix_bitop): '<S1>/Bitwise OR' incorporates:
   *  Switch: '<S1>/OV_Warning_Mask'
   *  Switch: '<S1>/UV_Warning_Mask'
   */
  BmsSupervisor_Y.warning_flags = (uint16_T)((uint32_T)rtb_uv_warning_active |
    tmp);

  /* Chart: '<S7>/UV_Fault_Chart' incorporates:
   *  Inport: '<Root>/min_cell_mv'
   *  Inport: '<Root>/step_ms'
   */
  if (BmsSupervisor_DW.is_active_c2_BmsSupervisor == 0) {
    BmsSupervisor_DW.is_active_c2_BmsSupervisor = 1U;
    BmsSupervisor_DW.is_c2_BmsSupervisor = BmsSupervisor_IN_INACTIVE;
    rtb_uv_fault_active = false;
    BmsSupervisor_DW.clear_timer_ms_p = 0U;
  } else if (BmsSupervisor_DW.is_c2_BmsSupervisor == BmsSupervisor_IN_ACTIVE) {
    rtb_uv_fault_active = true;
    qY_tmp = BmsSupervisor_DW.clear_timer_ms_p + /*MW:OvSatOk*/
      BmsSupervisor_U.step_ms;
    qY = qY_tmp;
    if (qY_tmp < BmsSupervisor_DW.clear_timer_ms_p) {
      qY = MAX_uint32_T;
    }

    if (rtb_AND && (BmsSupervisor_U.min_cell_mv >= 3000) && (qY >= 1000U)) {
      BmsSupervisor_DW.is_c2_BmsSupervisor = BmsSupervisor_IN_INACTIVE;
      rtb_uv_fault_active = false;
      BmsSupervisor_DW.clear_timer_ms_p = 0U;
    } else if (rtb_AND) {
      if (BmsSupervisor_U.min_cell_mv >= 3000) {
        if (qY_tmp < BmsSupervisor_DW.clear_timer_ms_p) {
          qY_tmp = MAX_uint32_T;
        }

        BmsSupervisor_DW.clear_timer_ms_p = qY_tmp;
      } else {
        BmsSupervisor_DW.clear_timer_ms_p = 0U;
      }
    }
  } else {
    /* case IN_INACTIVE: */
    rtb_uv_fault_active = false;
    qY_tmp = BmsSupervisor_DW.entry_timer_ms_m + /*MW:OvSatOk*/
      BmsSupervisor_U.step_ms;
    qY = qY_tmp;
    if (qY_tmp < BmsSupervisor_DW.entry_timer_ms_m) {
      qY = MAX_uint32_T;
    }

    if (rtb_AND && (BmsSupervisor_U.min_cell_mv <= 2800) && (qY >= 500U)) {
      BmsSupervisor_DW.is_c2_BmsSupervisor = BmsSupervisor_IN_ACTIVE;
      rtb_uv_fault_active = true;
      BmsSupervisor_DW.entry_timer_ms_m = 0U;
    } else if (rtb_AND) {
      if (BmsSupervisor_U.min_cell_mv <= 2800) {
        if (qY_tmp < BmsSupervisor_DW.entry_timer_ms_m) {
          qY_tmp = MAX_uint32_T;
        }

        BmsSupervisor_DW.entry_timer_ms_m = qY_tmp;
      } else {
        BmsSupervisor_DW.entry_timer_ms_m = 0U;
      }
    }
  }

  /* End of Chart: '<S7>/UV_Fault_Chart' */

  /* Chart: '<S3>/OV_Fault_Chart' incorporates:
   *  Inport: '<Root>/max_cell_mv'
   *  Inport: '<Root>/step_ms'
   */
  if (BmsSupervisor_DW.is_active_c5_BmsSupervisor == 0) {
    BmsSupervisor_DW.is_active_c5_BmsSupervisor = 1U;
    BmsSupervisor_DW.is_c5_BmsSupervisor = BmsSupervisor_IN_INACTIVE;
    BmsSupervisor_DW.clear_timer_ms_k = 0U;

    /* Switch: '<S1>/OV_Fault_Mask' incorporates:
     *  Constant: '<S1>/Constant4'
     */
    tmp = 0U;
  } else if (BmsSupervisor_DW.is_c5_BmsSupervisor == BmsSupervisor_IN_ACTIVE) {
    qY_tmp = BmsSupervisor_DW.clear_timer_ms_k + /*MW:OvSatOk*/
      BmsSupervisor_U.step_ms;
    qY = qY_tmp;
    if (qY_tmp < BmsSupervisor_DW.clear_timer_ms_k) {
      qY = MAX_uint32_T;
    }

    if (rtb_AND && (BmsSupervisor_U.max_cell_mv <= 4150) && (qY >= 1000U)) {
      BmsSupervisor_DW.is_c5_BmsSupervisor = BmsSupervisor_IN_INACTIVE;
      BmsSupervisor_DW.clear_timer_ms_k = 0U;

      /* Switch: '<S1>/OV_Fault_Mask' incorporates:
       *  Constant: '<S1>/Constant4'
       */
      tmp = 0U;
    } else {
      if (rtb_AND) {
        if (BmsSupervisor_U.max_cell_mv <= 4150) {
          if (qY_tmp < BmsSupervisor_DW.clear_timer_ms_k) {
            qY_tmp = MAX_uint32_T;
          }

          BmsSupervisor_DW.clear_timer_ms_k = qY_tmp;
        } else {
          BmsSupervisor_DW.clear_timer_ms_k = 0U;
        }
      }

      /* Switch: '<S1>/OV_Fault_Mask' incorporates:
       *  Constant: '<S1>/Constant5'
       */
      tmp = 2U;
    }
  } else {
    /* case IN_INACTIVE: */
    qY_tmp = BmsSupervisor_DW.entry_timer_ms_f + /*MW:OvSatOk*/
      BmsSupervisor_U.step_ms;
    qY = qY_tmp;
    if (qY_tmp < BmsSupervisor_DW.entry_timer_ms_f) {
      qY = MAX_uint32_T;
    }

    if (rtb_AND && (BmsSupervisor_U.max_cell_mv >= 4250) && (qY >= 500U)) {
      BmsSupervisor_DW.is_c5_BmsSupervisor = BmsSupervisor_IN_ACTIVE;
      BmsSupervisor_DW.entry_timer_ms_f = 0U;

      /* Switch: '<S1>/OV_Fault_Mask' incorporates:
       *  Constant: '<S1>/Constant5'
       */
      tmp = 2U;
    } else {
      if (rtb_AND) {
        if (BmsSupervisor_U.max_cell_mv >= 4250) {
          if (qY_tmp < BmsSupervisor_DW.entry_timer_ms_f) {
            qY_tmp = MAX_uint32_T;
          }

          BmsSupervisor_DW.entry_timer_ms_f = qY_tmp;
        } else {
          BmsSupervisor_DW.entry_timer_ms_f = 0U;
        }
      }

      /* Switch: '<S1>/OV_Fault_Mask' incorporates:
       *  Constant: '<S1>/Constant4'
       */
      tmp = 0U;
    }
  }

  /* End of Chart: '<S3>/OV_Fault_Chart' */

  /* Switch: '<S1>/Measurement_Invalid_Mask' incorporates:
   *  Constant: '<S1>/Constant10'
   *  Constant: '<S1>/Constant11'
   *  Inport: '<Root>/measurement_valid'
   *  Logic: '<S2>/NOT'
   */
  if (!BmsSupervisor_U.measurement_valid) {
    tmp_0 = 4U;
  } else {
    tmp_0 = 0U;
  }

  /* Switch: '<S1>/Measurement_Stale_Mask' incorporates:
   *  Constant: '<S1>/Constant12'
   *  Constant: '<S1>/Constant13'
   *  Inport: '<Root>/measurement_stale'
   */
  if (BmsSupervisor_U.measurement_stale) {
    tmp_1 = 8U;
  } else {
    tmp_1 = 0U;
  }

  /* S-Function (sfix_bitop): '<S1>/Bitwise OR3' incorporates:
   *  S-Function (sfix_bitop): '<S1>/Bitwise OR1'
   *  S-Function (sfix_bitop): '<S1>/Bitwise OR2'
   *  Switch: '<S1>/Measurement_Invalid_Mask'
   *  Switch: '<S1>/Measurement_Stale_Mask'
   *  Switch: '<S1>/OV_Fault_Mask'
   *  Switch: '<S1>/UV_Fault_Mask'
   */
  BmsSupervisor_Y.fault_flags = (uint16_T)((uint32_T)rtb_uv_fault_active | tmp |
    (uint16_T)(tmp_0 | tmp_1));

  /* Chart: '<Root>/Supervisor_Stateflow' */
  if (BmsSupervisor_DW.is_active_c3_BmsSupervisor == 0) {
    BmsSupervisor_DW.is_active_c3_BmsSupervisor = 1U;
    if (!rtb_AND) {
      BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_SENSOR_ERROR;
      BmsSupervisor_Y.state = 4U;
    } else if (BmsSupervisor_Y.warning_flags != 0) {
      BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_FAULT;
      BmsSupervisor_Y.state = 3U;
    } else if (BmsSupervisor_Y.fault_flags != 0) {
      BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_WARNING;
      BmsSupervisor_Y.state = 2U;
    } else {
      BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_NORMAL;
      BmsSupervisor_Y.state = 1U;
    }
  } else {
    switch (BmsSupervisor_DW.is_c3_BmsSupervisor) {
     case BmsSupervisor_IN_FAULT:
      BmsSupervisor_Y.state = 3U;
      if (!rtb_AND) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_SENSOR_ERROR;
        BmsSupervisor_Y.state = 4U;
      } else if ((BmsSupervisor_Y.warning_flags == 0) &&
                 (BmsSupervisor_Y.fault_flags != 0)) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_WARNING;
        BmsSupervisor_Y.state = 2U;
      } else if ((BmsSupervisor_Y.warning_flags == 0) &&
                 (BmsSupervisor_Y.fault_flags == 0)) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_NORMAL;
        BmsSupervisor_Y.state = 1U;
      }
      break;

     case BmsSupervisor_IN_NORMAL:
      BmsSupervisor_Y.state = 1U;
      if (!rtb_AND) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_SENSOR_ERROR;
        BmsSupervisor_Y.state = 4U;
      } else if (BmsSupervisor_Y.warning_flags != 0) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_FAULT;
        BmsSupervisor_Y.state = 3U;
      } else if (BmsSupervisor_Y.fault_flags != 0) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_WARNING;
        BmsSupervisor_Y.state = 2U;
      }
      break;

     case BmsSupervisor_IN_SENSOR_ERROR:
      BmsSupervisor_Y.state = 4U;
      if (rtb_AND && (BmsSupervisor_Y.warning_flags != 0)) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_FAULT;
        BmsSupervisor_Y.state = 3U;
      } else if (rtb_AND && (BmsSupervisor_Y.warning_flags == 0) &&
                 (BmsSupervisor_Y.fault_flags != 0)) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_WARNING;
        BmsSupervisor_Y.state = 2U;
      } else if (rtb_AND && (BmsSupervisor_Y.warning_flags == 0) &&
                 (BmsSupervisor_Y.fault_flags == 0)) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_NORMAL;
        BmsSupervisor_Y.state = 1U;
      }
      break;

     default:
      /* case IN_WARNING: */
      BmsSupervisor_Y.state = 2U;
      if (!rtb_AND) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_SENSOR_ERROR;
        BmsSupervisor_Y.state = 4U;
      } else if (BmsSupervisor_Y.warning_flags != 0) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_FAULT;
        BmsSupervisor_Y.state = 3U;
      } else if (BmsSupervisor_Y.fault_flags == 0) {
        BmsSupervisor_DW.is_c3_BmsSupervisor = BmsSupervisor_IN_NORMAL;
        BmsSupervisor_Y.state = 1U;
      }
      break;
    }
  }

  /* End of Chart: '<Root>/Supervisor_Stateflow' */

  /* RelationalOperator: '<S12>/Compare' incorporates:
   *  Constant: '<S12>/Constant'
   */
  rtb_AND = (BmsSupervisor_Y.state == 2);

  /* RelationalOperator: '<S11>/Compare' incorporates:
   *  Constant: '<S11>/Constant'
   */
  rtb_uv_fault_active = (BmsSupervisor_Y.state == 1);

  /* Outport: '<Root>/charge_allowed_shadow' incorporates:
   *  Logic: '<S5>/Charge_Allowed'
   *  Logic: '<S5>/Not_OV_Warning'
   *  Logic: '<S5>/UV_Only'
   *  Logic: '<S5>/Warning_Charge_Allowed'
   */
  BmsSupervisor_Y.charge_allowed_shadow = ((rtb_uv_warning_active &&
    !rtb_ov_warning_active && rtb_AND) || rtb_uv_fault_active);

  /* Outport: '<Root>/discharge_allowed_shadow' incorporates:
   *  Logic: '<S5>/Discharge_Allowed'
   *  Logic: '<S5>/Not_UV_Warning'
   *  Logic: '<S5>/OV_Only'
   *  Logic: '<S5>/Warning_Discharge_Allowed'
   */
  BmsSupervisor_Y.discharge_allowed_shadow = ((!rtb_uv_warning_active &&
    rtb_ov_warning_active && rtb_AND) || rtb_uv_fault_active);
}

/* Model initialize function */
void BmsSupervisor_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void BmsSupervisor_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
